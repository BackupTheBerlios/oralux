#!/bin/sh

export Doc="/usr/share/oralux/doc"
export Games="/usr/share/oralux/install/packages/games"

export RussianText="/usr/share/oralux/install/packages/menu/oralux-pdmenu.ru"

export exitText="Press the Escape key to exit the current menu"
export YasrText="YASR documentation "
export YasrDoc="$Doc/Yasr/en/README.oralux"
export LinksText="Links, web browser"
export LinksDoc="$Doc/htm/faq.htm"
export NeText="N E, the nice editor"
export mcvoxText="m c vox, file browser"
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
	export YasrText="YASR Dokumentation (Englische)"
	export YasrDoc="$Doc/Yasr/en/README.oralux"
	export LinksText="Links, Webbrowser"
#	export LinksDoc="$Doc/htm/faq.htm"
	export NeText="N E, der 'Nice Editor'"
	export mcvoxText="m c vox, Datei-Browser"
	export qalcText="qalc, Taschenrechner"
	export mocText="moc, Audio-Player für die Konsole"
	export podracerText="podracer, Podcast empfänger mit Bittorrent Unterstützung"
	export linphonecText="linphonec, ein SIP Internet Telefonprogram"
	export imcomText="imcom, Jabber-Client"
	export aventureText="Spiel ein Spiel, Abenteuerspiel"
	export $aventureDoc="$Games/de/abent.z5"
	export shellText1="Zur Shell"
	export shellText2="Geben Sie 'exit' ein, um zum Menu zurückzukehren."
	export dateText="Zeit anzeigen"
	export dosemuText1="dosemu, DOS unter Linux ausführen"
	export dosemuText2="Geben sie 'exitemu' ein, um zum Menu zurückzukehren."
	;;
    pt*) 
	export YasrText="YASR documentation"
	export YasrDoc="$Doc/Yasr/pt_BR/README.oralux"
	;; 
    es*) 
	export YasrText="YASR documentation"
	export YasrDoc="$Doc/Yasr/es/README.oralux"
	export LinksText="Links, web browser"
	export LinksDoc="$Doc/htm/faq-es.htm"
	export aventureDoc="$Games/es/AVENTURA.Z5"
	;; 
    fr*) 
	export exitText="Appuyez sur la touche Escape pour quitter le menu"
	export YasrText="Documentation YASR"
	export YasrDoc="$Doc/Yasr/fr/README.oralux"
	export LinksText="Links, navigateur web"
	export LinksDoc="$Doc/htm/faq-fr.htm"
	export NeText="N E, éditeur de textes"
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

Options="-ur"
if [ -e "$HOME/pdmenurc" ]; then
    pdmenu $Options "$HOME/pdmenurc"
    exit 0
fi

pdmenu $Options - <<EOF
#!/usr/bin/pdmenu

preproc:echo menu:main:minimenu

exit:$exitText::exit
exec:$YasrText::ne $YasrDoc
exec:$LinksText::links /usr/share/oralux/doc/htm/LinksDoc
exec:$NeText::ne

#exec:Help::ne /usr/share/oralux/doc/mcvox-$LANG.txt

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
