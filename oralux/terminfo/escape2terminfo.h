
#define MAXPARAM 20
extern int myParameters[MAXPARAM];
extern int myNumberOfParameters;
/* < terminalMode */
enum terminalMode
  {
    STANDOUT, UNDERLINE,  REVERSE,
    BLINK2,  DIM2,  BOLD2,  BLANK,  
    PROTECT, ALTERNATE_CHARSET,
  };
/* > */
/* < enum StringCapacity */

/* From Man page terminfo(5) 
If the following array is updated, then update the myStringCapacity array
The first enum equals 1 since the enum is returned by yylex and 0 means end of file.
*/
enum StringCapacity
  {
    ACSC=1,     /* graphics charset pairs, based on vt100 */
    CBT,        /* back tab (P) */
    BEL,        /* audible signal */
    CR,         /* carriage return (P*) */
    CPI,        /* Change number of characters per inch  to #1 */
    LPI,        /* Change number of lines per inch to #1 */
    CHR,        /* Change horizontal resolution to #1 */
    CVR,        /* Change vertical resolution to #1 */
    CSR,        /* change region to line #1 to line #2 */
    RMP,        /* like ip but when in insert mode */
    TBC,        /* clear all tab stops */
    MGC,        /* clear right and left soft margins */
    CLEAR,      /* clear screen and home cursor */
    EL1,        /* Clear to beginning of line */
    EL,         /* clear to end of line */
    ED,         /* clear to end of screen */
    HPA,        /* horizontal position #1, absolute (P) */
    CMDCH,      /* terminal settable cmd character in prototype !? */
    CWIN,       /* define a window #1 from #2,#3 to #4,#5 */
    CUP,        /* move to row #1 columns #2 */
    CUD1,       /* down one line */
    HOME,       /* home cursor (if no cup) */
    CIVIS,      /* make cursor invisible */
    CUB1,       /* move left one space */
    MRCUP,      /* memory relative cursor addressing, move to row #1 columns #2 */
    CNORM,      /* make cursor appear normal (undo civis/cvvis) */
    CUF1,       /* non-destructive space (move right one space) */
    LL,         /* last line, first column (if no cup) */
    CUU1,       /* up one line */
    CVVIS,      /* make cursor very visible */
    DEFC,       /* Define a character #1, #2 dots wide,descender #3 */
    DCH1,       /* delete character */
    DL1,        /* delete line (P*) */
    DIAL,       /* dial number #1 */
    DSL,        /* disable status line */
    DCLK,       /* display clock */
    HD,         /* half a line down */
    ENACS,      /* enable alternate char set */
    SMACS,      /* start alternate character set (P) */
    SMAM,       /* turn on automatic margins */
    BLINK,      /* turn on blinking */
    BOLD,       /* turn on bold (extra bright) mode */
    SMCUP,      /* string to start programs using cup */
    SMDC,       /* enter delete mode */
    DIM,        /* turn on half-bright mode */
    SWIDM,      /* Enter double-wide mode */
    SDRFQ,      /* Enter draft-quality mode */
    SMIR,       /* enter insert mode */
    SITM,       /* Enter italic mode */
    SLM,        /* Start leftward carriage motion */
    SMICM,      /* Start micro-motion mode */
    SNLQ,       /* Enter NLQ mode */
    SNRMQ,      /* Enter normal-quality mode */
    PROT,       /* turn on protected mode */
    REV,        /* turn on reverse video mode */
    INVIS,      /* turn on blank mode (characters invisible) */
    SSHM,       /* Enter shadow-print mode */
    SMSO,       /* begin standout mode */
    SSUBM,      /* Enter subscript mode */
    SSUPM,      /* Enter superscriptmode */
    SMUL,       /* begin underline mode */
    SUM,        /* Start upward carriage motion */
    SMXON,      /* turn on xon/xoff handshaking */
    ECH,        /* erase #1 characters (P) */
    RMACS,      /* end alternate character set (P) */
    RMAM,       /* turn off automatic margins */
    SGR0,       /* turn off all attributes */
    RMCUP,      /* strings to end programs using cup */
    RMDC,       /* end delete mode */
    RWIDM,      /* End double-wide mode */
    RMIR,       /* exit insert mode */
    RITM,       /* End italic mode */
    RLM,        /* End left-motion mode */
    RMICM,      /* End micro-motion mode */
    RSHM,       /* End shadow-print mode */
    RMSO,       /* exit standout mode */
    RSUBM,      /* End subscript mode */
    RSUPM,      /* End superscript mode */
    RMUL,       /* exit underline mode */
    RUM,        /* End reverse character motion */
    RMXON,      /* turn off xon/xoff handshaking */
    PAUSE,      /* pause for 2-3 seconds */
    HOOK,       /* flash switch hook */
    FLASH,      /* visible bell (may not move cursor) */
    FF,         /* hardcopy terminalpage eject (P*) */
    FSL,        /* return from status line */
    WINGO,      /* go to window #1 */
    HUP,        /* hang-up phone */
    IS1,        /* initialization string */
    IS2,        /* initialization string */
    IS3,        /* initialization string */
    IF,         /* name of initialization file */
    IPROG,      /* path name of program for initialization */
    INITC,      /* initialize color #1 to (#2,#3,#4) */
    INITP,      /* Initialize color pair #1 to fg=(#2,#3,#4), bg=(#5,#6,#7) */
    ICH1,       /* insert character (P) */
    IL1,        /* insert line (P*) */
    IP,         /* insert padding after inserted character */
    KA1,        /* upper left of keypad */
    KA3,        /* upper right of keypad */
    KB2,        /* center of keypad */
    KBS,        /* backspace key */
    KBEG,       /* begin key */
    KCBT,       /* back-tab key */
    KC1,        /* lower left of keypad */
    KC3,        /* lower right of keypad */
    KCAN,       /* cancel key */
    KTBC,       /* clear-all-tabs key */
    KCLR,       /* clear-screen or erase key */
    KCLO,       /* close key */
    KCMD,       /* command key */
    KCPY,       /* copy key */
    KCRT,       /* create key */
    KCTAB,      /* clear-tab key */
    KDCH1,      /* delete-character key */
    KDL1,       /* delete-line key */
    KCUD1,      /* down-arrow key */
    KRMIR,      /* sent by rmir or smir in insert mode */
    KEND,       /* end key */
    KENT,       /* enter/send key */
    KEL,        /* clear-to-end-of-line key */
    KED,        /* clear-to-end-of-screen key */
    KEXT,       /* exit key */
    KF0,        /* F0 function key */
    KF1,        /* F1 function key */
    KF10,       /* F10 function key */
    KF11,       /* F11 function key */
    KF12,       /* F12 function key */
    KF13,       /* F13 function key */
    KF14,       /* F14 function key */
    KF15,       /* F15 function key */
    KF16,       /* F16 function key */
    KF17,       /* F17 function key */
    KF18,       /* F18 function key */
    KF19,       /* F19 function key */
    KF2,        /* F2 function key */
    KF20,       /* F20 function key */
    KF21,       /* F21 function key */
    KF22,       /* F22 function key */
    KF23,       /* F23 function key */
    KF24,       /* F24 function key */
    KF25,       /* F25 function key */
    KF26,       /* F26 function key */
    KF27,       /* F27 function key */
    KF28,       /* F28 function key */
    KF29,       /* F29 function key */
    KF3,        /* F3 function key */
    KF30,       /* F30 function key */
    KF31,       /* F31 function key */
    KF32,       /* F32 function key */
    KF33,       /* F33 function key */
    KF34,       /* F34 function key */
    KF35,       /* F35 function key */
    KF36,       /* F36 function key */
    KF37,       /* F37 function key */
    KF38,       /* F38 function key */
    KF39,       /* F39 function key */
    KF4,        /* F4 function key */
    KF40,       /* F40 function key */
    KF41,       /* F41 function key */
    KF42,       /* F42 function key */
    KF43,       /* F43 function key */
    KF44,       /* F44 function key */
    KF45,       /* F45 function key */
    KF46,       /* F46 function key */
    KF47,       /* F47 function key */
    KF48,       /* F48 function key */
    KF49,       /* F49 function key */
    KF5,        /* F5 function key */
    KF50,       /* F50 function key */
    KF51,       /* F51 function key */
    KF52,       /* F52 function key */
    KF53,       /* F53 function key */
    KF54,       /* F54 function key */
    KF55,       /* F55 function key */
    KF56,       /* F56 function key */
    KF57,       /* F57 function key */
    KF58,       /* F58 function key */
    KF59,       /* F59 function key */
    KF6,        /* F6 function key */
    KF60,       /* F60 function key */
    KF61,       /* F61 function key */
    KF62,       /* F62 function key */
    KF63,       /* F63 function key */
    KF7,        /* F7 function key */
    KF8,        /* F8 function key */
    KF9,        /* F9 function key */
    KFND,       /* find key */
    KHLP,       /* help key */
    KHOME,      /* home key */
    KICH1,      /* insert-character key */
    KIL1,       /* insert-line key */
    KCUB1,      /* left-arrow key */
    KLL,        /* lower-left key (home down) */
    KMRK,       /* mark key */
    KMSG,       /* message key */
    KMOV,       /* move key */
    KNXT,       /* next key */
    KNP,        /* next-page key */
    KOPN,       /* open key */
    KOPT,       /* options key */
    KPP,        /* previous-page key */
    KPRV,       /* previous key */
    KPRT,       /* print key */
    KRDO,       /* redo key */
    KREF,       /* reference key */
    KRFR,       /* refresh key */
    KRPL,       /* replace key */
    KRST,       /* restart key */
    KRES,       /* resume key */
    KCUF1,      /* right-arrow key */
    KSAV,       /* save key */
    SHIFTKBEG, /* shifted begin key */
    SHIFTKCAN,       /* shifted cancel key */
    SHIFTKCMD,       /* shifted command key */
    SHIFTKCPY,       /* shifted copy key */
    SHIFTKCRT,       /* shifted create key */
    SHIFTKDC,        /* shifted delete-character key */
    SHIFTKDL,        /* shifted delete-linekey */
    KSLT,       /* select key */
    SHIFTKEND,       /* shifted end key */
    SHIFTKEOL,       /* shifted clear-to-end-of-line key */
    SHIFTKEXT,       /* shifted exit key */
    KIND,       /* scroll-forward key */
    SHIFTKFND,       /* shifted find key */
    SHIFTKHLP,       /* shifted help key */
    SHIFTKHOM,       /* shifted home key */
    SHIFTKIC,        /* shifted insert-character key */
    SHIFTKLFT,       /* shifted left-arrow key */
    SHIFTKMSG,       /* shifted message key */
    SHIFTKMOV,       /* shifted move key */
    SHIFTKNXT,       /* shifted next key */
    SHIFTKOPT,       /* shifted options key */
    SHIFTKPRV,       /* shifted previous key */
    SHIFTKPRT,       /* shifted print key */
    KRI,        /* scroll-backward key */
    SHIFTKRDO,       /* shifted redo key */
    SHIFTKRPL,       /* shifted replace key */
    SHIFTKRIT,       /* shifted right-arrow key */
    SHIFTKRES,       /* shifted resume key */
    SHIFTKSAV,       /* shifted save key */
    SHIFTKSPD,       /* shifted suspend key */
    KHTS,       /* set-tab key */
    SHIFTKUND,       /* shifted undo key */
    KSPD,       /* suspend key */
    KUND,       /* undo key */
    KCUU1,      /* up-arrow key */
    RMKX,       /* leave 'keyboard_transmit' mode */
    SMKX,       /* enter 'keyboard_transmit' mode */
    LF0,        /* label on functionkey f0 if not f0 */
    LF1,        /* label on function key f1 if not f1 */
    LF10,       /* label on function key f10 if not f10 */
    LF2,        /* label on function key f2 if not f2 */
    LF3,        /* label on function key f3 if not f3 */
    LF4,        /* label on function key f4 if not f4 */
    LF5,        /* label on function key f5 if not f5 */
    LF6,        /* label on function key f6 if not f6 */
    LF7,        /* label on function key f7 if not f7 */
    LF8,        /* label on function key f8 if not f8 */
    LF9,        /* label on function key f9 if not f9 */
    FLN,        /* label format */
    RMLN,       /* turn off soft labels */
    SMLN,       /* turn on soft labels */
    RMM,        /* turn off meta mode */
    SMM,        /* turn on meta mode (8th-bit on) */
    MHPA,       /* Like column_address in micro mode */
    MCUD1,      /* Like cursor_down in micro mode */
    MCUB1,      /* Like cursor_left in micro mode */
    MCUF1,      /* Like cursor_right in micro mode */
    MVPA,       /* Like row_address #1 in micro mode */
    MCUU1,      /* Like cursor_up in micro mode */
    NEL,        /* newline (behave like cr followed by lf) */
    PORDER,     /* Match software bits to print-head pins */
    OC,         /* Set all color pairs to the original ones */
    OP,         /* Set default pair to its original value */
    PAD,        /* padding char (instead of null) */
    DCH,        /* delete #1 characters (P*) */
    DL,         /* delete #1 lines (P*) */
    CUD,        /* down #1 lines (P*) */
    MCUD,       /* Like parm_down_cursor in micro mode */
    ICH,        /* insert #1 characters (P*) */
    INDN,       /* scroll forward #1 lines (P) */
    IL,         /* insert #1 lines (P*) */
    CUB,        /* move #1 characters to the left (P) */
    MCUB,       /* Like parm_left_cursor in micro mode */
    CUF,        /* move #1 characters to the right (P*) */
    MCUF,       /* Like parm_right_cursor in micro mode */
    RIN,        /* scroll back #1 lines (P) */
    CUU,        /* up #1 lines (P*) */
    MCUU,       /* Like parm_up_cursor in micro mode */
    PFKEY,      /* program function key #1 to type string #2 */
    PFLOC,      /* program function key#1 to execute string#2 */
    PFX,        /* program function key#1 to transmit string #2 */
    PLN,        /* program label #1 to show string #2 */
    MC0,        /* print contents of screen */
    MC5P,       /* turn on printer for #1 bytes */
    MC4,        /* turn off printer */
    MC5,        /* turn on printer */
    PULSE,      /* select pulse dialing */
    QDIAL,      /* dial number #1 without checking */
    RMCLK,      /* remove clock */
    REP,        /* repeat char #1 #2 times (P*) */
    RFI,        /* send next input char (for ptys) */
    RS1,        /* reset string */
    RS2,        /* reset string */
    RS3,        /* reset string */
    RF,         /* name of reset file */
    RC,         /* restore cursor to position of last save_cursor */
    VPA,        /* vertical position #1 absolute (P) */
    SC,         /* save current cursor position (P) */
    IND,        /* scroll text up (P) */
    RI,         /* scroll text down (P) */
    SCS,        /* Select character set, #1 */
    SGR,        /* define video attributes #1-#9 (PG9) */
    SETB,       /* Set background color #1 */
    SMGB,       /* Set bottom margin at current line */
    SMGBP,      /* Set bottom margin at line #1 or (if smgtp is not given) #2 lines from bottom */
    SCLK,       /* set clock, #1 hrs #2 mins #3 secs */
    SCP,        /* Set current color pair to #1 */
    SETF,       /* Set foreground color #1 */
    SMGL,       /* set left soft margin at current column. See smgl. (ML is not in BSD termcap). */
    SMGLP,      /* Set left (right) margin at column #1 */
    SMGR,       /* set right soft margin at current column */
    SMGRP,      /* Set right margin at column #1 */
    HTS,        /* set a tab in every row, current columns */
    SMGT,       /* Set top margin at current line */
    SMGTP,      /* Set top (bottom) margin at row #1 */
    WIND,       /* current window is lines #1-#2 cols #3-#4 */
    SBIM,       /* Start printing bit image graphics */
    SCSD,       /* Start character set definition #1, with #2 characters in the set */
    RBIM,       /* Stop printing bit image graphics */
    RCSD,       /* End definition of character set #1 */
    SUBCS,      /* List of subscriptable characters */
    SUPCS,      /* List of superscriptable characters */
    HT,         /* tab to next 8-space hardware tab stop */
    DOCR,       /* Printing any of these characters causes CR */
    TSL,        /* move to status line, column #1 */
    TONE,       /* select touch tone dialing */
    UC,         /* underline char and move past it */
    HU,         /* half a line up */
    U0,         /* User string #0 */
    U1,         /* User string #1 */
    U2,         /* User string #2 */
    U3,         /* User string #3 */
    U4,         /* User string #4 */
    U5,         /* User string #5 */
    U6,         /* User string #6 */
    U7,         /* User string #7 */
    U8,         /* User string #8 */
    U9,         /* User string #9 */
    WAIT,       /* wait for dial-tone */
    XOFFC,      /* XOFF character */
    XONC,       /* XON character */
    ZEROM,      /* No motion for subsequent character */
    SCESA,        /* Alternate escape for scancode emulation */
    BICR,         /* Move to beginning of same row */
    BINEL,        /* Move to next row of the bit image */
    BIREP,        /* Repeat bit image cell #1 #2 times */
    CSNM,         /* Produce #1'th item from list of character set names */
    CSIN,         /* Init sequence for multiple codesets */
    COLORNM,      /* Give name for color #1 */
    DEFBI,        /* Define rectangular bit image region */
    DEVT,         /* Indicate language/codeset support */
    DISPC,        /* Display PC character #1 */
    ENDBI,        /* End a bit-image region */
    SMPCH,        /* Enter PC character display mode */
    SMSC,         /* Enter PC scancode mode */
    RMPCH,        /* Exit PC character display mode */
    RMSC,         /* Exit PC scancode mode */
    GETM,         /* Curses should get button events, parameter #1 not documented. */
    KMOUS,        /* Mouse event has occurred */
    MINFO,        /* Mouse status information */
    PCTRM,        /* PC terminal options */
    PFXL,         /* Program function key #1 to type string #2 and show string #3 */
    REQMP,        /* Request mouse position */
    SCESC,        /* Escape for scancode emulation */
    S0DS,         /* Shift to codeset 0 (EUC set 0, ASCII) */
    S1DS,         /* Shift to codeset 1 */
    S2DS,         /* Shift to codeset 2 */
    S3DS,         /* Shift to codeset 3 */
    SETAB,        /* Set background color to #1, using ANSI escape */
    SETAF,        /* Set foreground color to #1, using ANSI escape */
    SETCOLOR,     /* Change to ribbon color #1 */
    SMGLR,        /* Set both left and right margins to #1, #2.  (ML is not in BSD termcap). */
    SLINES,       /* Set page length to #1 lines */
    SMGTB,        /* Sets both top and bottom margins to #1, #2 */
    EHHLM,       /* Enter horizontal highlight mode */
    ELHLM,       /* Enter left highlight mode */
    ELOHLM,      /* Enter low highlight mode */
    ERHLM,       /* Enter right highlight mode */
    ETHLM,       /* Enter top highlight mode */
    EVHLM,       /* Enter vertical highlight mode */
    SGR1,        /* Define second set of video attributes #1-#6 */
    SLENGTH,     /* YI Set page length to #1 hundredth of an inch */
    LASTENUM,
    TEXTFIELD=LASTENUM, /* Used to identify text, this is not a capacity */
};

/* > */
