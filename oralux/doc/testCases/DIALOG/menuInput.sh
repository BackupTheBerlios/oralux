#! /bin/sh
DIALOG=${DIALOG=dialog}
tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
trap "rm -f $tempfile" 0 1 2 5 15

$DIALOG --clear --title "Title: testing menu" \
    --menu "Menu TEXT" 20 51 4 \
    "CHOICE1"  "First choice" \
    "CHOICE2" "Second choice" \
    "CHOICE3" "Third choice" \
    2> $tempfile

retval=$?

choice=`cat $tempfile`

case $retval in
    0)
    echo "'$choice' is selected";;
    1)
    echo "Cancel pressed.";;
    255)
    echo "ESC pressed.";;
esac

$DIALOG --title "Title inputBox" --clear \
        --inputbox "Enter data:" 16 51 "default text" 2> $tempfile

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

