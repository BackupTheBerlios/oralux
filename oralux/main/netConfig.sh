#!/bin/bash
# raf: utf8

# ----------------------------------------------------------------------------
# netConfig.sh
# $Id: netConfig.sh,v 1.9 2005/04/03 00:36:28 gcasse Exp $
# $Author: gcasse $
# Description: Menu for internet settings
# $Date: 2005/04/03 00:36:28 $ |
# $Revision: 1.9 $ |
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

export DIALOG=dialog-oralux
export MINIMENU=/tmp/minimenu.tmp
tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
trap "rm -f $tempfile" 0 1 2 5 15


case "$LANGUAGE" in
fr)
TextCancelPressed="Opération annulée."
TextEscPressed="Touche échappement appuyée."
;;
*)
TextCancelPressed="Cancel pressed."
TextEscPressed="ESC pressed."
;;
esac


rm -f $MINIMENU 2>/dev/null

adslMenu()
{
case "$LANGUAGE" in
fr)
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
    ./eagleConfig.sh;;
    2)
    eciadsl-config-text;;
esac
;;

1)
echo "$TextCancelPressed";;
255)
echo "$TextEscPressed";;
esac
}

case "$LANGUAGE" in
fr)
TextSettingUp="Configuration de la connexion internet"
TextSerialModem="Modem série"
TextAdslUsb="Modem U S B, A D S L"
TextMail="Courriel"
TextQuit="Quitter"
;;
*)
TextSettingUp="Setting up the internet connection"
TextSerialModem="Serial Modem"
TextAdslUsb="Adsl USB Modem"
TextMail="Mail"
TextQuit="Quit"
;;
esac


$DIALOG --nobutton --menu "$TextSettingUp" 20 51 4 \
    "1" "$TextSerialModem" \
    "2" "$TextAdslUsb" \
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
	pppconfig --dialog;;
	2)
	adslMenu;;
	3)
	php mailMenu.php;;
	4)
	touch $MINIMENU;;
    esac
    ;;
    1)
    echo "$TextCancelPressed";;
    255)
    echo "$TextEscPressed";;
esac


