#!/bin/bash
# ----------------------------------------------------------------------------
# netConfig.sh
# $Id: netConfig.sh,v 1.18 2006/04/17 09:11:43 gcasse Exp $
# $Author: gcasse $
# Description: Menu for internet settings
# $Date: 2006/04/17 09:11:43 $ |
# $Revision: 1.18 $ |
# Copyright (C) 2004, 2005 Gilles Casse (gcasse@oralux.org)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# ----------------------------------------------------------------------------

#set -vx

#export DIALOG=dialog-oralux
export DIALOG=/usr/bin/dialog

tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
trap "rm -f $tempfile" 0 1 2 5 15

case "$LANGUAGE" in
    de*|at*|ch*)
    TextCancelPressed="Abbrechen gedrückt."
    TextEscPressed="Escape gedrückt."
    ;;

    es*)
    TextCancelPressed="Cancelar presionado"
    TextEscPressed="ESC presionado"
    ;;

    fr*)
    TextCancelPressed="Opération annulée."
    TextEscPressed="Touche échappement appuyée."
    ;;

    pt*)
    TextCancelPressed="Cancelar Pressionado."
    TextEscPressed="ESC Pressionado."
    ;;

#TBD GC
#ru*)

    *)
    TextCancelPressed="Cancel pressed."
    TextEscPressed="ESC pressed."
    ;;
esac


adslMenu()
{
    case "$LANGUAGE" in
	fr*)
	    TextWhichUsbADSL="Quel est votre modem U S B,  A D S L?"
	    ;;
	*)
	    TextWhichUsbADSL="Which is your U S B, A D S L Modem?"
	    ;;
esac

$DIALOG --nobutton --menu "$TextWhichUsbADSL" 20 51 4 \
    "1" "Sagem"\
    "2" "E C I"\
    2> $tempfile

#	"3" "SpeedTouch"\

retval=$?

choice=`cat $tempfile`

case $retval in
    0)
    case $choice in
	1)
	./eagleConfig.sh
	;;
	2)
	eciadsl-config-text
	;;
    esac
    ;;
    1)
    echo "$TextCancelPressed"
    ;;
    255)
    echo "$TextEscPressed"
    ;;
esac
}

case "$LANGUAGE" in
de*|at*|ch*)
    TextPing="Ihre Internetverbindung testen."
    TextPingOk="Ihre Internetverbindung funktioniert."
    TextPingKo="Keine Antwort von google.com"
    TextSettingUp="Ihre Internetverbindung einrichten"
    TextSerialModem="Serielles Modem"
    TextEthernet="Ethernet connexion Internetverbindung"
    TextMail="Mail"
    TextQuit="Beenden"
    Textpon="Geben Sie in einer Shell den pon Befehl, P O N, ein, um die Verbindung zu starten."
    Textpoff="Der poff Befehl, P O F F, unterbricht die Verbindung"
    ;;

    es*)
    TextPing="Pruebe su conexión a Internet"
    TextPingOk="La conexión a Internet funciona correctamente"
    TextPingKo="google.com no responde."
    TextSettingUp="Configurando la conexión a internet"
    TextSerialModem="Modem serial"
    TextEthernet="Conexión Ethernet"
    TextMail="Correo Electrónico"
    TextQuit="Salir"
    Textpon="El comando pon inicia la conexión. Escriba el comando P O N en
la consola."
    Textpoff="El comando poff detiene la conexion. P O F F."
    ;;

    fr*)
    TextPing="Test de la connexion internet"
    TextPingOk="La connexion internet fonctionne"
    TextPingKo="Pas de réponse de google.com"
    TextSettingUp="Configuration de la connexion internet"
    TextSerialModem="Modem série"
    TextEthernet="Liaison Ethernet"
    TextMail="Courriel"
    TextQuit="Quitter"
    Textpon="Pour démarrer la connection, vous taperez dans un shell la commande pon, qui s'écrit P O N."
    Textpoff="La commande poff, P O F F, arrête la connexion."
    ;;
    pt*)
    TextPing="Testando sua conexão com a internet."
    TextPingOk="A sua conexão com a internet está funcionando."
    TextPingKo="Nenhuma resposta de google.com"
    TextSettingUp="Configurando a conexão com a internet."
    TextSerialModem="Serial Modem"
    TextEthernet="Conexão através da placa de rede."
    TextMail="E-Mail"
    TextQuit="Sair"
    Textpon="Você pode digitar em um shell o comando pon, P O N, Para iniciar uma conexão."
    Textpoff="O comando poff, P O F F, finaliza a conexão."
    ;;
    *)
    TextPing="Testing your internet connection"
    TextPingOk="The internet connexion is working"
    TextPingKo="No answer from google.com"
    TextSettingUp="Setting up the internet connection"
    TextSerialModem="Serial Modem"
    TextEthernet="Ethernet connexion"
    TextMail="Mail"
    TextQuit="Quit"
    Textpon="You may want to type in a shell the pon command, P O N, to start the connection"
    Textpoff="The poff command, P O F F, stops the connection"
    ;;
esac

while [ -n "1" ]; do
    $DIALOG --nobutton --menu "$TextSettingUp" 20 51 4 \
	"1" "$TextPing" \
	"2" "$TextSerialModem" \
	"3" "$TextEthernet" \
	"4" "$TextMail" \
	"5" "$TextQuit" \
	2> $tempfile
    
#"IM" => "ICQ"
    
    retval=$?

    choice=`cat $tempfile`
    
    case $retval in
	0)
	case $choice in
	    1)
	    echo "ping google.com"
	    ping -q -c 1 google.com 1>/dev/null 2>&1
	    if [ "$?" == "0" ]; then
		echo $TextPingOk
	    else
		echo $TextPingKo
	    fi    
	    ;;
	    2)
	    pppconfig --dialog
	    echo $Textpon
	    echo $Textpoff
	    ;;
	    3)
	    netcardconfig
	    ;;
	    4)
	    php mailMenu.php
		# TBD
	    chown knoppix.knoppix /home/knoppix/oraluxmail.xml 2>/dev/null
	    dir=/home/knoppix/mail
	    if [ ! -d $dir ]; then
		install -d -m 700 -o knoppix -g knoppix $dir 2>/dev/null
	    fi
	    ;;
	    5)
	    exit
	    ;;
#	4);;
	esac
	;;
	1)
	echo "$TextCancelPressed";;
	255)
	echo "$TextEscPressed";;
    esac
done

