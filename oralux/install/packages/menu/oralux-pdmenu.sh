#!/bin/sh

export Doc="/usr/share/oralux/doc"
export Games="/usr/share/oralux/install/packages/games"

export RussianText="/usr/share/oralux/install/packages/menu/oralux-pdmenu.ru"

export exitText="Press the Escape key to exit the current menu"
export YasrText="YASR documentation "
export YasrDoc="$Doc/Yasr/en/README.oralux"
export SpeakupText="Speakup documentation "
export SpeakupDoc="$Doc/Speakup/spkguide.txt"
export LinksText="Links, web browser"
export LinksDoc="$Doc/htm/faq.htm"
export NeText="N E, the nice editor"
export mcvoxText="m c vox, file browser"
export mcvoxDocText="m c vox documentation"
export mcvoxDoc="$Doc/mcvox/en/mcvox.txt"
export qalcText="qalc, calculator"
export mocText="moc, console audio player"
export podracerText="podracer, podcast receiver with BitTorrent support"
export linphonecText="linphonec, SIP internet phone"
export imcomText="imcom, jabber client"
export aventureText="play a game\: Adventure"
export aventureDoc="$Games/en/Advent.z5"
export shellText1="Drop to a shell"
export shellText2="Type 'exit' to return to the menu."
export dateText="Display the time"
export dosemuText1="dosemu, run DOS under Linux"
export dosemuText2="Type 'exitemu' to return to the menu."

case "$LANGUAGE" in 
de*|at*|ch*)
	export exitText="Drücken Sie Escape, um das aktuelle Menu zu verlassen"
	export YasrText="YASR Dokumentation"
	export YasrDoc="$Doc/Yasr/de/README.oralux"
	export SpeakupText="Speakup Dokumentation (Englische)"
#	export SpeakupDoc="$Doc/Speakup/spkguide.txt"
	export LinksText="Links, Webbrowser"
# TBD GC
#	export LinksDoc="$Doc/htm/faq.htm"
	export NeText="N E, der 'Nice Editor'"
	export mcvoxDocText="m c vox documentation (Englische)"
	export mcvoxText="m c vox, Datei-Browser"
	export qalcText="qalc, Taschenrechner"
	export mocText="moc, Audio-Player für die Konsole"
	export podracerText="podracer, Podcast empfänger mit Bittorrent Unterstützung"
	export linphonecText="linphonec, ein SIP Internet Telefonprogram"
	export imcomText="imcom, Jabber-Client"
	export aventureText="Spiel ein Spiel, Abenteuerspiel"
	export aventureDoc="$Games/de/abent.z5"
	export shellText1="Zur Shell"
	export shellText2="Geben Sie 'exit' ein, um zum Menu zurückzukehren."
	export dateText="Zeit anzeigen"
	export dosemuText1="dosemu, DOS unter Linux ausführen"
	export dosemuText2="Geben sie 'exitemu' ein, um zum Menu zurückzukehren."
	;;
    pt*) 
	export YasrText="Documentação do YASR."
	export YasrDoc="$Doc/Yasr/pt_BR/README.oralux"
	export SpeakupText="Documentação do Speakup"
	export SpeakupDoc="$Doc/Speakup/pt-br/tutorial-pt-br.txt"

	export exitText="Pressione a tecla Escape para sair do menu atual."

	export LinksText="Links, Navegador WEB."
# TBD GC
#	export LinksDoc="$Doc/htm/faq.htm"
	export NeText="N E, Editor nice."
	export mcvoxText="m c vox, Navegador de Arquivos."
	export qalcText="qalc, Calculadora."
	export mocText="moc, Player de áudio em modo Console."
	export podracerText="podracer, Suporte a recepção de podcast com o BitTorrent."
	export linphonecText="linphonec, Ligações telefônicas via internet."
	export imcomText="imcom, Cliente jabber."
	export aventureText="Executa o Jogo\: Adventure"
	export aventureDoc="$Games/en/Advent.z5"
	export shellText1="Leva a um shell."
	export shellText2="Digite \'exit\' para voltar ao menu."
	export dateText="Exibe a hora."
	export dosemuText1="dosemu, Emulador DOS para Linux."
	export dosemuText2="Digite 'exitemu' para voltar ao menu."
	;; 
    es*)
	export exitText="Presione la tecla Escape para salir del menú actual."
	export YasrText="Documentación de YASR"
	export YasrDoc="$Doc/Yasr/es/README.oralux"
	export SpeakupText="Documentación de Speakup"
#	export SpeakupDoc="$Doc/Speakup/spkguide.txt"
	export LinksText="Links, un navegador web."
	export LinksDoc="$Doc/htm/faq-es.htm"
	export NeText="N E, un editor de textos."
	export mcvoxText="m c vox"
#	export mcvoxDoc="$Doc/mcvox/en/mcvox.txt"
	export qalcText="qalc, una calculadora"
	export mocText="moc, un reproductor de audio."
	export podracerText="podracer, un receptor de podcasts con soporte para BitTorrent."
	export linphonecText="linphonec, telefonía SIP en internet."
	export imcomText="imcom, un programa para acceder a jabber"
	export aventureText="Jugar\: Adventure"
	export aventureDoc="$Games/es/AVENTURA.Z5"
	export shellText1="Salir a una consola"
	export shellText2="Escriba 'exit' para regresar al menú."
	export dateText="Mostrar la hora"
	export dosemuText1="dosemu, un emulador de DOS para Linux"
	export dosemuText2="Escriba 'exitemu' para regresar al menú."	
	;; 
    fr*) 
	export exitText="Appuyez sur la touche Escape pour quitter le menu"
	export YasrText="Documentation YASR"
	export SpeakupText="Documentation Speakup (anglais)"
#	export SpeakupDoc="$Doc/Speakup/spkguide.txt"
	export YasrDoc="$Doc/Yasr/fr/README.oralux"
	export LinksText="Links, navigateur web"
	export LinksDoc="$Doc/htm/faq-fr.htm"
	export NeText="N E, éditeur de textes"
	export mcvoxDocText="Documentation m c vox"
	export mcvoxDoc="$Doc/mcvox/fr/mcvox.txt"
	export mcvoxText="m c vox, navigateur de fichiers"
	export qalcText="qalc, calculatrice"
	export mocText="moc, lecteur audio pour la console"
	export podracerText="podracer, recepteur podcast avec support BitTorrent"
	export linphonecText="linphonec, téléphone par internet S I P"
	export imcomText="imcom, client jabber"
	export aventureText="Jeu \: Aventure"
	export aventureDoc="$Games/fr/aventure.z5"
	export shellText1="Lancer un shell"
	export shellText2="Tapez 'exit' pour revenir au menu."
	export dateText="Afficher l'heure"
	export dosemuText1="dosemu, lancer DOS sous Linux"
	export dosemuText2="Tapez 'exitemu' pour revenir au menu."
	;; 
    ru*) 
	source $RussianText
	;; 
esac

if [ "$DESKTOP" = "Speakup" ]; then
    DesktopText="$SpeakupText"
    DesktopDoc="$SpeakupDoc"
else
    DesktopText="$YasrText"
    DesktopDoc="$YasrDoc"
fi




Options="-ur"
if [ -e "$HOME/pdmenurc" ]; then
    pdmenu $Options "$HOME/pdmenurc"
    exit 0
fi

pdmenu $Options - <<EOF
#!/usr/bin/pdmenu

preproc:echo menu:main:minimenu

exit:$exitText::exit
exec:$DesktopText::ne $DesktopDoc
exec:$LinksText::links $LinksDoc
exec:$NeText::ne

#exec:Help::ne $mcvoxDoc

exec:$mcvoxDocText::ne $mcvoxDoc
exec:$mcvoxText::mcvox
exec:$qalcText::qalc
exec:$mocText::mocp
exec:$podracerText::podracer
exec:$linphonecText::linphonec
exec:Micq, I C Q::micq
exec:$imcomText::imcom
exec:$aventureText::/usr/games/frotz $aventureDoc

preproc:\
	case "$LANGUAGE" in \
	fr*) echo "exec:Jeu \: Filaments::/usr/games/frotz /usr/share/oralux/install/packages/games/fr/filaments.z8"; \
	esac

exec:$shellText1::echo "$shellText2";/bin/sh -login
exec:$dateText:display:date
exec:$dosemuText1::echo "$dosemuText2";sleep 2;dosemu

EOF
