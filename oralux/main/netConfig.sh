#!/bin/bash
# raf: utf8

# ----------------------------------------------------------------------------
# netConfig.sh
# $Id: netConfig.sh,v 1.6 2004/12/03 22:59:28 gcasse Exp $
# $Author: gcasse $
# Description: Menu for internet settings
# $Date: 2004/12/03 22:59:28 $ |
# $Revision: 1.6 $ |
# Copyright (C) 2004 Gilles Casse (gcasse@oralux.org)
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

DIALOG=dialog-oralux
MINIMENU=/tmp/minimenu.tmp
tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
trap "rm -f $tempfile" 0 1 2 5 15

TextCancelPressed="Cancel pressed."
TextEscPressed="ESC pressed."

rm -f $MINIMENU 2>/dev/null

adslMenu()
{
    TextWhichUsbADSL="Which is your USB ADSL Modem?"
    $DIALOG --nobutton --menu "$TextWhichUsbADSL" 20 51 4 \
	"0" "Sagem"\
	"1" "SpeedTouch"\
	"2" "E C I"\
    2> $tempfile
    
    retval=$?
    
    choice=`cat $tempfile`
    
    case $retval in
	0)
case $choice in
    0)
    ./eagleConfig.sh;;
    1)
    ;;
    2)
    ;;
esac
;;

1)
echo "$TextCancelPressed";;
255)
echo "$TextEscPressed";;
esac
}

TextSettingUp="Setting up the internet connection"
TextSerialModem="Serial Modem"
TextAdslUsb="Adsl USB Modem"
TextMail=Mail
TextQuit=Quit

$DIALOG --nobutton --menu "$TextSettingUp" 20 51 4 \
    "0" "$TextSerialModem" \
    "1" "$TextAdslUsb" \
    "2" "$TextMail" \
    "3" "$TextQuit" \
    2> $tempfile

#"IM" => "ICQ"

retval=$?

choice=`cat $tempfile`

case $retval in
    0)
    case $choice in
	0)
	pppconfig --dialog-oralux;;
	1)
	adslMenu;;
	2)
	php mailMenu.php;;
	3)
	touch $MINIMENU;;
    esac
    ;;
    1)
    echo "$TextCancelPressed";;
    255)
    echo "$TextEscPressed";;
esac


