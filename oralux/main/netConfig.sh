#!/bin/bash
# raf: utf8

# ----------------------------------------------------------------------------
# netConfig.sh
# $Id: netConfig.sh,v 1.2 2004/11/07 21:19:14 gcasse Exp $
# $Author: gcasse $
# Description: Menu for internet settings
# $Date: 2004/11/07 21:19:14 $ |
# $Revision: 1.2 $ |
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

DIALOG=${DIALOG=dialog}
tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
trap "rm -f $tempfile" 0 1 2 5 15

TextCancelPressed="Cancel pressed."
TextEscPressed="ESC pressed."

adslMenu()
{
    TextWhichUsbADSL="Which is your USB ADSL Modem?"
    $DIALOG --nobutton --menu "$TextWhichUsbADSL" 20 51 4 --nobutton \
	"1" "Sagem"\
	"2" "SpeedTouch"\
	"3" "E C I"\
    2> $tempfile
    
    retval=$?
    
    choice=`cat $tempfile`
    
    case $retval in
	0)
case $choice in
    1)
    ./eagleConfig.sh;;
    2)
    ;;
    3)
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

$DIALOG --nobutton --menu "$TextSettingUp" 20 51 4 \
    "1" "$TextSerialModem" \
    "2" "$TextAdslUsb" \
    "3" "$TextMail" \
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
	./mailMenu.php;;
    esac
    ;;
    1)
    echo "$TextCancelPressed";;
    255)
    echo "$TextEscPressed";;
esac


