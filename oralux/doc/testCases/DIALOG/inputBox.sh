#!/bin/sh

DIALOG=${DIALOG=dialog}
DefaultValue=$1

tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
trap "rm -f $tempfile" 0 1 2 5 15
echo "tempfile=$tempfile"

#$DIALOG --nobutton --title "Title inputBox" --clear \

$DIALOG --title "Title inputBox" --clear \
        --inputbox "Enter data:" 16 51 "$DefaultValue" 2> $tempfile

retval=$?

case $retval in
    0)
    echo "Data: `cat $tempfile`";;
    1)
    echo "Cancel pressed.";;
    255)
    if test -s $tempfile ; then
	cat $tempfile
    else
	echo "ESC pressed."
    fi
    ;;
esac

