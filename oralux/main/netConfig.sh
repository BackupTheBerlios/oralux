#!/bin/bash
# ----------------------------------------------------------------------------
# netConfig.sh
# $Id: netConfig.sh,v 1.14 2006/02/13 20:18:48 gcasse Exp $
# $Author: gcasse $
# Description: Menu for internet settings
# $Date: 2006/02/13 20:18:48 $ |
# $Revision: 1.14 $ |
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
    fr*)
	TextCancelPressed="Opération annulée."
	TextEscPressed="Touche échappement appuyée."
	;;
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
    fr*)
	TextSettingUp="Configuration de la connexion internet"
	TextSerialModem="Modem série"
	TextEthernet="Liaison Ethernet"
	TextMail="Courriel"
	TextQuit="Quitter"
	Textpon="Pour démarrer la connection, vous taperez dans un shell la commande pon, qui s'écrit P O N."
	Textpoff="La commande poff, P O F F, arrête la connexion."
	;;
    *)
	TextSettingUp="Setting up the internet connection"
	TextSerialModem="Serial Modem"
	TextEthernet="Ethernet connexion"
	TextMail="Mail"
	TextQuit="Quit"
	Textpon="You will want to type in a shell the pon command, P O N, to start the connection"
	Textpoff="The poff command, P O F F, stops the connection"
	;;
esac


$DIALOG --nobutton --menu "$TextSettingUp" 20 51 4 \
    "1" "$TextSerialModem" \
    "2" "$TextEthernet" \
    "3" "$TextMail" \
    "4" "$TextQuit" \
    2> $tempfile

#"IM" => "ICQ"

retval=$?

choice=`cat $tempfile`

case $retval in
    0)
	case $choice in
	    1)
		pppconfig --dialog
		echo $Textpon
		echo $Textpoff
		;;
	    2)
		netcardconfig
		;;
	    3)
		php mailMenu.php
		# TBD
		chown knoppix.knoppix /home/knoppix/oraluxmail.xml 2>/dev/null
		dir=/home/knoppix/mail
		if [ ! -d $dir ]; then
		    mkdir $dir 2>/dev/null
		    chmod 700 $dir
		fi
		;;
#	4);;
    esac
	;;
    1)
	echo "$TextCancelPressed";;
    255)
	echo "$TextEscPressed";;
esac


