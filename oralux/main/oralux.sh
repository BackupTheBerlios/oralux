#! /bin/sh
# ----------------------------------------------------------------------------
# oralux.sh
# $Id: oralux.sh,v 1.6 2005/01/30 21:43:51 gcasse Exp $
# $Author: gcasse $
# Description: This script is called at init time
# $Date: 2005/01/30 21:43:51 $ |
# $Revision: 1.6 $ |
# Copyright (C) 2003, 2004, 2005 Gilles Casse (gcasse@oralux.org)
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
####

TTY=`tty`
FILE=/tmp/oralux.tmp
MINIMENU=/tmp/minimenu.tmp

callYasr()
{
    rm -f $MINIMENU
#    killall -9 minimenu 2>/dev/null

    yasr -p ./minimenu

# #    sleep 1

# # TBD
# #    stty icanon icrnl isig echo iexten opost 
# #    clear
# #    killall -9 minimenu 2>/dev/null

    result=0
    [ -e $MINIMENU ] && result=1
    return "$result"
}

getStickyString()
{
    echo "s/=[ ]*"$1"[ \t]*$/= S"$1"/g"
}

if [ "$TTY" == "/dev/tty1" -a ! -e "$FILE" ]
    then
    
    touch $FILE
    
# The user is asked about her required environment (TTS language, which TTS,...)
    
    state=START
    
    while  [ "$state" != "STOP" ]; do
	
	# dialog-oralux is only used for the menu.
	# Once more enhanced, it could replace dialog
	DIALOG_DIR=/usr/bin
	cd $DIALOG_DIR
	if [ ! -h dialog ]; then
	    mv dialog dialog.original
	    ln -s dialog-oralux dialog
	fi

	cd /usr/share/oralux/main
	sudo ./oralux `tty` start

	if [ -h $DIALOG_DIR/dialog ]; then
	    rm $DIALOG_DIR/dialog
	    mv $DIALOG_DIR/dialog.original $DIALOG_DIR/dialog
	fi
    
# Taking in account the (perhaps updated) config
	source /etc/sysconfig/knoppix
	export LANG
	export COUNTRY
	export LANGUAGE
	export CHARSET
	export KEYTABLE
	export XKEYBOARD
	export DESKTOP
	export ORALUXTTSLANG
	export ORALUXRELEASE
	export ORALUXSTICKYKEYS
	export ORALUXREPEATKEYS
	export EMACSPEAKTTS
	export EMACSPEAKTTSPORT

	loadkeys "$KEYTABLE"
	if [ $ORALUXSTICKYKEYS == "1" ]
	    then
	    dumpkeys | sed -e "$(getStickyString Control)"\
		-e "$(getStickyString Shift)"\
		-e "$(getStickyString Alt)"\
		-e "$(getStickyString AltGr)" | loadkeys
	fi

	if [ $ORALUXREPEATKEYS == "1" ]
	    then
	    setterm -repeat on
	else
	    setterm -repeat off
	fi
	
# Starting the audio desktop
	cd /home/knoppix
	if [ $EMACSPEAKTTS == "EFM" ]
	    then
	    efm
	elif [ $DESKTOP == "Emacspeak" ]
	    then
	    emacspeak
	elif [ $DESKTOP == "Shell" ]
	    then
echo "lancement zsh"
	    zsh
	else	    
# TBD
	    sudo chown knoppix:knoppix /home/knoppix/.yasr.conf

	    echo 1 
	    callYasr
	    # If crash: start again
	    if [ "$?" = "0" ]; then
		echo 2
		callYasr
		if [ "$?" = "0" ]; then
		    echo 3
		    callYasr
		fi
	    fi
	fi
	echo -e '\007'	
	cd /usr/share/oralux/main
	sudo ./oralux `tty` stop
    done
fi

