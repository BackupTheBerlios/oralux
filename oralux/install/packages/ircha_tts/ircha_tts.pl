#!/usr/bin/perl

# * 2005, Gilles Casse <gcasse@oralux.org> 
#   This file comes from the IrcHa project: 
#   http://www.telefonica.net/web2/ircha/
#   
#   It has been slightly customized to provide a stand-alone phonetizer
#  

# Proyecto ircHa v1.0beta5.5 (IRC Hablado)

# Traductor de texto a fonemas para MBrola y enfocado a su uso en IRC
# Basado en un código original de Alistair Conkie (1997)
# Modificado por Juan Ignacio Rodríguez
# Para un proyecto idea de Angel del Valle

#$volumen = 5; # Volumen de la voz entre 1 (bajo) y 8 (fuerte).
#$tempo = 0.8; # Tempo de la voz, 1 normal, cuanto mas pequeño mas rapido.

# Poner a cero si no se quere que lea los puntos y comas, y los considere casi todos como puntuación.
$lee_el_punto = 1;

# Poner a cero si no queremos leer las cosas que parezcan una hora
$lee_la_hora = 1;

# Poner a cero si no queremos leer las cosas que parezcan una URL
$lee_las_URLs = 1;

# Variables de entorno, en ellas estan los nombres de los ficheros y directorios que ircHa usa
# En condiciones normales no debe modificarse por el usuario

#$ircha_log = 'ircha.log';  	 	# Log de flood
#$ircha_tmp = 'ircha_';		 	# Nombre de los ficheros temporales
#$ircha_lock = 'ircha.lock';		# Nombre del fichero de bloqueo

$ircha_dir = '/usr/share/ircha';					# Para linux, el directorio donde esta el ircha
#$mbrola_linux = "$ircha_dir/mbrola -v $volumen -t $tempo $ircha_dir/es1/es1";	# Llamada al Mbrola de linux
$diccionario_linux = "$ircha_dir/diccionario";				# Diccionario en Linux
$banlist_linux = "$ircha_dir/banlist";					# Banlist en Linux
$diccionario_windows = 'diccionario';					# Diccionario en Windows
$banlist_windows = 'banlist';						# Banlist en Widows
#$mbrola_windows = "mbrola -v $volumen -t $tempo ./es1/es1";			# Llamada al Mbrola de Windows

# Poner esta variable a 1 para ver en STDERR la trascripción fonética
$debug = 0;

# Inicialización de variables

%char_corrs = (
        '%', ' porciento ',
        '#', " canal ",
        '$', ' dolar ',
        '&', " 'ampersand ",
        '*', ' asterisco ',
        '+', " m'as ",
#        '-', ' menos ',
        '/', ' barra ',
        '=', ' igual ',
        '<', '',
        '>', '',
        '@', ' arroba ',
        '\\', ' barrainvertida ',
        '^', '',
        '_', '',
        '~', '',
        '|', '',
        "'", '"',

        "\243", ' libra ', # simbolo libra esterlina
        "\300", "'a", # `A
        "\301", "'a", # 'A
        "\302", "'a", # ^A
        "\303", "'a", # ~A
        "\304", '"a', # ¨A
        "\305", "'a", # ºA
        "\306", 'ae', # AE
        "\307", "\347",  # Ç
        "\310", "'e", # `E
        "\311", "'e", # 'E
        "\312", "'e", # ^E
        "\313", '"e', # ¨E
        "\314", "'i", # 'I
        "\315", "'i", # `I
        "\316", "'i", # ^I
        "\317", '"i', # ¨I
        "\320", 'ed', # ed
        "\321", '~n', # Ñ
        "\322", "'o", # `O
        "\323", "'o", # 'O
        "\324", "'o", # ^O
        "\325", "'o", # ~O
        "\326", '"o', # ¨O
        "\327", 'x',  # x
        "\330", " vac'io ",  # 0
        "\331", "'u", # `U
        "\332", "'u", # 'U
        "\333", "'u", # ^U
        "\334", '"u', # ¨U
        "\335", "'i", # 'Y
        "\336", ' ',  # npi
        "\337", 'b',  # beta
        "\340", "'a", # `a
        "\341", "'a", # 'a
        "\342", "'a", # ^a
        "\343", "'a", # ~a
        "\344", '"a', # ¨a
        "\345", "'a", # ºa
        "\346", 'ae', # ae
#        "\347", 's',  # ç
        "\350", "'e", # `e
        "\351", "'e", # 'e
        "\352", "'e", # ^e
        "\353", '"e', # ¨e
        "\354", "'i", # `i
        "\355", "'i", # 'i
        "\356", "'i", # ^i
        "\357", '"i', # ¨i
        "\360", 'd',  # d
        "\361", "~n", # ñ
        "\362", "'o", # `o
        "\363", "'o", # 'o
        "\364", "'o", # ^o
        "\365", "'o", # ~o
        "\366", '"o', # ¨o
        "\367", 'd',  # d rara
        "\370", " vac'io ", # vacio
        "\371", "'u", # `u
        "\372", "'u", # 'u
        "\373", "'u", # ^u
        "\374", '"u', # ¨u
        "\375", "'i", # 'y
        "\376", '',   # npi
        "\377", '"i', # 'y


        ##### THERE ARE SEVERAL MISSING
        ## inverted ? 277
        ## inverted ! 241
);

# Relacion de palabras átonas
%fw = (
        'al', 'fw',
        'aun', 'fw',
        'vos', 'fw',
        'del', 'fw',
        'el', 'fw',
        'en', 'fw',
        'quien', 'fw',
        'con', 'fw',
        'cual', 'fw',
        'cuan', 'fw',
        'las', 'fw',
        'los', 'fw',
        'les', 'fw',
        'mas', 'fw',
        'mis', 'fw',
        'nos', 'fw',
        'os', 'fw',
        'pos', 'fw',
        'pues', 'fw',
        'sin', 'fw',
        'sus', 'fw',
        'tan', 'fw',
        'tras', 'fw',
        'tus', 'fw',
        'un', 'fw',

        'ante', 'no_ss',
        'hace', 'no_ss',
        'hacia', 'no_ss',
        'hasta', 'no_ss',
        'aunque', 'no_ss',
        'casi', 'no_ss',
        'desde', 'no_ss',
        'donde', 'no_ss',
        'entre', 'no_ss',
        'frente', 'no_ss',
        'como', 'no_ss',
        'contra', 'no_ss',
        'cuando', 'no_ss',
        'mientras', 'no_ss',
        'pero', 'no_ss',
        'porque', 'no_ss',
        'puesto', 'no_ss',
        'que', 'no_ss',
        'cerca', 'no_ss',
        'sino', 'no_ss',
        'sobre', 'no_ss',

);

# Duraciones básicas
%tosampa = (
        '#', '_',
        'a', 'a',
        'e', 'e',
        'i', 'i',
        'o', 'o',
        'u', 'u',
        'a:', 'a1',
        'e:', 'e1',
        'i:', 'i1',
        'o:', 'o1',
        'u:', 'u1',
        'j', 'i',
        'W', 'u',
        'w', 'w',
        'y', 'jj',
        'c', 'tS',
        's', 's',
        'z', 's',
        'p', 'p',
        'b', 'b',
        'B', 'b',
        'f', 'f',
        't', 't',
        'd', 'd',
        'D', 'd',
        'S', 'T',
        'k', 'k',
        'g', 'g',
        'G', 'g',
        'x', 'x',
        'm', 'm',
        'n', 'n',
        '~', 'J',
        'N', 'n',
        'M', 'm',
        'l', 'l',
        'L', 'L',
        'r', 'r',
        'R', 'rr',
);

%dur0 = (
        'a', '90',
        'e', '90',
        'i', '80',
        'o', '90',
        'u', '80',
        'a:', '108',
        'e:', '108',
        'i:', '96',
        'o:', '108',
        'u:', '96',
        'j', '60',
        'W', '60',
        'w', '45',
        'y', '90',
        'c', '135',
        's', '90', # Era 110 pero silbaban las eses
        'z', '60',
        'p', '100',
        'b', '60',
        'B', '80', # Era 65 pero era demasiado corto
        'f', '100',
        't', '85',
        'd', '60',
        'D', '65',
        'S', '100',
        'k', '100',
        'g', '50',
        'G', '80',
        'x', '130',
        'm', '70',
        'n', '80',
        '~', '110',
        'N', '50',
        'M', '50',
        'l', '80',
        'L', '105',
        'r', '50',
        'R', '80',
);

# Reglas de Phonetización
%ts = (
        'a',  'a',      # pata
        'e',  'e',      # pelo
        'i',  'i',      # iris
        'o',  'o',      # poco
        'u',  'u',      # luna
        "'a", 'a:',
        "'e", 'e:',
        "'i", 'i:',
        "'o", 'o:',
        "'u", 'u:',
        '"u',  'W',

        # Semiconsonantes

        'ia', 'j a',
        'ie', 'j e',
        'io', 'j o',
        'iu', 'j u',
        'ua', 'W a',
        'ue', 'W e',
        'ui', 'W i',
        'uo', 'W o',
        "i'a", 'j a:',
        "i'e", 'j e:',
        "i'o", 'j o:',
        "i'u", 'j u:',
        "u'a", 'W a:',
        "u'e", 'W e:',
        "u'i", 'W I:',
        "u'o", 'W o:',

        #
        # Diptongos
        # a_i   ai      baile
        # a_i   ay      estay
        # a_u   au      causa
        # e_i   ei      reina
        # e_i   ey      rey
        # e_u   eu      deuda
        # o_i   oi      oiga
        # o_i   oy      soy

        'y', 'i',       # y       STILL TO FIGURE OUT postulate only #y#

        #               (repostulate: not following vowel)
        #
        # Semiconsonants
        # I know where they should go
        # Some of it is extra-word......
        #
        # Consonants

        'b',  'b',      # bomba
        'v',  'b',      # enviar          assimilation effects to be calculated

        #                               for both bits
        #                               after m and n (n is pronounced m)
        #                               what conditions
        # (some b's appear to be silent eg obscuro)
        #

        'ce',  'S e',   # cero
        'ci',  'S i',   # cinco
        "c'e", 'S e',
        "c'i", 'S i',   # cinco
        'c',   'k',     # casa

        'ch', 'c',      # mucho

        'd', 'd',       # dama            assimilation effects to be calculated

        #                               after silence or n,l

        'f', 'f',       # foto

        'ge', 'x e',    # general
        'gi', 'x i',    # ginebra
        "g'e", 'x e:',
        "g'i", 'x i:',
        'gue', 'g e',
        'gui', 'g i',
        "gu'e", 'g e:',
        "gu'i", 'g i:',
        'g', 'g',       # gloria          assimilation effects to be calculated

        # various gu combinations

        'hue', 'w e',   # huesca
        'hie', 'y e',   # hierro
        'h',    '\\-',  # hecho

        'j', 'x',       # jota            outstanding question re final j silent??

        'k', 'k',       # kilo

        'l', 'l',       # lelo

        'll', 'L',      # llama

        #'ll+\.',  'L', # catalan names

        'm', 'm',       # mano            (and perhaps n at the end of a word)

        'n', 'n',       # pan             assimilation effects to be calculated

        #                               see b/v

        '~n', '~',      # niño

        'p', 'p',       # papa
        'ph', 'f',      # Raphael 8-)

        'que', 'k e',   # que
        'qui', 'k i',   # quien
        "qu'e", 'k e:',
        "qu'i", 'k i:',
        'q',    'k',    # quorum          this subtlty is not explained in many places

        #                               look at quorum and quantum in particular.

        'rr', 'R',      # torre
        'r', 'r',       # coro

        # R     r       rapido          (beginning of word and after l,n,s) hence assim
        #
        #sb       z b      ?               does this apply across word boundaries too?
        #sd       z d      desde
        #sg       z g      asgo
        #sl       z l      ?
        #sm       z m      fantasma
        #sn       z n      desde

        's', 's',       # casa            sometimes gets swallowed if there is rr after
        'sh', 'c',      # shock

        #                               (this is an assimilation)
        #
        #        's+cons', 'e_s',       expand this one I think
        #

        't', 't',       # tabla

        'w', 'w',       # week-end

        'x', 'k s',     # taxi            needs some refinement!!
        'xt', 't s t',  # texto

        # x     s      pretexto        look elsewhere for conditions suppoesdly, but examples??
        'exa', 'e G s a',       # exacto,examen
        #

        'ya', 'y a',    # ya
        'ye', 'y e',    # yeso
        'yi', 'y i',
        'yo', 'y o',
        'yu', 'y u',
        "y'a", 'y a:',
        "y'e", 'y e:',
        "y'o", 'y o:',
        "y'u", 'y u:',

        #                       (there is also stuff like conyugal)
        #                       (which is a bit assimilative
        #

        'ç', 's',
        'z', 'S',

        #
        # Look at some interesting words like
        # chalet, gag,jazz,shock
        #
        # what to do about double consonants (Frapp)
        #
        # how importand is knowing syllable/word boundaries to assimilation
        # assimilations (bdg ptk)
        #
        # semiconsonants....
        #
        # the intra and extra word phonemena need to be treated separately I reckon.
        #
        # I think accents should be specifically included.

        '\\-', '.',     # eg sub-rayar
        '\\|', '|'      # word boundary
);

# Estos son los datos para la rutina de conversión de números a letras
@unidades = ('cero', 'uno', 'dos', 'tres', 'cuatro', 'cinco', 'seis', 'siete', 'ocho', 'nueve');
@decenas1 = ('cero', 'diez', 'veinte', 'treinta', 'cuarenta', 'cincuenta', 'sesenta', 'setenta', 'ochenta', 'noventa');
@decenas2 = ('cero', 'dieci', 'venti', 'treinta y ', 'cuarenta y ', 'cincuenta y ', 'sesenta y ', 'setenta y ', 'ochenta y ', 'noventa y ');
@excepciones = ('diez','once', 'doce', 'trece', 'catorce', 'quince', 'dieciseis', 'diecisiete', 'dieciocho', 'diecinueve');
@centenas = ('cien', 'ciento', 'doscientos', 'trescientos', 'cuatrocientos', 'quinientos', 'seiscientos', 'setecientos', 'ochocientos', 'novecientos');
@prefijos = ('', 'mi', 'bi', 'tri', 'cuatri', 'quinti', 'sexti', 'septi', 'hepti', 'octi');

# Estos son los datos para la rutina de conversión de números ordinales a letras
@uni_ord = ('cero', 'primer', 'segund', 'tercer', 'cuart', 'quint', 'sext', 'séptim', 'octav', 'noven');
@dec_ord = ('cero', 'décim', 'vigésim', 'trigésim', 'cuadragésim', 'quincuagésim', 'sexagésim', 'septuagésim', 'octagésim', 'nonagésim');
@excep_ord = ('décim', 'undécim', 'duodécim');
@pref_ord = ('centésim', '', 'duo', 'tri', 'cuadra', 'quincua', 'sexa', 'septa', 'octa', 'nona');

# Diccionario de sustitución de palabras
%diccionario = (
                '-', 'guión',
                '_', 'sub rallado',
                '<', 'menor que',
                '>', 'mayor que',
                '<=', 'menor o igual que',
                '>=', 'mayor o igual que',
                '^', 'circunflejo',
                '~', 'tilde',
                '|', 'barra vertical',
                'etc', 'etcetera'
             );

# Diccionario de deletreo
%deletrear = (
                'a', 'a',
                'b', 'be',
                'c', 'ce',
                'd', 'de',
                'e', 'e',
                'f', 'efe',
                'g', 'ge',
                'h', 'ache',
                'i', 'i',
                'j', 'jota',
                'k', 'ka',
                'l', 'ele',
                'm', 'eme',
                'n', 'ene',
                'ñ', 'eñe',
                'o', 'o',
                'p', 'pe',
                'q', 'ku',
                'r', 'erre',
                's', 'ese',
                't', 'te',
                'u', 'u',
                'v', 'uve',
                'w', 'uvedoble',
                'x', 'ekis',
                'y', 'igriega',
                'z', 'zeta',
                'ç', 'cedilla'
             );

# Lista de prohibición
@banlist = ();

# Calculo del entorno operativo y el directorio temporal
$Windows = $ENV{'windir'} ne ''; # Estamos en windows?
$tmpdir = $Windows ? $ENV{'TEMP'} : '/tmp/';

# Cambiamos al directorio, si no en windows da problemillas
#GC
# if($Windows)
# {
# 	$_ = rindex($0, '\\');
# 	chdir(substr($0, 0, $_)) if($_ != 0);

# 	require "play_win.pl";
	
# 	$mbrola_cmd = $mbrola_windows;
# 	$diccionario = $diccionario_windows;
# 	$banlist = $banlist_windows;
# }
# else
{
#	$mbrola_cmd = $mbrola_linux;
	$diccionario = $diccionario_linux;
	$banlist = $banlist_linux;
}

# Leemos el diccionario

open(FILE, $diccionario);
while(<FILE>)
{
        chop;
        if((! /^\#/) && ($_ ne ''))
        {
                ($a, $b) = split(/[\t ]/, $_, 2);
                $diccionario{lc $a} = lc $b;
        }
}
close(FILE);

# Leemos la lista de prohibiciones

open(FILE, banlist);
while(<FILE>)
{
        chop;
        push(@banlist, $_) if((! /^\#/) && ($_ ne ''));
}
close(FILE);


# Si hay linea de comandos la tomamos como cadena a leer, sino tomamos la entrada salida

if($#ARGV == -1)
{
	@_ = <STDIN>;
	$_ = join(' ', @_);
}
else
{

	$_ = join(' ', @ARGV);
}
lee($_);

# Rutina principal de lectura,
# preprocesa la entrada, la divide en frases
# y la envia al proceso de sintesis
sub lee
{
        local $_ = lc $_[0];
        my($i, $buffer, @sentencelist);
	
        # Eliminamos los códigos de colorines del irc

        s/\003\d\d,\d\d//g;
        s/\003\d\d,\d//g;
        s/\003\d,\d\d//g;
        s/\003\d,\d//g;
        s/\003\d\d//g;
        s/\003\d//g;

        # Filtrado de caracteres Extraños

	tr/[\0-\37]//;
        tr/[\40-~\243\246\252\272\300-\377]/ /c;

        $_ = diccionario($_); # Primera pasada de diccionario

        # Con esto intentamos evitar redundancias y discrepancias en la puntuación

        tr/'\t\n/"  /;                           # Los apostrofes los confunde con acentos y los tabuladores son espacios
        tr/?!,:;".()[]{}-//s;                    # Eliminamos redundancias en la puntuación
        s/([\[\(\{?!,:;\"\.\\\/\}\)\]])/ $1 /g;  # "Espaciamos" las puntuación
        tr/\(\)\[\]\{\}\"]/,,,,,,,/;             # "Simplificamos" la puntuación

        $_ = diccionario($_); # Segunda pasada de diccionario

        tr/\252\272/  /; # Eliminamos los ordinales que hayan podido quedar
        tr/ //s;         # Eliminamos espacios redundantes

# 	# Rudimentario control de flood, por si alguien repite insistentemente una frase
# 	$i = (stat("$tmpdir/$ircha_log"))[9];
# 	if(time() <= ($i + 10))
# 	{
#         	open(FILE, "<$tmpdir/$ircha_log");
#         	$buffer = <FILE>;
#         	chop($buffer);
#         	close(FILE);
        
#         	if($buffer eq $_)
#         	{
#         		print STDERR "Protección antiflood activada.\n";
#         		return;
# 		}
# 	}

# 	open(ANT, ">$tmpdir/$ircha_log");
# 	print ANT "$_\n";
# 	close(ANT);
	###

	# Lista de prohibición, si se menciona lo contenido en la lista, no se lee
	foreach $i(@banlist)
	{
		if(/$i/)
		{
	       		print STDERR "Banlist activada ($i).\n";
	       		return;
		}
	}
	###
	

        if(! /^[ ]*$/) # Si hay algo que procesar;
        {
                @sentencelist = ();
                foreach (split(/([.!?]) /)) # Lo dividimos en frases
                {
                        if(/^[\.!?]$/)
                        {
                                $sentencelist[$#sentencelist] .= $_; # Si es un separador de frase lo añadimos a la ultima frase.
                        }
                        else
                        {
                                # Sustituimos caracteres como los acentos por sus equivalentes prefonéticos
                                $buffer = '';
                                foreach $i (split(//))
                                {
                                        $buffer .= defined($char_corrs{$i}) ? $char_corrs{$i} : $i;
                                }
                                push(@sentencelist, $buffer);
                        }
                }

                process_and_send_to_synth(@sentencelist); # Y procesamos
        }
}

# Sustituye palabras del diccionario por sus equivalentes,
# adapta cosas como las URL a una formato "legible",
# y traduce los numeros a letras.
sub diccionario
{
        my @line = ();
        my $punto;

        foreach (split(/\s+/, $_[0]))
        {

                if(defined $diccionario{$_}) # Una palabra a convertir
                {
                        $_ = $diccionario{$_};
                        $punto = '';
                }
                else
		{
                        if(/[\.,]$/)
                        {
                               	tr/\.//s if(/\.\.\.$/);
                               	tr/\,//s if(/\,\,\,$/);                        
                                $punto = substr($_, length($_)-1);
                               	$_ = substr($_, 0, -1);
                        }
                        else
                        {
                                $punto = '';
                        }
        
              		if($lee_la_hora && (/(\d+):(\d+):(\d+)/ || /(\d\d)h(\d\d)m(\d\d)s/i))
              		{
              			$_ = "$1 horas $2 minutos $3 segundos";
              		}
        		elsif($lee_la_hora && /(\d\d):(\d\d)/)
        		{
        			$_ = "$1 hora $2 minutos";
        		}
        		elsif($lee_la_hora && /(\d\d)m(\d\d)s/i)
        		{
        			$_ = "$1 minutos $2 segundos";
        		}
                        elsif($lee_las_URLs && (/.*:\/\/.*\..*/ || /www\..*/ || /ftp\..*/))
                        {
                                s/\./ punto /g;
                                s/:/ dos puntos /g;
                                s/\?/ interrogación /g;
                                s/\-/ guión /g;
                        }
                        elsif(/[0-9]/)
                        {
                                my $tmp = '';
                                my @p = split(//);
                                for(my $i = 0; $i <= $#p; $i++)
                                {
                                        $tmp .= $p[$i];
                                        if(($p[$i] =~ /[0-9,\.\-\252\272]/) && ($p[$i + 1] !~ /[0-9,\.\-\252\272]/))
                                        {
                                                push(@line, numero2letras($tmp));
                                                $tmp = '';
                                        }
                                        elsif(($p[$i] !~ /[0-9,\.\-\252\272]/) && ($p[$i + 1] =~ /[0-9,\.\-\252\272]/))
                                        {
                                                push(@line, $tmp);
                                                $tmp = '';
                                        }
                                }
                                $_ = $tmp;
                        }
                        elsif((/^(\.)(.*)/) && ($2 ne ''))
                        {
                                $_ = "punto $2";
                        }
                        elsif($lee_el_punto && /\./)
                        {
                                s/\./ punto /g;
                        }
                }

                push(@line, "$_$punto")
        }

        return join(' ', @line);
}

# Rutina principal del sintetizador fonético.
sub process_and_send_to_synth
{
        my @xxxx_str = ();
        my @tokens;

        foreach (@_) # Para cada frase
        {
                @tokens = treat_and_tags($_);
                @tokens = transcribe(@tokens);
                @tokens = conv(@tokens);
                @tokens = massage(@tokens);
                @tokens = prosody(@tokens);
                @tokens = change_phonemes(@tokens);

                push(@xxxx_str, @tokens);
        }

        # Una vez tenemos la trasnscripción fonetica, lo enviamos al sintetizador vocal
#GC
	print join("\n", @xxxx_str)."\n"
}

# Rutina de division de palabras,
# divide las frases en palabras y realiza un pequeño proceso de excepciones
# como las palabra empezadas en sub, los guiones, las palabras terminadas
# en mente o las que deben deletrearse.
# Tambien añade los tags de entonación que cada palabra requiere.
sub treat_and_tags
{
        my @words = ();
        my $i;

        foreach (split /\s+/, $_[0]) # Dividimos en palabras y hacemos algunos retoques
        {
                if(/.+\-.+/ )
                {
                        tr/\-/|/;
                }
                elsif(/^sub(lev|lin|lun|ray|rei|rep|rog)/)
                {
                       s/^sub/sub\-/;
                }
                elsif(/mente$/)
                {
                        if($_ !~ /^(aumente|lamente|tormente)$/)
                        {
                                s/mente$/|mente/;
                        }
                }
                elsif(/^(va|ve|vi|da|di|fe|ha|he|no|ti|yo|ya)$/ )
                {
                       s/(.)(.)/$1'$2/;
                }
#               elsif(/[a-z'"~]/)
                elsif(! /[aeiouy?!\.,:;\-]/)
                {
#                       $vocales = tr/[aeiouyw'"]// * 3;
#                       $otras = tr/[aeiouy'"]//c;
#                       if($otras > $vocales)
#                       {
                                foreach (split(//))
                                {
                                        push(@words, "$deletrear{$_}/CW");
                                }
                                next;
#                       }
                }

		# Añadimos los tags de entonación de la frase
                push(@words, '/PUNCT') if(($words[$#words] =~ /\/CW$/) && defined($fw{$_}));

	        # Añadimos los "tags" de entonación a las palabras
        	# Es una palabra átona, una palabra normal o puntuación?
                push(@words, /[a-z]/ ? (defined($fw{$_}) ? "$_/FW" : "$_/CW") : "$_/PUNCT");
        }

        push(@words, './PUNCT') if($words[$#words] !~ /^[?!\.]/);

        return @words;
}

# Transcribe la frase a fonemas, palabra por palabra
sub transcribe
{
        my $trans;
        my $flag = 0;

        foreach (@_)
        {
                /(.*)\/(.*)/;
                if($2 ne 'PUNCT') # De momento ignoramos la puntuación
                {
                        $trans = transcribe_word($1);
                        $trans = add_missing_stresses($trans) if($2 eq 'CW');
                        $_ = "$trans/$2";

                        ## Con esto aun tenemos el problema de que hacer, precisamente, con
                        ## las palabras no acentuadas (FW)
                        
                        $flag = 1;
                }
        }

	return $flag ? @_ : (); # Si todo es puntuación, no hay nada que pronunciar
}

# Marca las diferentes entonaciones de las frases
sub conv
{
        my @outlist = ();
        my $j = 0;
        my $syll_num = 0;
        my($k, $duration);

        $outlist[$j++] = '# 50 H[<1.0>';
        foreach $_ (@_)
        {
                /(.*)\/(.*)/;
                if($2 eq 'PUNCT')
                {
                        $k = &last_vowel(@outlist);

                        if($1 =~ /[:-]/)
                        {
                                $outlist[$k] .= 'L-H]<0.7>';
                                $duration = 250; ### was 750
                        }
                        elsif($1 eq ';')
                        {
                                $outlist[$k] .= 'L-L]<0.7>';
                                $duration = 250; ### was 750
                        }
                        elsif($1 eq '.')
                        {
                                $outlist[$k] .= 'L-H]<1.0>';
                                $duration = 250; ### was 1200
                        }
                        elsif($1 eq '!')
                        {          # not right
                                $outlist[$k] .= 'H-L]<1.0>';
                                $duration = 250; ### was 1200
                        }
                        elsif($1 eq '?')
                        {
                                $outlist[$k] .= 'L-L]<1.0>';
                                $duration = 250; ### was 1200
                        }
                        elsif ($1 eq ',')
                        {
                                $outlist[$k] .= 'L-H]<0.7>';
                                $duration = 200;
                        }
                        else
                        { # añadido para prevenir un caracter desconocido
                                $outlist[$k] .= 'L-H]<0.7>';
                                $duration = 10;
                        }
                        $outlist[$j++] = "# $duration";
                }
                else
                {
                        foreach (split(' ', $1))
                        {
                                if(! /\-/)
                                {
                                        if(defined($dur0{$_}))
                                        {
                                                # Es  un fonema real, e incluso podria ser acento de la frase
                                                $outlist[$j++] = "$_ $syll_num ".(/:$/ ? 'H*<0.5>' : '');
                                        }
                                        elsif (/[\+\-\.\|]/)
                                        {
                                                $syll_num++;
                                        }
                                        else
                                        {
                                                print STDERR "Algo desconocido: $j\n";
                                        }
                                }
                        }
                        $syll_num++;
                }
        }

        $outlist[$j] = '# 1000' if(($j eq 1) || ($outlist[$j - 1] !~ /^#/));

        return @outlist;
}

# Retorna la posicion de la ultima vocal
sub last_vowel
{
        my($i, @phoneme);

        for($i = $#_; $i >= 0; $i--)
        {
                @phoneme = split(' ', $_[$i]);
                if($phoneme[0] =~ /[aeiou]/)
                {
                        return $i;
                }
        }
        return $#_;

        print STDERR "No hay vocal!!\n";
}

# Ahora que conocemos las pausas este procedimiento hace las asimilaciones
sub massage
{
        my($mi, $a, $b, $c);
        my $l = $#_;

        my @phonss = ();
        my @syllns = ();
        my @pcomms = ();

        foreach (@_)
        {
                ($a, $b, $c) = split(' ');
                push(@phonss, $a);
                push(@syllns, $b);
                push(@pcomms, $c);
        }

        for($mi = 0; $mi <= $l; $mi++)
        {
                $phonss[$mi] = uc $phonss[$mi] if(($phonss[$mi] =~ /[bgd]/) && (!$mi || ($phonss[$mi - 1] =~ /[MN#]/) && ($syllns[$mi - 1] != $syllns[$mi])))
        }

        for($mi = 0; $mi <= $l - 2; $mi++)
        {
                if(($phonss[$mi] eq "k") && ($phonss[$mi + 1] eq "s") && ($syllns[$mi + 1] != $syllns[$mi + 2]))
                {
                        $phonss[$mi] = -1;
                }
	}

#        for($mi = 0; $mi <= $l - 1; $mi++)
#        {
#                if(($phonss[$mi] eq "s") && ($phonss[$mi + 1] eq "R") && ($syllns[$mi] != $syllns[$mi + 1]))
#                {
#                        $phonss[$mi] = -1;
#                }
#	}

        for($mi = 0; $mi <= $l - 1; $mi++)
        {
                if(($phonss[$mi] eq $phonss[$mi + 1]) && ($phonss[$mi] !~ /[aeiou]/))
                {
                        $phonss[$mi] = -1;
                }
 
        }

        for($mi = 0; $mi <= $l - 1; $mi++)
        {
                if(($phonss[$mi] eq 'k') &&
                  (($syllns[$mi] != $syllns[$mi + 1]) ||
                   ($mi <= $l - 2) && ($phonss[$mi+1] eq 's') && ($syllns[$mi + 1] != $syllns[$mi + 2])))
                {
                        $phonss[$mi] = 'G';
                }

                if(($phonss[$mi] eq 'p') &&
                  (($syllns[$mi] != $syllns[$mi + 1]) ||
                   ($mi <= $l - 2) && ($phonss[$mi + 1] eq 's') && ($syllns[$mi + 1] != $syllns[$mi + 2])))
                {
                        $phonss[$mi] = 'B';
                }

                if(($phonss[$mi] eq 't') &&
                  (($syllns[$mi] != $syllns[$mi + 1]) ||
                   ($mi <= $l - 2) && ($phonss[$mi + 1] eq 's') && ($syllns[$mi + 1] != $syllns[$mi + 2])))
                {
                        $phonss[$mi] = 'D';
                }

#                if(($phonss[$mi] eq 's') && ($phonss[$mi + 1] =~ /[Sbdgr]/) && ($syllns[$mi] != $syllns[$mi + 1]))
#                {
#                        $phonss[$mi] = 'S';
#                }
        }

        @_ = ();
        for($mi = 0; $mi <= $l; $mi++)
        {
                push(@_, join(' ',($phonss[$mi],$syllns[$mi],$pcomms[$mi]))) if($phonss[$mi] ne -1);
        }

        return @_;
}

# Cuenta el numero de silabas
sub snum
{
        my @input = split(' ', $_[0]);

        return $input[0] ne '#' ? $input[1] : -1;
}

# Prosodia de la frase (entonación)
sub prosody
{
        my @phonemelist = @_;

	my(@phoneme, $i, $fop);
        my @spnlist = ();
        my @spnlistreserve = ();
        my $curr_snum = -1;         # just for starting off
        my $ref_snum = -1;          # just for starting off

        for($i = 0;$i <= $#phonemelist; $i++)
        {              # each time we find a symbol we make a curve
                @phoneme = split(' ',$phonemelist[$i]);
                $spnlist[$i] = "$phoneme[0]";
                $curr_snum = snum($phonemelist[$i]);
                $spnlist[$i] .= ($phoneme[0] ne '#') ? "\t$dur0{$phoneme[0]}" : "\t$phoneme[1]";

                if($i eq 0)
                {
                        $spnlist[$i] .= "\t(0,120)";
                        next;
                }

                if($i eq $#phonemelist)
                {
                        $spnlist[$i] .= "\t(99,80)";
                        next;
                }

                # need to find max and min with same syllable number.
                if($curr_snum ne $ref_snum)
                {
                        $sos = $i;
                        $ref_snum = $curr_snum;
                }

                if($#phoneme eq 2)
                {
                        $fop = $i;
                        while(snum($phonemelist[$fop]) eq $curr_snum)
                        {
                                $fop++;
                        }
                        $fop--;         # we overshot slightly

                        if($phoneme[2] =~ /^H\*<(\d+\.\d+)>$/)
                        {
                                $spnlist[$sos] .= "\t(0,100)";
                                $spnlist[$i] .= "\t(30,130)";
                        }
                        elsif($phoneme[2] =~ /^H\[<(\d+\.\d+)>$/)
                        {
                        }
                        elsif($phoneme[2] =~ /^L-H\]<(\d+\.\d+)>$/)
                        {
                                $spnlistreserve[$fop] .= "\t(90,100)";
                        }
                        elsif($phoneme[2] =~ /^H\*<.\..>L-H\]<(\d+\.\d+)>$/)
                        {
                                $spnlist[$sos] .= "\t(0,100)";
                                $spnlist[$i] .= "\t(30,130)";
                                $spnlist[$i] .= "\t(80,100)";
                                $spnlistreserve[$fop] .= "\t(99,120)";
                        }
                        elsif($phoneme[2] =~ /^L-L\]<(\d+\.\d+)>$/) # interrogación
                        {
                                $spnlistreserve[$fop] .= "\t(90,180)"; # originariamente (99,80)
                        }
                        elsif($phoneme[2] =~ /^H\*<.\..>L-L\]<(\d+\.\d+)>$/) # interrogación
                        {
                                $spnlist[$sos] .= "\t(0,100)";
                                $spnlist[$i] .= "\t(30,130)";
#                                $spnlist[$i] .= "\t(80,90)";
                                $spnlistreserve[$fop] .= "\t(90,180)"; #originalmente (99,80)
                        }
                        elsif($phoneme[2] =~ /^H-L\]<(\d+\.\d+)>$/) # Admiración
                        {
                                $spnlistreserve[$fop] .= "\t(99,80)";
                        }
                        elsif($phoneme[2] =~ /^H\*<.\..>H-L\]<(\d+\.\d+)>$/) # Admiración
                        {
                                $spnlist[$sos] .= "\t(0,100)";
                                $spnlist[$i] .= "\t(30,130)";
                                $spnlist[$i] .= "\t(80,90)";
                                $spnlistreserve[$fop] .= "\t(99,80)";
                        }
                        else
                        {
                                print STDERR "Hay problemas para encontrar la entonación correcta.\n";
#                                print STDERR "Phoneme = $phoneme[2]\n";
                        }
                }

        }

        for($i=0; $i <= $#spnlist; $i++)
        {
                $spnlist[$i] .= $spnlistreserve[$i];
        }

        return @spnlist;
}

# Convierte la fonetización interna al formato del MBrola
sub change_phonemes
{
        my @llist = @_;
        my($l, @pline);

        foreach (@llist)
        {
                @pline = split(' ');
                $pline[0] = $tosampa{$pline[0]};
                $pline[0] =~ s/1//;
                $_ = join("\t", @pline);

#                s/\(/ /g;
#                s/\)/ /g;
#                s/,/ /g;

#                s/[\(\),]/ /g;
        }

        return @llist;
}



# Transcribe una palabra a fonemas y hace la división silabica
sub transcribe_word
{
       $_ = (find_match($_[0]))[0];

#       print STDERR "Transcripcion: $_\n";

        # algunas cosas con cierto contexto

        s/^r/R/;                # Se escribe r y no rr a principio de palabra
        s/(\.|n|l|s) r/$1 R/;   # y antes de n l s
        s/b $/B/;		# la B se pronuncia mas larga al final de palbra

        return $_;
}

# Busca las consonancias entre las letras y los fonemas y realiza la autentica transcripción fonética
sub find_match
{
        my $word = $_[0];

        my($key, $t, $tv);

        return ('', 1) if($word eq '');

        foreach $key (reverse(sort(keys(%ts))))
        {
                if($word =~/^($key)(.*)/)
                {
                        ($t, $tv) = find_match($2);
                        return ("$ts{$key} $t", 1) if($tv eq 1);
                }
        }

        return ('', 0);
}

# Añade loa acentos a las palabras que no llevan tilde
sub add_missing_stresses
{
	# Queremos implementar dos patrones bastante complejos
	# que porque tan complejos? porque tienes que serlo
	# Primero, si la palabra termina en n, s, o vocal y tiene 2 silabas
	# añadimos una acento si es necesario

        my @bits = split(/\|/, $_[0]); # Divide por silabas forzadas como en las palabras terminadas en mente
        my $found_stress = 0;
        my($e, $l);

        foreach $bitphons (@bits)
        {
                if($bitphons =~ /:/)
                {
                        # Esta ya tiene el acento puesto, nada que hacer
                        $found_stress = 1;
                        next;
                }

                @phonlist = split(' ', $bitphons);
                $l = $#phonlist;

 		# Primer caso, la penultima
                if($bitphons =~ /[aeiousn][ ]?$/)
                {
                        for($e = 0; $e <= $l; $e++)
                        {
                                if($phonlist[$e] =~ /^[aeiou]$/)
                                {
                                        # otras condiciones que satisfacer...
                                        if(($e <= $l - 2) && ($phonlist[$e + 1] =~ /^[iu]$/))
                                        {
                                                $look = $e + 2;
                                        }
                                        elsif($e <= $l - 1)
                                        {
                                                $look = $e + 1;
                                        }

                                        while(($look <= $l) && ($phonlist[$look] !~ /^[aeiou]$/))
                                        {
                                                $look++;
                                        }

                                        if(($look == $l) && ($phonlist[$look] =~ /^[aeiou]$/) ||
                                           ($phonlist[$look] =~ /^[aeiou]$/) &&
                                           (($look == ($l - 1)) && ($phonlist[$look + 1] =~ /^[sn]$/) ||
                                            ($look == ($l - 2)) && ($phonlist[$look + 1] =~ /^i$/) && ($phonlist[$look + 2] =~ /^s$/)))
                                        {
                                                $found_stress = 1;
                                                $phonlist[$e] .= ':';
                                                last;
                                        }
                                }
                        }
                }

                # Segundo caso, la ultima silaba
                if(($found_stress == 0) && ($l >= 1))
                {
                        if(($phonlist[$l - 1] =~ /[aeiou]/) && ($phonlist[$l] =~ /[aeiou]/) ||
                           ($phonlist[$l - 1] =~ /[aeioujW]/) && ($phonlist[$l] !~ /[aeiou]/))
                        {
                                $phonlist[$l - 1] .= ':';
                        }
                        elsif($l >= 2)
                        {
                                if(($phonlist[$l - 2] =~ /[aeiou]/) &&
                                  (($phonlist[$l - 1] !~ /[aeiou]/) && ($phonlist[$l] !~ /[aeiou]/) ||
                                   ($phonlist[$l - 1] =~ /[iu]/) && ($phonlist[$l] !~ /[aeiour]/)))
                                {
                                        $phonlist[$l - 2] .= ':';
                                }
                        }
                }

                $bitphons = join(' ', @phonlist);
        }

        return join(" . ", @bits);
}

# Convierte los numero a su correcta forma en letras
sub numero2letras
{
        my @out = ();
        my @n = split(/([\.,\-\252\272])/, $_[0]);
        my $i;
        for($i = 0; $i <= $#n; $i++)
        {
                push(@out, $n[$i] eq ''   ? '' :
                	   $n[$i] eq '.'  ? ' punto ' :
                           $n[$i] eq ','  ? ' coma ' :
                           $n[$i] eq '-'  ? ' menos ' :
                           $n[$i] =~ /[\252\272]/ ? ' ' :
                           $n[$i + 1] =~ /[\252\272]/ ? ordinal_letra($n[$i], $n[$i + 1]) :
                                                        numero_letra($n[$i]));

        }
        return join(' ', @out);
}

sub numero_letra
{
        my $numero = $_[0];

        return $unidades[$numero] if ($numero < 10);

        if ($numero < 100)
        {
                @_ = split//,$numero;
                return do
                        {
                                $_[1] == 0      ? $decenas1[$_[0]]:
                                $numero < 20    ? $excepciones[$_[1]]:
                                                  "$decenas2[$_[0]]$unidades[$_[1]]";
                        }
        }
        elsif ($numero < 1000)
        {
                @_ = split//,$numero;
                return $centenas[0] if($numero == 100);
                return $centenas[$_[0]]. do
                        {
                                ($_[1] + $_[2]) == 0    ? '':
                                $_[1] == 0              ? " $unidades[$_[2]]":
                                $_[2] == 0              ? " $decenas1[$_[1]]":
                                $_[1] == 1              ? " $excepciones[$_[2]]":
                                                          " $decenas2[$_[1]]$unidades[$_[2]]";
                        }
        }
        elsif($numero < 1000000)
        {
                my $miles = int($numero / 1000);
                my $cientos = $numero % 1000;

                return ($miles == 1 ? '' : &numero_letra($miles).' ').'mil'.($cientos == 0 ? '' : ' '.&numero_letra($cientos));
        }
        else
        {
                my $i = 0;
                my $letras = '';
                while ($numero > 0)
                {
                        if($numero < 1000000)
                        {
                                $_ = $numero + 0; # Es importante esta suma de cero
                                $numero = 0;
                        }
                        else
                        {
                                $_ = substr($numero, length($numero) - 6) + 0; # Es importante esta suma de cero
                                $numero = substr($numero, 0, length($numero) - 6);
                        }
                        if($_ != 0)
                        {
                                $_ = &numero_letra($_).' '.$prefijos[$i].(!$i ? '' : $_ > 1 ?  'llones' : 'llón');
                                s/uno/un/ if($i != 0);
                                $letras = $_." $letras";
                        }
                        $i++;
                }
                $letras =~ s/cero//g;
                $letras =~ tr/ //s;
                return $letras;
        }
}

sub ordinal_letra
{
        my $numero = $_[0];
        my $genero = $_[1] eq "\252" ? 'a' : 'o';

        return "$uni_ord[$numero]$genero" if ($numero < 10);

        if ($numero < 100)
        {
                @_ = split//,$numero;
                return do
                        {
                                $_[1] == 0      ? $dec_ord[$_[0]]:
                                $numero < 13    ? $excep_ord[$_[1]]:
                                                  "$dec_ord[$_[0]]$genero $uni_ord[$_[1]]";
                        }.$genero;
        }
        elsif ($numero < 1000)
        {
                @_ = split//,$numero;
                return "$pref_ord[$_[0]]centésim$genero". do
                        {
                                ($_[1] + $_[2]) == 0    ? '':
                                $_[1] == 0              ? " $uni_ord[$_[2]]$genero":
                                $_[2] == 0              ? " $dec_ord[$_[1]]$genero":
                                ($_[1] == 1) && ($_[2] =~ /[012]/) ? " $excep_ord[$_[2]]$genero":
                                                          " $dec_ord[$_[1]]$genero $uni_ord[$_[2]]$genero";
                        }
        }
        elsif($numero < 10000)
        {
                return "milésim$genero" if($numero == 1000);
                return $pref_ord[substr($numero, 0, 1)]."milésim$genero ".ordinal_letra($numero % 1000, $_[1])
        }
        else
        {
                return 'el ordinal de '.numero_letra($numero);
        }
}

