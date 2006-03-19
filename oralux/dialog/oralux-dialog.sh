#!/bin/sh



case $1 in
    1)
	# replace dialog by dialog-oralux
	cd /usr/bin
	if [ -e dialog-oralux ]; then
	    if [ -e dialog.orig ]; then
		rm -f dialog
	    else
		mv dialog dialog.orig
	    fi
	    ln -s dialog-oralux dialog
	fi
	;;
    0)
	# reinstall dialog
	cd /usr/bin
	if [ -e dialog.orig ]; then
	    rm -f dialog
	    ln -s dialog.orig dialog
	fi
	;;
    *)
	echo "usage: oralux-dialog.sh [01]"
	;;
esac
