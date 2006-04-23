#!/bin/bash
# ----------------------------------------------------------------------------
# persistentStorage.sh
# $Id: persistentStorage.sh,v 1.2 2006/04/23 23:12:03 gcasse Exp $
# $Author: gcasse $
# Description: Menu for the persistent storages
# $Date: 2006/04/23 23:12:03 $ |
# $Revision: 1.2 $ |
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


case "$LANGUAGE" in
    fr*)
	TextSaveconfig="Voulez-vous juste sauvegarder quelques fichiers : configuration, etc..."
	TextImage="Voulez-vous stocker une image de votre système (3 Méga octets de libre, au minimum)."
	TextQuit="Quitter"
	;;
    *)
	TextSaveconfig="Do you want to save only a few files: configuration, etc..."
	TextImage="Do you want to save an image of your system (at least 3 Mega Bytes free)."
	TextQuit="Quit"
	;;
esac


$DIALOG --nobutton --menu "$TextSettingUp" 20 51 4 \
    "1" "$TextSaveconfig" \
    "2" "$TextImage" \
    "3" "$TextQuit" \
    2> $tempfile

retval=$?

choice=`cat $tempfile`

case $retval in
    0)
	case $choice in
	    1)
		saveconfig
		;;
	    2)
		knoppix-mkimage
		;;
#	4);;
    esac
	;;
    1)
	echo "$TextCancelPressed";;
    255)
	echo "$TextEscPressed";;
esac

