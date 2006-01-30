#! /bin/sh
# ----------------------------------------------------------------------------
# oralux.sh
# $Id: oralux.sh,v 1.14 2006/01/30 22:49:38 gcasse Exp $
# $Author: gcasse $
# Description: This script is called at init time
# $Date: 2006/01/30 22:49:38 $ |
# $Revision: 1.14 $ |
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

FILE=/tmp/oralux.tmp
TTY=$(tty|sed 's+/UNIONFS++')

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
	# Once enhanced, it could replace dialog
	export DIALOG=/usr/bin/dialog-oralux

	cd /usr/share/oralux/main
	sudo ./oralux $TTY start

	# Restoring the initial dialog
	export DIALOG=/usr/bin/dialog

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
	
	if [ -z $MULTISPEECH_LANG_SWITCH ] && [ "$LANGUAGE" = "ru" ]; then
	    export MULTISPEECH_LANG_SWITCH=1
	fi

# Starting the audio desktop
	cd /home/knoppix
	if [ $DESKTOP == "Emacspeak" ]
	    then
	    emacspeak
	elif [ $DESKTOP == "Shell" ]
	    then
echo "lancement zsh"
	    zsh
	else	    
# TBD
	    sudo chown knoppix:knoppix /home/knoppix/.yasr.conf
	    oralux-yasr.sh

	    # just for safe
	    pkill test-tengoo
	    reset
#/usr/bin/screen -c /usr/share/oralux/install/packages/screen/.screenrc.work
	fi
	echo -e '\007'	
	cd /usr/share/oralux/main
	sudo ./oralux $TTY stop
    done
fi

