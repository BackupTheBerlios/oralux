/*
 * SOFTSPEECH for Light-Lia_Phon
 *
 * $Id: softspeech.c,v 1.1 2004/09/27 20:30:03 gcasse Exp $
 *
 * Derived by Phil's Free alias Phil Buch      July 20, 2003
 * Contact : biglux@culte.org
 *
 * From:
 * ======================================================================
 * Softspeech - A wrapper around freephone/hadifix and mbrola
 * Copyright (C) 1998 Roger Butenuth <butenuth@uni-paderborn.de>
 * For the latest version see:
 * http://www.uni-paderborn.de/cs/heiss/blinux/index.html
 * Id: speech.c,v 1.2 1998/12/15 20:59:56 butenuth
 * ======================================================================
 * 6 April 2004: Gilles Casse 
 * The code can now filter any string starting with '[' and ending with ']'
 * The list of these strings are stored in the array named delims
 * function strip_np: modified
 * function search_np: added
 * 6 April 2004: GC: adding ' :pu' to the list.
 * --
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <errno.h>
#include <string.h> /* for strsignal */
#include <ctype.h>

#define DWRITE          1   /* write phonemes to stderr */
#define HADIFIX_DOT_BUG 1   /* activate bugfix for hadifix */

#define SPEECH_DIR "/bin"

#define LANG_GERMAN          0
#define LANG_FRENCH          1
#define LANG_BRITISH_ENGLISH 2

#define LANGUAGES            3

#define DEFAULT_LANGUAGE     LANG_FRENCH

typedef struct lang_struct {
    char   *t2pho_argv[10];   /* command line for phoneme generator */
    char   *mbrola_argv[10];   /* command line for mbrola */
    double pitch;      /* ";; F=" parameter (frequency) of mbrola */
    double time;      /* ";; T=" parameter (time) of mbrola */
    int    volume;      /* parameter for /dev/mixer */
    char   *flush;    /* ";; FLUSH " parameter of mbrola */
    char   *lang;     /* for -lang option */
} lang_t;


lang_t language[LANGUAGES] = {
 {            /* german */
   { "txt2pho", "-p", "/usr/local/share/mbrola/txt2pho/data/", NULL },
   { "mbrola", "-e", "/usr/local/share/mbrola/voices/de4", "-", "-", NULL },
   1.0,
   1.0,
   75,
   "#",
   "de"
 },
 {            /* french */
#define LLP_MBR_INITF_POS 3
#define FR_VOICE_POS      4
   { "lliaphon", NULL },
   { "mbrola", "-e", "-I", "data/noarch/initfile.lia",
               "/usr/local/share/mbrola/voices/fr4", "-", "-", NULL },
   1.4,
   0.6,
   75,
   "!",
   "fr"
 },
 {            /* british english */
   { "freephone", "-h", "lib/lexicon", NULL },
   { "./mbrola", "-e", "en1/en1", "-", "-", NULL },
   1.0,
   1.0,
   75,
   "#",
   "en"
 }
};

int current_language;

#define INPUT_FIFO_SIZE 80000

typedef struct {
    int   in;         /* next write position */
    int   out;         /* next read position */
    int   count;      /* bytes in buffer */
    char  data[INPUT_FIFO_SIZE];
} input_fifo_t;


/*
 * When we read from the phoneme generator, we put only complete lines
 * into the fifo.
 * Whenever we got more than a line or a partial line, this partial
 * line is in the "phoneme_buf"
 * "kill_line" is set to true on a speech pipe clear when there is 
 * partial line in "phoneme_buf". It is a hint to read_from_t2pho to
 * throw away this line on the next read.
 */

#define PHONEME_BUF_SIZE        4096
#define PHONEME_LINE_LEN         100 /* max linelength for phonemes */
#define PHONEME_FIFO_SIZE        200 /* in lines */
#define PHONEME_FIFO_CMD_SIZE     500 /* in lines */

char phoneme_buf[PHONEME_BUF_SIZE];
int  phoneme_buf_count;
int  kill_line; /* Mis à 0 par :
                 * - initialize
                 * - read_from_t2pho après détection d'1 '\n'
                 * Mis à 1 par :
                 * - clear_speech_pipe si phoneme_buf_count différent de 0
                 */

char line_fragment[PHONEME_LINE_LEN];
char fragment_len;

typedef struct {
    int  in;         /* next write position */
    int  out;         /* next read position */
    int  count;         /* lines in buffer */
    char data[PHONEME_FIFO_SIZE][PHONEME_LINE_LEN];
    int  len[PHONEME_FIFO_SIZE];
    int  cmd_in;      /* next write position */
    int  cmd_out;      /* next read position */
    int  cmd_count;      /* number of stored mbrola commands */
    char cmd_data[PHONEME_FIFO_CMD_SIZE][PHONEME_LINE_LEN];
    int  cmd_len[PHONEME_FIFO_SIZE];
} phoneme_fifo_t;


#define AUDIO_FIFO_SIZE 32000   /* measured in bytes, 2 seconds */
            /* (2 bytes per sample, 16k samples/s */

typedef struct {
    int   in;         /* next write position */
    int   out;         /* next read position */
    int   count;      /* bytes in buffer */
    char  data[AUDIO_FIFO_SIZE];
} audio_fifo_t;


void initialize(int argc, char *argv[]);
void main_loop(void);
void new_language(int lang);
void clear_speech_pipe(void);

void   write_to_input_fifo(const char *str, int len);
void   read_from_input(int back_from_index);
void   lire_entree(int back_from_index);
double time_diff(struct timeval *t);

void write_to_t2pho(void);
void read_from_t2pho(void);

int  insert_mbrola_command(char *command);
void write_to_mbrola(void);
void read_from_mbrola(void);

void write_to_audio(void);
int  open_audio(void);
int  audio_write_size(int fd);
int  silence(int fd, double duration);

int  restart_t2pho(lang_t *lang, int restart);
int  restart_mbrola(lang_t *lang, int restart);
int  start_program(char *argv[], int *to, int *from, int *pid);
ssize_t rep_read(int fd, void *buf, size_t count);
ssize_t rep_write(int fd, const void *buf, size_t count);
void signal_handler(int signal);
void sigchild_handler(int signal);
void dump_state(void);
char *translate(char *mot);


int from_input, to_t2pho, from_t2pho, to_mbrola, from_mbrola, to_audio;
int mix_fd;
int from_input_enable; /* mis à 1 par :
                        * - initialize
                        * - clear_speech_pipe
                        * - write_to_t2pho si input_fifo.count < (INPUT_FIFO_SIZE / 2)
                        * remis à 0 par :
                        * - main_loop si une scrutation de from_input indique que l'entrée est close
                        * - write_to_input_fifo si input_fifo.count >= (INPUT_FIFO_SIZE / 2)
                        * - lire_entree si rep_read retourne 0 ou moins
                        */

int to_t2pho_enable; /* mis à 1 par :
                      * - write_to_input_fifo si input_fifo.count > 0
                      * remis à 0 par :
                      * - write_to_t2pho si input_fifo.count redevient nul
                      * - restart_t2pho
                      */
int from_t2pho_enable; /* mis à 1 par :
                        * - clear_speech_pipe
                        * - write_to_mbrola si phoneme_fifo.count redevient inférieur à PHONEME_FIFO_SIZE
                        * remis à 0 par :
                        * - read_from_t2pho si phoneme_fifo.count atteint PHONEME_FIFO_SIZE
                        * - restart_t2pho
                        */
int to_mbrola_enable; /* mis à 1 par :
                       * - read_from_t2pho si phoneme_fifo.count ou phoneme_fifo.cmd_count non nul
                       * - insert_mbrola_command si buffer de commande pas plein
                       * remis à 0 par :
                       * - write_to_mbrola si phoneme_fifo.count et phoneme_fifo.cmd_count nuls
                       * - restart_mbrola
                       */

int from_mbrola_enable; /* mis à 1 par :
                         * - clear_speech_pipe
                         * - restart_mbrola
                         */

int mbrola_flushed; /* mis à 1 par :
                     * - clear_speech_pipe (après signal SIGUSR1 envoyé à Mbrola)
                     * - main_loop après insertion d'une commande flush
                     *   (si rien dans la file d'entrée input_fifo
                     *    et si
                     *    last_t2pho_time pas mis à jour depuis moins 1/10ème)
                     *   de seconde
                     * - restart_t2pho
                     * remis à 0 par :
                     * - read_from_t2pho si au moins 1 ligne a été lue du phonétiseur
                     * - insert_mbrola_command
                     */
int audio_flushed; /* mis à 1 par :
                    * - initialize
                    * - clear_speech_pipe
                    * - main_loop après appel de audio_write_size
                    * remis à 0 par :
                    * - write_to_audio
                    */

struct timeval last_input_time;
struct timeval last_t2pho_time; /* Mis à jour par :
                                 * - read_from_t2pho si au moins un phonème a été lu
                                 * - insert_mbrola_command
                                 * - restart_t2pho
                                 */
struct timeval last_audio_time;

int t2pho_pid, mbrola_pid;

int index_mark = 0;

input_fifo_t  input_fifo;
phoneme_fifo_t phoneme_fifo;
audio_fifo_t  audio_fifo;

#define QSIZE 10000
static char queued[QSIZE + 2]; // Tampon d'acquisition d'un texte mis en file
                               // avant commande (d) de vidage de cette file
static int  in_queue;

/*
 * ----------------------------------------------------------------------
 * Main. Open log file (as long as we are in beta phase),
 * initialize, loop...
 * ----------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
    struct sigaction sig_act;
    int              ilang;
    char             *home;
    char             *logf;
    char             *lliaphon_dir;
    char             *lliaphon_initf;
    char             *fr_voice;
#define LOGF ".softspeech.log"

    home = getenv("HOME");
    assert(home != NULL);
    logf = malloc(strlen(home) + strlen(LOGF) + 2);
    assert(logf != NULL);
    sprintf(logf, "%s/%s", home, LOGF);
    if (freopen(logf, "w", stderr) == NULL) {
       fprintf(stderr, "impossible d'ouvrir %s\n", logf);
       freopen("/dev/null", "w", stderr);
    } else {
       chmod(logf, S_IWUSR|S_IRUSR);
    }
    setlinebuf(stderr);

    signal(SIGINT,  signal_handler);
    signal(SIGTERM, signal_handler);

    sig_act.sa_handler = sigchild_handler;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags    =  SA_NOCLDSTOP;
    sig_act.sa_restorer = NULL;
    sigaction(SIGCHLD, &sig_act, NULL);

    /*
     * -lang option : just for unit test
     */
    if (argc == 3 && strcmp(argv[1], "-lang") == 0) {
       for (ilang = 0; ilang < LANGUAGES; ilang++) {
           if (strcmp(argv[2], language[ilang].lang) == 0) {
              current_language = ilang;
              break;
           }
       }
       if (ilang == LANGUAGES)
          current_language = DEFAULT_LANGUAGE;
    } else {
       current_language = DEFAULT_LANGUAGE;
    }
    fprintf(stderr, "main: current_language = %d\n", current_language);

    /* 
     * French phonetizer initialization
     */
    lliaphon_dir = getenv("LLIAPHON");
    if (lliaphon_dir == NULL) {
       fprintf(stderr, "Variable d'environnement LLIAPHON non définie\n");
       exit(1);
    } else {
       lliaphon_initf = malloc(strlen(language[LANG_FRENCH].mbrola_argv[LLP_MBR_INITF_POS])
                               + strlen(lliaphon_dir) + 2);
       assert(lliaphon_initf != NULL);
       sprintf(lliaphon_initf, "%s/%s",
               lliaphon_dir,
               language[LANG_FRENCH].mbrola_argv[LLP_MBR_INITF_POS]
              );
       language[LANG_FRENCH].mbrola_argv[LLP_MBR_INITF_POS] =  lliaphon_initf;
    }
    fr_voice = getenv("PM_FR_VOICE");
    if (fr_voice != NULL)
       language[LANG_FRENCH].mbrola_argv[FR_VOICE_POS] = fr_voice ;
    
    /*
     * Go on !
     */
    initialize(argc, argv);

    silence(to_audio, 0.1);
    ioctl(to_audio, SNDCTL_DSP_SYNC);

    main_loop();

    return 0;
}


/*
 * ----------------------------------------------------------------------
 * Setup pipes and start programs.
 * ----------------------------------------------------------------------
 */
void initialize(int argc, char *argv[])
{
fprintf(stderr, "initialize\n");
    /*
     * Start the text-to-phoneme converter.
     */
    fprintf(stderr, "initialize: current_language = %d\n", current_language);
    restart_t2pho(&language[current_language], 0);

    kill_line = 0;
    /*
     * Start the mbrola phoneme synthesizer. 
     */
    restart_mbrola(&language[current_language], 0);
    /*
     * Initialize audio/mixer device and fifo
     */
    to_audio = open_audio();
    assert(to_audio >= 0);
    mix_fd = open("/dev/mixer", O_RDWR);
    assert(mix_fd >= 0);

    audio_fifo.in    = 0;
    audio_fifo.out   = 0;
    audio_fifo.count = 0;

    gettimeofday(&last_input_time, NULL);
    audio_flushed = 1;

    from_input = 0;   /* we read from stdin */
    from_input_enable = 1;   /* enable input */
}


/*
 * ----------------------------------------------------------------------
 * Wait for events and handle them. This function never returns, 
 * termination is done via exit, not return in main.
 * ----------------------------------------------------------------------
 */
void main_loop(void)
{
    fd_set          rfds, wfds, efds;
    int             retval;
    int             max_write_size, write_size, block_size, t;
    struct timeval  tv;
    int             max_fd;
    char            com_str[50];

fprintf(stderr, "main_loop\n");
    assert(ioctl(to_audio, SNDCTL_DSP_GETBLKSIZE, &block_size) >= 0);
    max_write_size = audio_write_size(to_audio);
    
    do {
       if (index_mark != 0
           && audio_flushed
           && mbrola_flushed
           && input_fifo.count == 0
           && audio_fifo.count == 0
           && phoneme_fifo.count == 0
           && time_diff(&last_input_time) > 0.2
          ) {
           printf("@I%d\n", index_mark - 1); fflush(stdout);
           index_mark = 0;
           lire_entree(1);
       }

       if (!mbrola_flushed &&
           input_fifo.count == 0
           && phoneme_fifo.count == 0
           && time_diff(&last_t2pho_time) > 0.1
          ) {
          sprintf(com_str, "%s\n", language[current_language].flush);
          (void)insert_mbrola_command(com_str);
          mbrola_flushed = 1;
       }
       if (!audio_flushed
           && input_fifo.count == 0
           && audio_fifo.count == 0
           && phoneme_fifo.count == 0
           && time_diff(&last_audio_time) > 0.2
           && audio_write_size(to_audio) >= block_size
          ) {
          /*
           * It should be o.k. to do a DSP_POST, but this seems to
           * be implemented buggy in the standard kernel sound driver.
           */
          assert(ioctl(to_audio, SNDCTL_DSP_SYNC) >= 0);
          audio_flushed = 1;
       }

       FD_ZERO(&rfds);
       FD_ZERO(&wfds);
       FD_ZERO(&efds);

       if (from_input_enable && index_mark == 0) {
          FD_SET(from_input, &rfds);
       }
       if (to_t2pho_enable) 
          FD_SET(to_t2pho, &wfds);
       if (from_t2pho_enable)
          FD_SET(from_t2pho, &rfds);
       if (to_mbrola_enable)
          FD_SET(to_mbrola, &wfds);
       if (from_mbrola_enable)
          FD_SET(from_mbrola, &rfds);

       FD_SET(from_input,  &efds);
       FD_SET(to_t2pho,    &efds);
       FD_SET(from_t2pho,  &efds);
       FD_SET(to_mbrola,   &efds);
       FD_SET(from_mbrola, &efds);
       FD_SET(to_audio,    &efds);

       if (audio_fifo.count > 0) {
          /*
           * The timeout should not be too short (wasting cpu power), but
           * not to long (underrun in audio buffer). Compute number of
           * bytes in audio buffer (== 2 * samples) and time t to play these
           * in microseconds. Divide this time by X to be on the save side.
           */
          write_size = audio_write_size(to_audio);
          t = (max_write_size - write_size) * 1e6 / 32000;
          t /= 4;
          tv.tv_sec  = t / 1000000;
          tv.tv_usec = t % 1000000;
       } else {
          /*
           * @@@
           */
          tv.tv_sec  = 0;
          tv.tv_usec = 50 * 1000;
       }

       max_fd = from_input;
       if (from_t2pho  > max_fd) max_fd = from_t2pho;
       if (from_mbrola > max_fd) max_fd = from_mbrola;
       if (to_t2pho    > max_fd) max_fd = to_t2pho;
       if (to_mbrola   > max_fd) max_fd = to_mbrola;
       if (to_audio    > max_fd) max_fd = to_audio;

       retval = select(max_fd + 1, &rfds, &wfds, &efds, &tv);
       if (retval == -1) {
           if (errno == EINTR) {
              fprintf(stderr, "interrupted select\n");
              continue;
           }
           perror("select");
       }
       if (FD_ISSET(from_input, &efds)) {
           from_input_enable = 0;
           fprintf(stderr, "input closed\n");
           signal_handler(0);
       }
       if (FD_ISSET(to_t2pho, &efds) || FD_ISSET(from_t2pho, &efds)) {
           if (FD_ISSET(from_t2pho, &efds))
              fprintf(stderr, "exception on from_t2pho\n");
           if (FD_ISSET(to_t2pho, &efds))
              fprintf(stderr, "exception on to_t2pho\n");
           dump_state();
           fprintf(stderr, "restarting t2pho\n");
           restart_t2pho(&language[current_language], 1);
           fprintf(stderr, "t2pho restarted\n");
       }
       if (FD_ISSET(to_mbrola, &efds) || FD_ISSET(from_mbrola, &efds)) {
           if (FD_ISSET(from_mbrola, &efds))
              fprintf(stderr, "exception on from_mbrola\n");
           if (FD_ISSET(to_mbrola, &efds))
              fprintf(stderr, "exception on to_mbrola\n");
           dump_state();
           fprintf(stderr, "restarting mbrola\n");
           restart_mbrola(&language[current_language], 1);
           fprintf(stderr, "mbrola restarted\n");
       }
       if (FD_ISSET(to_audio, &efds)) {
           fprintf(stderr, "exception on to_audio\n");
           dump_state();
           signal_handler(0);
       }
       /*
        * Ordering of the following event handling is not chosen randomly:
        * The pipeline is handled "from the back", assuming that the 
        * back end is most time consuming (audio: 32000 bytes/second).
        */
       if (audio_write_size(to_audio) > 0)
           write_to_audio();

       if (FD_ISSET(from_mbrola, &rfds))
           read_from_mbrola();
       if (FD_ISSET(to_mbrola, &wfds))
           write_to_mbrola();

       if (FD_ISSET(from_t2pho, &rfds))
           read_from_t2pho();
       if (FD_ISSET(to_t2pho, &wfds))
           write_to_t2pho();
       if (FD_ISSET(from_input, &rfds))
           lire_entree(0);

    } while (1);
}


/*
 * ----------------------------------------------------------------------
 *
 * ----------------------------------------------------------------------
 */
void new_language(int lang)
{
    int param;

    if (lang < 0 || lang >= LANGUAGES || lang == current_language)
       return;

    clear_speech_pipe();

    if (lang != current_language) {
       current_language = lang;
       fprintf(stderr, "new_language: current_language = %d\n", current_language);
       fprintf(stderr, "restarting t2pho\n");
       restart_t2pho(&language[current_language], 1);
       fprintf(stderr, "t2pho restarted\n");
       fprintf(stderr, "restarting mbrola\n");
       restart_mbrola(&language[current_language], 1);
       fprintf(stderr, "mbrola restarted\n");

       param = language[current_language].volume;
       param = param | (param << 8);
       (void)ioctl(mix_fd, MIXER_WRITE(SOUND_MIXER_PCM), &param);
    }
}


/*
 * ----------------------------------------------------------------------
 * Try to throw away everything in the speech pipe. Care must be taken
 * with partially filled lines between the phoneme generator and mbrola:
 * Throwing them away can cause syntax errors.
 * Handling of the SIGUSR1 signal by mbrola needs at least version 3.01g1.
 * The command buffer is not cleared!
 * ----------------------------------------------------------------------
 */
void clear_speech_pipe(void)
{
    char            com_str[50];

    fprintf(stderr, "clear_speech_pipe BEGIN\n");
    kill(mbrola_pid, SIGUSR1);

    from_input_enable = 1;
    from_t2pho_enable = 1;
    from_mbrola_enable = 1;
    mbrola_flushed = 1;
    audio_flushed = 1;

    input_fifo.in    = 0;
    input_fifo.out   = 0;
    input_fifo.count = 0;

    in_queue = 0;
    *queued = '\0';

    if (phoneme_buf_count != 0) {
       fprintf(stderr, "setting kill_line = 1\n");
       kill_line = 1;
    }

    phoneme_fifo.in    = 0;
    phoneme_fifo.out   = 0;
    phoneme_fifo.count = 0;

    assert((audio_fifo.count & 1) == 0);
    audio_fifo.in    = 0;
    audio_fifo.out   = 0;
    audio_fifo.count = 0;
    ioctl(to_audio, SNDCTL_DSP_RESET);

    sprintf(com_str, "%s\n", language[current_language].flush);
    (void)insert_mbrola_command(com_str);
    if (index_mark != 0) {
       printf("@I%d\n", index_mark - 1); fflush(stdout);
       index_mark = 0;
    }
    fprintf(stderr, "clear_speech_pipe END\n");
}


/*
 * ----------------------------------------------------------------------
 * Write a line of data to the input fifo. This may cause an overflow
 * when the input is expanded more than 100% by the parser for a longer
 * period (half of the INPUT_FIFO_SIZE).
 * ----------------------------------------------------------------------
 */
void write_to_input_fifo(const char *str, int len)
{
    int max_s;   /* copy max_s bytes */

    if (input_fifo.in >= input_fifo.out) {
       max_s = INPUT_FIFO_SIZE - input_fifo.in;
       if (max_s > INPUT_FIFO_SIZE - input_fifo.count)
           max_s = INPUT_FIFO_SIZE - input_fifo.count;
       if (max_s > len)
           max_s = len;
       memcpy(&input_fifo.data[input_fifo.in], str, max_s);
       str += max_s;
       len -= max_s;
       input_fifo.count += max_s;
       input_fifo.in    += max_s;
       if (input_fifo.in == INPUT_FIFO_SIZE)
           input_fifo.in = 0;
    }
    if (input_fifo.in < input_fifo.out) {
       max_s = INPUT_FIFO_SIZE - input_fifo.count;
       if (max_s > len)
           max_s = len;
       memcpy(&input_fifo.data[input_fifo.in], str, max_s);
       input_fifo.count += max_s;
       input_fifo.in    += max_s;
       if (input_fifo.in == INPUT_FIFO_SIZE)
           input_fifo.in = 0;
       str += max_s;
       len -= max_s;
    }
    if (input_fifo.count >= INPUT_FIFO_SIZE / 2) {
       from_input_enable = 0;
    }
    if (input_fifo.count > 0)
       to_t2pho_enable = 1;
    
    if (len > 0)
       fprintf(stderr, "input overflow\n");
}


/*
 * ----------------------------------------------------------------------
 * Parse and execute a command. The @ and letter introducing are already
 * removed from the line.
 * Return value is the number of characters read.
 * ----------------------------------------------------------------------
 */
int do_command(int command, const char *line, int len)
{
    char   com_str[40];
    double dval;
    int    i;
    int    param;

    i = 0;
    param = 0;
    while ('0' <= line[i] && line[i] <= '9' && i < len) {
       param = 10 * param + line[i] - '0';
       i++;
    }

    fprintf(stderr, "do_command: Commande (%c) \t (%d)\n",
                    command, param);
    switch (command) {
        case 'A':   /* volume */
           if (param > 100)
               param = 100;
           language[current_language].volume = param;
           param = param | (param << 8);
           (void)ioctl(mix_fd, MIXER_WRITE(SOUND_MIXER_PCM), &param);
           break;
        case 'W':   /* speed = 1 / time */
           if (param == 0)
               break;
           dval = 100.0 / (double)param;
           language[current_language].time = dval;
           sprintf(com_str, ";; T=%g\n", dval);
           insert_mbrola_command(com_str);
           break;
        case 'F':   /* frequency (pitch) */
           if (param == 0)
               break;
           dval = (double)param / 100.0;
           language[current_language].pitch = dval;
           sprintf(com_str, ";; F=%g\n", dval);
           insert_mbrola_command(com_str);
           break;
        case 'I':
           /*
            * The user can specify 0 as mark label, but that is our flag
            * for no mark active, for this reason, add 1.
            */
           index_mark = param + 1;
           break;
        case '=':   /* select language */
           new_language(param - 1);
           break;
        default:   /* ignore (bad parameter) */
           fprintf(stderr, "do_command: mauvaise commande (%c(%d))\n",
                           command, param);
           break;
    }

    return i;
}


/*
 * ----------------------------------------------------------------------
 * We have a complete line, without a speech flush command in it.
 * ----------------------------------------------------------------------
 */
void parse_line(const char *line, int len, int *parsed)
{
    int  start, i;
    int  command;

    fprintf(stderr, "parse_line: ligne (%.*s), longueur=%d\n",
                    len, line, len);

    start = 0;
    i     = 0;
    while (i < len && index_mark == 0) {
       while (i < len && line[i] != '@')
           i++;
       write_to_input_fifo(&line[start], i - start);
       write_to_input_fifo("\n", 1);
       if (current_language != LANG_GERMAN)
           write_to_input_fifo("\n", 1);
       if (i < len) {
           /*
            * We have found a '@', let's look for the command behind it
            */
           if (i + 2 < len) {
              i++;      /* skip the '@' */
              command = line[i++]; /* get and skip command letter */
              i += do_command(command, &line[i], len - i);
           }      
       }
       start = i;
    }
    *parsed = i;
}


/*
 * Description des chaines actuellement filtrées
 */
static char delims_start = '[';
static char delims_end = ']';
static char *delims[3] = { ":np", " :dv", ":nh", " :pu" };
static int ldelims[3] = { sizeof(delims[0])-1, sizeof(delims[1])-1, sizeof(delims[2])-1 };

/*
 * search_np() : recherche la chaine (":np",...) jusqu'à son terminateur ']' 
 * et retourne l'adresse du caractere suivant cette chaine
 * NULL est retourné si aucune chaine n'a été reconnue
 * pcur: en entrée pointe sur le premier caractere de la chaine ('[')
 */
char* search_np(char* pcur)
{
  int idelim = 0;
  char* pdeb=pcur;
  char* pnew=NULL;

  for (idelim = 0; idelim < sizeof(delims) / sizeof(delims[0]); idelim++ ) {
    pcur=pdeb+1; 
    
    // le délimiteur recherché est peut-être trouvé
    if (strncmp(pcur, delims[idelim], ldelims[idelim]) == 0) {
      pcur += ldelims[idelim];
      
      // Recherche du terminateur
      while (*pcur && (*pcur != delims_end)) {
	++pcur;
      }
      
      if (*pcur == delims_end) {
	pnew=++pcur; // Ok!
	break;
      }
    }
  }
  return pnew;
}


/*
 * strip_np() : élimine des décorations Non Prononçables envoyées par Emacspeak
 *              [:np ... ]
 */
void strip_np(char * param)
{
  char        *pnew = NULL;
  char        *pcur = param;

  if (! param) {
      return;
  }

  while (*pcur) {
    if ((*pcur == delims_start) 
      && (pnew = search_np(pcur))) {
      memmove(pcur, pnew, strlen(pnew) + 1);
      //      fprintf(stderr, "paramètre nettoyé : %s\n", param);
      pcur--; // traiter le nouveau caractere (issu de la recopie). 
    }
    pcur++;
  }
}

/*
 * get_param() : retourne un paramètre (trouvé entre 2 accolades)
 *               - parsed : indique en sortie le nombre d'octets scrutés jusqu'à
 *                          la détection de la fermeture du paramètre ou à
 *                          à l'épuisement du texte en entrée;
 *                          en entrée, si non nul, indique que la capture du
 *                          paramètre a déjà commencé dans une précédente
 *                          invocation.
 *               - closed : en sortie, indique si la fermeture du paramètre a
 *                          été trouvée
 *                          * -1 si erreur détectée
 *                          *  0 si analyse de paramètre pas terminée
 *                          *  1 si analyse de paramètre terminée
 */
static char *get_param(const char *text, int len, int *parsed, int *closed)
{
#define PARAM_MODULO 1024
    static char *param;
    static int  param_len;
    int         inpar = 0;
    int         ipos = 0;
    int         opos = 0;

#ifdef DEBUG
    fprintf(stderr, "get_param(text=#%.*s# \t len=%d, *parsed=%d)\n",
                    len, text, len, *parsed);
#endif

    *closed = 0;

    if (! param_len) {
        param_len = PARAM_MODULO;
        param = malloc(param_len);
    } else if (len >= param_len) {
        param_len += PARAM_MODULO;
        param = realloc(param, param_len);
    }
    if (! param)
        return NULL;
    *param = '\0';

    if (*parsed != 0) {
        inpar = 1;
        *parsed = 0;
    }
    while (ipos < len && inpar >= 0) {
        switch (text[ipos]) {
            case '{':
                if (inpar == 0) {
                    inpar = 1;
                } else {
                    *closed = -1;
                    *parsed = ipos - 2;
                    inpar = -1;
                }
                break;
            case '}':
                if (inpar == 0) {
                    fprintf(stderr, "get_param: accolade fermante inattendue dans %.*s\n",
                                    ipos, text);
                    inpar = -1;
                    *parsed = ipos;
                    *closed = -1;
                } else {
                    *closed = 1;
                    *parsed = ipos;
                    inpar = -1;
//                  while( (opos - 1) > 0 && isspace(param[opos - 1])) {
//                      opos--;
//                  }
                }
                break;
            default:
                if ( inpar == 1 ) {
                    param[opos] = text[ipos];
                    opos++;
                }
            break;
        }
        ipos++;
    }
    if (inpar >= 0) {
        *parsed = len;
    }
    param[opos] = '\0';

    strip_np(param);

#ifdef DEBUG
    fprintf(stderr, "get_param: param=(%s)\n", param);
#endif

    return param;
}


void queue_param(char *param)
{
    int qnewl;

    qnewl = strlen(param);
    if (in_queue + qnewl > QSIZE) {
        fprintf(stderr, "file pleine, param trop long ** (%s) **\n", param);
        assert(in_queue + qnewl <= QSIZE);
    } else {
        strcat(queued, param);
        in_queue += qnewl;
    }
}

int process_tts_cmd(const char *line, int len, int speak)
{
    char *param;
    char *traduc;
    int  retour = 0;
    int  parsed = 0;
    int  closed = 0;
    int  speed;
    char   com_str[40];
    double dval;

    fprintf(stderr, "process_tts_cmd(line = |%.*s|, len = |%d|)\n",
                    len, line, len);

    if (strncmp(line, "tts_say", 7) == 0) {
        fprintf(stderr, "process_tts_cmd : traitement de tts_say\n");
        param = get_param(line + 7, len - 7, &parsed, &closed);
        if (closed && parsed) {
            traduc = translate(param);
            if (traduc == NULL) {
                traduc = param;
            }
            queue_param(traduc);
            queue_param("\n");
            if (speak) {
                write_to_input_fifo(queued, in_queue);
            } else {
                fprintf(stderr, "process_tts_cmd : (%.*s) CANCELED\n", in_queue, queued );
            }
            *queued = '\0';
            in_queue = 0;
            retour = 1;
        }
    } else if (strncmp(line, "tts_set_speech_rate", 19) == 0) {
        fprintf(stderr, "process_tts_cmd : traitement de tts_set_speech_rate\n");
        if (sscanf(line, "%*s %d", &speed) == 1) {
            fprintf(stderr, "process_tts_cmd : passage en vitesse %d\n", speed);
            dval = 120.0 / (double)speed;
            language[current_language].time = dval;
            sprintf(com_str, ";; T=%g\n", dval);
            insert_mbrola_command(com_str);
            fprintf(stderr, "process_tts_cmd : mis en file commande %s\n", com_str);
	    retour = 1;
        }
    }
    return retour;
}

char *tb_trans[] =  {
    "ampersand",       "et commercial",
    "at",              "chez",
    "caret",           "circonflexe",
    "colon",           "deux points",
    "comma",           "virgule",
    "dash",            "tiret",
    "dot",             "point",
    "greater[*]than",  "supérieur",
    "left[*]bracket",  "crochet ouvrant",
    "left[*]paren",    "parenthèse ouvrante",
    "less[*]than",     "inférieur",
    "newline",         "fin de ligne",
    "question[*]mark", "point d'interrogation",
    "percent",         "pour cent",
    "pipe",            "barre verticale",
    "pound",           "dièse",
    "right[*]bracket", "crochet fermant",
    "right[*]paren",   "parenthèse fermante",
    "space",           "espace",
    "semi",            "point virgule",
    "star",            "étoile",
    "underscore",      "souligné",
    "yes",             "oui",
    NULL
};

char *translate(char *mot) {
    char **ptr = tb_trans;
    char *found = NULL;
    char *epure = mot;

    while (*epure && isspace(*epure)) {
        epure++;
    }
    while (*ptr != NULL && found == NULL) {
        if (strcmp(epure, *ptr) == 0) {
            found = *(++ptr);
        } else {
            ptr += 2;
        }
    }
    return found;
}
    

void ana_ligne(const char *line, int len, int speak)
{
    static int  suite;
    int  i = 0;
    char *param;
    int  qnewl;
    int  reject = 0;
    int  parsed;
    int  closed = 0;

    fprintf(stderr, "ana_ligne: ligne (%.*s), longueur=%d, speak=%d\n",
                    len, line, len, speak);
    while (i < len && isspace(line[i]))
        i++;

    if (suite) {
        parsed = in_queue;
    }
//    while (i < len) {
        if (suite) {
            suite = 0;
            param = get_param(line + i, len - i, &parsed, &closed);
            if (param && closed >= 0) {
                i += parsed;
                qnewl = strlen(param);
                if (in_queue + qnewl > QSIZE) {
                    fprintf(stderr,
                            "file pleine, param trop long ** (%s) **\n", param);
                    assert(in_queue + qnewl <= QSIZE);
                } else {
                    strcat(queued, param);
                    in_queue += qnewl;
                }
                if (closed == 0) {
                    parsed = i;
                }
            } else {
                reject = 1;
            }
        } else {
            switch (line[i]) {
                case 'q':
                    parsed = 0;
                    param = get_param(&line[i + 1], len - (i + 1), &parsed, &closed);
                    assert(param != NULL);
                    switch (closed) {
                        case 1:
                        case 0:
                            suite = (!closed);
                            queue_param(param);
                            break;
                        default:
                            break;
                    }
                    break;
        
                case 'd':
                    queued[in_queue] = '\n';
                    if (speak) {
                        write_to_input_fifo(queued, ++in_queue);
                    } else {
                        fprintf(stderr, "ana_ligne : (%.*s) NOT SPOKEN\n", in_queue, queued);
                    }
                    in_queue = 0;
                    *queued = '\0';
                    break;
        
                case 'l':
                    if (len == 4 && line[3] == '\n') {
                        queued[0] = line[2];
                        queued[1] = line[3];
                        if (speak) {
                            write_to_input_fifo(queued, 2);
                        } else {
                            fprintf(stderr, "ana_ligne : (%.2s) NOT SAID\n", queued);
                        }
                        in_queue = 0;
                        *queued = '\0';
                    } else {
                        parsed = 0;
                        closed = 0;
                        param = get_param(&line[i + 1], len - (i + 1),
                                          &parsed, &closed);
                        qnewl = strlen(param);
                        if (qnewl != 1) {
                            reject = 1;
                        } else {
                            queued[0] = *param;
                            queued[1] = '\n';
                            if (speak) {
                                write_to_input_fifo(queued, 2);
                            } else {
                                fprintf(stderr, "ana_ligne : (%.2s) NOT SAID\n", queued);
                            }
                            in_queue = 0;
                            *queued = '\0';
                        }
                    }
                    break;
        
                case 't':
                    reject = !process_tts_cmd(&line[i], len - i, speak);
                    break;

                default:
                    reject = 1;
                    break;
            }
        }
//    }
    if (reject == 1) {
        fprintf(stderr, "ana_ligne: ligne ignorée ** (%.*s) **\n", len, line);
    }
}


/*
 * ----------------------------------------------------------------------
 * Read a "line" from the input file descriptor. 
 * Lines may not be longer than MAX_LINE, speech does not begin
 * until we have seen a complete line, delimited by 0x0d.
 * Whenever we se a 0x18 in the input, we clear the whole speech
 * pipeline.
 * ----------------------------------------------------------------------
 */
#define MAX_LINE 1000
static char line_buffer[MAX_LINE];
static int  in_line = 0;

void read_from_input(int back_from_index)
{
    int clear_pipe;
    int r, i;
    int parsed;

    /*
     * This function is only called after a select(), so there should be
     * at least one character. When there are no characters, we terminate
     * by calling the signal handler with argument 0.
     * The exception is a "back_from_index call". In this case, do no 
     * read but consume the buffer content only.
     */
    if (!back_from_index) {
       r = rep_read(from_input, &line_buffer[in_line], MAX_LINE - in_line);
       if (r <= 0) {
           from_input_enable = 0;
           fprintf(stderr, "read_from_input(0): lu = %d\n", r);
           //PB20030720+PB20031122: signal_handler(0);
           return;
       }
       in_line += r;
    }
    gettimeofday(&last_input_time, NULL);
    /*
     * Search for a clear pipeline character (0x18). Whenever we find
     * one, throw away everything in the line buffer before it. 
     * After the search has finished, clear whole pipe. 
     */
    clear_pipe = 0;
    i = 0;
    while (i < in_line) {
       if (line_buffer[i] == 0x18) {
           line_buffer[i] = '\0';
fprintf(stderr, "Arrêt demandé -> ** (%s) DISCARDED **\n", line_buffer);
           memmove(&line_buffer[0], &line_buffer[i + 1], in_line - (i + 1));
           in_line -=  i + 1;
           i = 0;
           clear_pipe = 1;
       } else {
          i++;
       }
    }
    if (clear_pipe)
       clear_speech_pipe();
    /*
     * Now we have the buffer, containing one or more lines but no clear
     * pipe symbols. Search for end of line marks and handle the lines
     * by calling parse_line(). Not all of the line must be eaten, parsing
     * stops at index marks.
     */
    i = 0;
    while (i < in_line && index_mark == 0) {
       if (line_buffer[i] == 0x0d || line_buffer[i] == 0x0a) {
          parse_line(line_buffer, i + 1, &parsed);
          memmove(&line_buffer[0], &line_buffer[parsed], in_line - parsed);
          in_line -= parsed;
          i = 0;
       } else {
          i++;
       }
    }
}

void lire_entree(int back_from_index)
{
    int clear_pipe;
    int r, i;
    int curstop = -1;
    int cmd_start = 0; // position de début de la commande en cours d'acquisition
    int speak_start = 0; // position à partir de laquelle des paroles sont à dire

    /*
     * This function is only called after a select(), so there should be
     * at least one character. When there are no characters, we terminate
     * by calling the signal handler with argument 0.
     * The exception is a "back_from_index call". In this case, do no 
     * read but consume the buffer content only.
     */
    if (!back_from_index) {
       r = rep_read(from_input, &line_buffer[in_line], MAX_LINE - in_line);
       if (r <= 0) {
           from_input_enable = 0;
           fprintf(stderr, "lire_entree(0): lu = %d\n", r);
           //PB20030720+PB20031122: signal_handler(0);
           return;
       } else {
           fprintf(stderr, "lire_entree: lu = #%.*s#\n", 
   r, &line_buffer[in_line]);
           in_line += r;
       }
    }
    gettimeofday(&last_input_time, NULL);
    //PB20031206 recherche de commande d'arrêt 's'
    clear_pipe = 0;
    i = 0;
    while (i < in_line) {
       switch (line_buffer[i]) {
           case 's':
               if (curstop == -1)
                   curstop = i;
               else
                   curstop = -2;
               break;
           case ' ' :
           case '\t':
           case '\r':
               break;
           case '\n':
               if (curstop >= 0) {
                   //line_buffer[i] = '\0';
                   fprintf(stderr,
                           "Arrêt demandé -> ** (%.*s) MAY BE DISCARDED **\n",
                           (i + 1), line_buffer);
                   memmove(&line_buffer[cmd_start], &line_buffer[i + 1],
                           in_line - (i + 1));
                   in_line -=  (i + 1 - cmd_start);
                   speak_start = cmd_start;
                   clear_pipe = 1;
               } else {
                   cmd_start = i + 1;
               }
               curstop = -1;
               i = cmd_start - 1;
               break;
           default:
               curstop = -2;
               break;
       }
       i++;
    }
    if (clear_pipe)
       clear_speech_pipe();
    /*
     * Now we have the buffer, containing one or more lines but no clear
     * pipe symbols. Search for end of line marks and handle the lines
     * by calling parse_line(). Not all of the line must be eaten, parsing
     * stops at index marks.
     */
    fprintf(stderr, "lire_entree: à dire après éventuel clear = #%.*s#\n",
                    in_line, line_buffer);
    i = 0;
    cmd_start = 0;
    while (i < in_line && index_mark == 0) {
       if (line_buffer[i] == '\n') {
          ana_ligne(&line_buffer[cmd_start], (i + 1 - cmd_start), (i > speak_start));
          //in_line -= (i + 1);
          //memmove(&line_buffer[0], &line_buffer[i + 1], in_line);
          //i = 0;
          cmd_start = i + 1;
       }
       i++;
    }
    in_line = 0;
}


/*
 * ----------------------------------------------------------------------
 * Compute difference (in seconds) between given time t and current time.
 * ----------------------------------------------------------------------
 */
double time_diff(struct timeval *t)
{
    struct timeval ct;
    struct timezone zone;

    gettimeofday(&ct, &zone);
    
    return (ct.tv_sec - t->tv_sec + (ct.tv_usec - t->tv_usec) * 1e-6);
}


/*
 * ----------------------------------------------------------------------
 * Write parsed input text to phonem generator.
 * Version 0.8.1 of hadifix has a bug, it terminates on dots in the input
 * stream. The helper functions replaces all "." by ",".
 * ----------------------------------------------------------------------
 */
#if HADIFIX_DOT_BUG
static void replace_dots(char *data, int len)
{
    int i;

    if (current_language == LANG_GERMAN) {
       fprintf(stderr, "replace_dots: current_language = %d\n", current_language);
       for (i = 0; i < len; i++)
          if (data[i] == '.')
             data[i] = ',';
    }
}
#endif

void write_to_t2pho(void)
{
    int count, fifo_count;

    /*
     * out >= in signals that the full part of the fifo wraps around,
     * write part from "out" to "right end" first.
     */
    if (input_fifo.out >= input_fifo.in) {
        fifo_count = INPUT_FIFO_SIZE - input_fifo.out;
        if (fifo_count > input_fifo.count)
            fifo_count = input_fifo.count;
#if HADIFIX_DOT_BUG
        replace_dots(&input_fifo.data[input_fifo.out], fifo_count);
#endif
        count = rep_write(to_t2pho, &input_fifo.data[input_fifo.out],
                          fifo_count);
        assert(count >= 0);
        input_fifo.count -= count;
        input_fifo.out   += count;
        if (input_fifo.out == INPUT_FIFO_SIZE)
            input_fifo.out = 0;
    }
    /*
     * When the part from "out" to "right end" has been played completely,
     * write from "left border"
     */
    if (input_fifo.out < input_fifo.in) {
        fifo_count = input_fifo.count;
#if HADIFIX_DOT_BUG
        replace_dots(&input_fifo.data[input_fifo.out], fifo_count);
#endif
        count = rep_write(to_t2pho, &input_fifo.data[input_fifo.out],
                          fifo_count);
        if (count < 0)
            perror("write to t2pho");
        input_fifo.count -= count;
        input_fifo.out   += count;
        if (input_fifo.out == INPUT_FIFO_SIZE)
            input_fifo.out = 0;
    }
    if (input_fifo.count < INPUT_FIFO_SIZE / 2)
        from_input_enable = 1;
    if (input_fifo.count == 0)
        to_t2pho_enable = 0;
}


/*
 * ----------------------------------------------------------------------
 * Fill the buffer from the phoneme pipe. This has to be done
 * none blocking.
 * ----------------------------------------------------------------------
 */
void refill_phoneme_buf(void)
{
    int flags, got;

    flags = fcntl(from_t2pho, F_GETFL);
    assert(flags >= 0);
    assert(fcntl(from_t2pho, F_SETFL, flags | O_NONBLOCK) >= 0);
    got = rep_read(from_t2pho, &phoneme_buf[phoneme_buf_count],
                   PHONEME_BUF_SIZE - phoneme_buf_count);
    assert(fcntl(from_t2pho, F_SETFL, flags) >= 0);
    if (got > 0)
        phoneme_buf_count += got;
}


/*
 * ----------------------------------------------------------------------
 * Read from the phoneme generator and place lines in the fifo to mbrola.
 * Be aware not to destroy the line structure.
 * ----------------------------------------------------------------------
 */
void read_from_t2pho(void)
{
    int i;
    int lines = 0;

    while (phoneme_fifo.count < PHONEME_FIFO_SIZE) {
        refill_phoneme_buf();
        for (i = 0; i < phoneme_buf_count; i++) {
            assert(i < PHONEME_LINE_LEN);
            if (phoneme_buf[i] == '\n')
                break;
        }
        if (i < PHONEME_BUF_SIZE && phoneme_buf[i] == '\n') {
            i++;      /* move i behind '\n' */
            if (kill_line)
                kill_line = 0;
            else {
                memcpy(phoneme_fifo.data[phoneme_fifo.in],
                       phoneme_buf, i);
                phoneme_fifo.len[phoneme_fifo.in] = i;
                if (++phoneme_fifo.in == PHONEME_FIFO_SIZE)
                    phoneme_fifo.in = 0;
                phoneme_fifo.count++;
            }
            memmove(phoneme_buf, phoneme_buf + i, phoneme_buf_count - i);
            phoneme_buf_count -= i;
            lines++;
        } else {
            break;      /* no complete line available */
        }
    }
    if (lines > 0) {
        gettimeofday(&last_t2pho_time, NULL);
        mbrola_flushed = 0;
    }

    if (phoneme_fifo.count == PHONEME_FIFO_SIZE) {
        from_t2pho_enable = 0;
    }
    if (phoneme_fifo.count > 0 || phoneme_fifo.cmd_count > 0)
        to_mbrola_enable = 1;

}


/*
 * ----------------------------------------------------------------------
 * Insert a command in the command fifo (the mixed fifo from phoneme 
 * generator to mbrola). Return 1 on success, 0 on failure (command fifo
 * full). The command must be terminated by "\n\0".
 * ----------------------------------------------------------------------
 */
int insert_mbrola_command(char *command)
{
    int len = strlen(command);

    assert(len <= PHONEME_LINE_LEN); //PB20031109: was PHONEME_BUF_SIZE

    if (phoneme_fifo.cmd_count < PHONEME_FIFO_CMD_SIZE) {
        memcpy(phoneme_fifo.cmd_data[phoneme_fifo.cmd_in], command, len);
        phoneme_fifo.cmd_len[phoneme_fifo.cmd_in] = len;
        if (++phoneme_fifo.cmd_in == PHONEME_FIFO_CMD_SIZE)
            phoneme_fifo.cmd_in = 0;
        phoneme_fifo.cmd_count++;

        gettimeofday(&last_t2pho_time, NULL);
        mbrola_flushed = 0;
        to_mbrola_enable = 1;

        return 1;
    } else {
        fprintf(stderr, "command buffer full\n");
        return 0;
    }
}


/*
 * ----------------------------------------------------------------------
 * Write data from the phoneme fifo to the mbrola synthesizer.
 * The phoneme fifo is separated in two parts, one for commands,
 * one for usual phonemes. Commands have higher priority,
 * when all commands are written, phonemes follow.
 * ----------------------------------------------------------------------
 */
void write_to_mbrola(void)
{
    int  count;

    if (fragment_len > 0) {
       fprintf(stderr, "wtm, fragment_len = %d\n", fragment_len);
       count = rep_write(to_mbrola, line_fragment, fragment_len);
#if DWRITE
       write(2, line_fragment, count);
#endif
       if (count == fragment_len)
           fragment_len = 0;
       else {
           memmove(line_fragment, line_fragment + count,
                   fragment_len - count);
           fragment_len -= count;
           fprintf(stderr, "fragment still not written\n");
       }
    }
    /*
     * When we have get rid of the fragment, we can start to write
     * commands and data until the fifos are empty or a write
     * blocks.
     */
    while (fragment_len == 0 && phoneme_fifo.cmd_count > 0) {
       count = rep_write(to_mbrola,
                         phoneme_fifo.cmd_data[phoneme_fifo.cmd_out],
                         phoneme_fifo.cmd_len[phoneme_fifo.cmd_out]);
#if DWRITE
       write(2, phoneme_fifo.cmd_data[phoneme_fifo.cmd_out], count);
#endif
       if (count != phoneme_fifo.cmd_len[phoneme_fifo.cmd_out]) {
           fragment_len = phoneme_fifo.cmd_len[phoneme_fifo.cmd_out] - count;
           memcpy(line_fragment,
                  &phoneme_fifo.cmd_data[phoneme_fifo.cmd_out][count],
                  fragment_len);
       }
       if (++phoneme_fifo.cmd_out == PHONEME_FIFO_CMD_SIZE)
           phoneme_fifo.cmd_out = 0;
       phoneme_fifo.cmd_count--;
    }

    while (fragment_len == 0 && phoneme_fifo.count > 0) {
       count = rep_write(to_mbrola, phoneme_fifo.data[phoneme_fifo.out],
                           phoneme_fifo.len[phoneme_fifo.out]);
#if DWRITE
       write(2, phoneme_fifo.data[phoneme_fifo.out], count);
#endif
       if (count != phoneme_fifo.len[phoneme_fifo.out]) {
           fragment_len = phoneme_fifo.len[phoneme_fifo.out] - count;
           memcpy(line_fragment,
                     &phoneme_fifo.data[phoneme_fifo.out][count],
                     fragment_len);
       }
       if (++phoneme_fifo.out == PHONEME_FIFO_SIZE)
           phoneme_fifo.out = 0;
       phoneme_fifo.count--;
    }

    if (phoneme_fifo.count < PHONEME_FIFO_SIZE)
       from_t2pho_enable = 1;
    if (phoneme_fifo.count == 0 && phoneme_fifo.cmd_count == 0)
       to_mbrola_enable = 0;
    return;
}


/*
 * ----------------------------------------------------------------------
 * Read data from mbrola and put it into the aufio fifo.
 * ----------------------------------------------------------------------
 */
void read_from_mbrola(void)
{
    int max_s;         /* read max_s bytes */
    int r;         /* number of bytes read */
    int flags;
    int got = 0;

    flags = fcntl(from_mbrola, F_GETFL);
    assert(flags >= 0);
    assert(fcntl(from_mbrola, F_SETFL, flags | O_NONBLOCK) >= 0);

    if (audio_fifo.in >= audio_fifo.out) {
   max_s = AUDIO_FIFO_SIZE - audio_fifo.in;
   if (max_s > AUDIO_FIFO_SIZE - audio_fifo.count)
       max_s = AUDIO_FIFO_SIZE - audio_fifo.count;
   r = rep_read(from_mbrola, &audio_fifo.data[audio_fifo.in], max_s);
   if (r > 0) {
       audio_fifo.count += r;
       audio_fifo.in    += r;
       if (audio_fifo.in == AUDIO_FIFO_SIZE)
      audio_fifo.in = 0;
       got += r;
   }
    }
    if (audio_fifo.in < audio_fifo.out) {
   max_s = AUDIO_FIFO_SIZE - audio_fifo.count;
   r = rep_read(from_mbrola, &audio_fifo.data[audio_fifo.in], max_s);
   if (r > 0) {
       audio_fifo.count += r;
       audio_fifo.in    += r;
       if (audio_fifo.in == AUDIO_FIFO_SIZE)
      audio_fifo.in = 0;
       got += r;
   }
    }

    assert(fcntl(from_mbrola, F_SETFL, flags) >= 0);
}


/*
 * ----------------------------------------------------------------------
 * Write data from audio fifo to audio device, do not block.
 * ----------------------------------------------------------------------
 */
void write_to_audio(void)
{
    int max_write, count, count2, fifo_count;
    int written = 0;

    max_write = audio_write_size(to_audio);
    /*
     * out >= in signals that the full part of the fifo wraps around,
     * play part from "out" to "right end" first.
     */
    if (audio_fifo.out >= audio_fifo.in) {
        fifo_count = AUDIO_FIFO_SIZE - audio_fifo.out;
        if (fifo_count > audio_fifo.count)
            fifo_count = audio_fifo.count;
        if (max_write < fifo_count)
            count = max_write;
        else
            count = fifo_count;
        if (count > 0) {
            count2 = rep_write(to_audio, &audio_fifo.data[audio_fifo.out],
                               count);
            if (count2 < 0) {
                perror("write to audio");
                assert(0);
            }
            max_write        -= count2;
            audio_fifo.count -= count2;
            audio_fifo.out   += count2;
            if (audio_fifo.out == AUDIO_FIFO_SIZE)
                audio_fifo.out = 0;
            written += count2;
        }
    }
    /*
     * When the part from "out" to "right end" has been played completely,
     * play from "left border"
     */
    if (audio_fifo.out < audio_fifo.in) {
        if (max_write < audio_fifo.count)
            count = max_write;
        else
            count = audio_fifo.count;
        if (count > 0) {
            count2 = rep_write(to_audio, &audio_fifo.data[audio_fifo.out],
                               count);
            if (count2 < 0) {
                perror("write to audio");
                assert(0);
            }
            max_write        -= count2;
            audio_fifo.count -= count2;
            audio_fifo.out   += count2;
            if (audio_fifo.out == AUDIO_FIFO_SIZE)
                audio_fifo.out = 0;
            written += count2;
        }
    }

    if (written > 0) {
        gettimeofday(&last_audio_time, NULL);
        audio_flushed = 0;
    }
}


/*
 * ----------------------------------------------------------------------
 * Open the audio device for writing with the following parameters:
 *  - sampling frequency: 16000 Hz
 *  - sample size: 16 bit, signed integer, little endian
 *  - one channel (mono)
 * The resulting datarate is 16000 * sizeof(short) / s = 32000 byte/s
 * It would be nice to use non-blocking writes to the device or - even
 * better - select(), but it seems the audio driver does not support
 * this.
 * ----------------------------------------------------------------------
 */
int open_audio(void)
{
    int speed = 16000;
    int format = AFMT_S16_LE;
    int channels = 1;
    int fd;

    fd = open("/dev/dsp", O_WRONLY);
    assert(fd >= 0);
    assert(ioctl(fd, SNDCTL_DSP_SETFMT, &format) >= 0);
    assert(ioctl(fd, SNDCTL_DSP_CHANNELS, &channels) >= 0);
    assert(ioctl(fd, SNDCTL_DSP_SPEED, &speed) >= 0);

    return fd;
}


/*
 * ----------------------------------------------------------------------
 * Get the number of bytes we can write to the audio fd without blocking.
 * ----------------------------------------------------------------------
 */
int audio_write_size(int fd)
{
    audio_buf_info info;

    assert(ioctl(fd, SNDCTL_DSP_GETOSPACE, &info) >= 0);
    return info.fragments * info.fragsize;
}


/*
 * ----------------------------------------------------------------------
 * Play "duration" silence. Assumes 2 bytes samples, 16k sampling rate.
 * ----------------------------------------------------------------------
 */
int silence(int fd, double duration)
{
    static char buf[1024];
    int bytes;

    bytes = (int)(duration * 16000 * 2) & ~1;
    
    while (bytes > 0) {
        if (bytes > 1024) {
            rep_write(fd, buf, 1024);
            bytes -= 1024;
        } else {
            rep_write(fd, buf, bytes);
            bytes = 0;
        }
    }

    return 0;
}


/*
 * ----------------------------------------------------------------------
 * (Re-)start the text-to-phoneme translator.
 * Reading from the converter is enabled (space in buffer available),
 * writing to the converter is disabled (fifo to converter is empty).
 * restart == 1 means to kill an old version before start the new one.
 * ----------------------------------------------------------------------
 */
int restart_t2pho(lang_t *lang, int restart)
{
    int flags;

    input_fifo.in    = 0;
    input_fifo.out   = 0;
    input_fifo.count = 0;

    if (restart) {
        kill(t2pho_pid, SIGTERM);
        close(to_t2pho);
        close(from_t2pho);
    }
    start_program(lang->t2pho_argv, &to_t2pho, &from_t2pho, &t2pho_pid);
    flags = fcntl(to_t2pho, F_GETFL);
    assert(flags >= 0);
    assert(fcntl(to_t2pho, F_SETFL, flags | O_NONBLOCK) >= 0);
    from_t2pho_enable = 1;
    to_t2pho_enable   = 0;
    gettimeofday(&last_t2pho_time, NULL);
    mbrola_flushed = 1;
    phoneme_buf_count = 0;

    return 0;
}


/*
 * ----------------------------------------------------------------------
 * (Re-)start the mbrola synthesizer.
 * restart == 1 means to kill an old version before start the new one.
 * ----------------------------------------------------------------------
 */
int restart_mbrola(lang_t *lang, int restart)
{
    int flags;
    char com_str[50];

    fragment_len = 0;

    if (restart) {
        kill(mbrola_pid, SIGTERM);
        close(to_mbrola);
        close(from_mbrola);
    }
    start_program(lang->mbrola_argv, &to_mbrola, &from_mbrola, &mbrola_pid);
    flags = fcntl(to_mbrola, F_GETFL);
    assert(flags >= 0);
    assert(fcntl(to_mbrola, F_SETFL, flags | O_NONBLOCK) >= 0);
    from_mbrola_enable = 1;
    to_mbrola_enable   = 0;

    fprintf(stderr, "restart_mbrola: current_language = %d\n", current_language);
    sprintf(com_str, ";; T=%g\n", language[current_language].time);
    insert_mbrola_command(com_str);
    sprintf(com_str, ";; F=%g\n", language[current_language].pitch);
    insert_mbrola_command(com_str);
    sprintf(com_str, ";; FLUSH %s\n", language[current_language].flush);
    insert_mbrola_command(com_str);
    
    return 0;
}


/*
 * ----------------------------------------------------------------------
 * Start a program (int the background) and connect it with two pipes
 * to the caller:
 *  *to from caller to program
 *  *from from programm to caller
 * Process id of the program is returned in *pid.
 * ----------------------------------------------------------------------
 */
int start_program(char *argv[], int *to, int *from, int *pid)
{
    int to_pipe[2], from_pipe[2];

    assert(pipe(to_pipe) >= 0);
    assert(pipe(from_pipe) >= 0);
    
    *pid = fork();
    assert(*pid >= 0);
    if (*pid == 0) {      /* child */
   close(from_pipe[0]);
   close(to_pipe[1]);
   assert(dup2(to_pipe[0], 0) >= 0);
   assert(dup2(from_pipe[1], 1) >= 0);
   close(from_pipe[1]);
   close(to_pipe[0]);
   assert(chdir(SPEECH_DIR) >= 0);
   fprintf(stderr, "%s\n", argv[0]);
   execvp(argv[0], argv);
   perror("exec failed");
   return -1;
    } else {         /* parent */
   *to   = to_pipe[1];
   *from = from_pipe[0];
   close(to_pipe[0]);
   close(from_pipe[1]);
    }

    return 0;
}


/*
 * ----------------------------------------------------------------------
 * Wrapper arount write(), restart system call after signal.
 * ----------------------------------------------------------------------
 */
ssize_t rep_read(int fd, void *buf, size_t count)
{
    ssize_t res;

    do {
   res = read(fd, buf, count);
    } while (res == -1 && errno == EINTR);

    return res;
}


/*
 * ----------------------------------------------------------------------
 * Wrapper arount read(), restart system call after signal.
 * ----------------------------------------------------------------------
 */
ssize_t rep_write(int fd, const void *buf, size_t count)
{
    ssize_t res;

    do {
        res = write(fd, buf, count);
    } while (res == -1 && errno == EINTR);

    return res;
}


/*
 * ----------------------------------------------------------------------
 * General signal handler. Called with signal == 0 instead of exit.
 * Clean up and terminate.
 * ----------------------------------------------------------------------
 */
void signal_handler(int signal)
{
    char fname[200];

    fprintf(stderr, "signal %d, terminating\n", signal);
    if (t2pho_pid != 0) {
   kill(t2pho_pid, SIGTERM);
   sprintf(fname, "/tmp/%d.hadi.log", t2pho_pid);
   unlink(fname);
   sprintf(fname, "/tmp/%dhadivv.tmp", t2pho_pid);
   unlink(fname);
    }
    if (mbrola_pid != 0)
   kill(mbrola_pid, SIGTERM);
    close(to_audio);
    fclose(stderr);

    exit(0);
}

/*
 * ----------------------------------------------------------------------
 * Wait for the termination of the child.
 * ----------------------------------------------------------------------
 */
void sigchild_handler(int sig)
{
    int   status;
    pid_t pid;

    pid = waitpid(-1, &status, WNOHANG);
    while (pid > 0) {
   if (WIFEXITED(status))
       fprintf(stderr, "process %d returned %d\n",
          pid, WEXITSTATUS(status));
   else if (WIFSIGNALED(status))
#ifndef USE_CHECKER
       fprintf(stderr, "process %d died by signal %d (%s)\n",
          pid, WTERMSIG(status), strsignal(WTERMSIG(status)));
#else
       fprintf(stderr, "process %d died by signal %d\n",
          pid, WTERMSIG(status));
#endif
   else
       fprintf(stderr, "process %d died by unknown reason\n", pid);

   pid = waitpid(-1, &status, WNOHANG);
    }
}


/*
 * ----------------------------------------------------------------------
 * Called when there is an exception on one of the file descriptors.
 * Dump state to stderr and exit.
 * ----------------------------------------------------------------------
 */
void dump_state(void)
{
    fprintf(stderr, "kill_line = %d\n", kill_line);
    fprintf(stderr, "phoneme_buf_count = %d\n", phoneme_buf_count);
    fprintf(stderr, "fragment_len = %d\n", fragment_len);
    fprintf(stderr, "phoneme_fifo.in    = %d\n", phoneme_fifo.in);
    fprintf(stderr, "phoneme_fifo.out   = %d\n", phoneme_fifo.out);
    fprintf(stderr, "phoneme_fifo.count = %d\n", phoneme_fifo.count);
    fprintf(stderr, "phoneme_fifo.cmd_in    = %d\n", phoneme_fifo.cmd_in);
    fprintf(stderr, "phoneme_fifo.cmd_out   = %d\n", phoneme_fifo.cmd_out);
    fprintf(stderr, "phoneme_fifo.cmd_count = %d\n", phoneme_fifo.cmd_count);
    fprintf(stderr, "from_input_enable  = %d\n", from_input_enable);
    fprintf(stderr, "to_t2pho_enable    = %d\n", to_t2pho_enable);
    fprintf(stderr, "from_t2pho_enable  = %d\n", from_t2pho_enable);
    fprintf(stderr, "to_mbrola_enable   = %d\n", to_mbrola_enable);
    fprintf(stderr, "from_mbrola_enable = %d\n", from_mbrola_enable);
}
