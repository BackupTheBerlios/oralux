#! /bin/sh
DIALOG=${DIALOG=dialog}
tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
trap "rm -f $tempfile" 0 1 2 5 15

$DIALOG --backtitle "BackTitle: testing checklist" \
    --title "Title: testing checklist" \
    --checklist "Menu TEXT" 20 51 4 \
    "CHOICE1"  "First choice" off \
    "CHOICE2" "Second choice" ON \
    "CHOICE3" "Third choice" on \
    "CHOICE4" "Fourth choice" OFF \
    2> $tempfile

retval=$?

choice=`cat $tempfile`

echo $retval

case $retval in
    0)
    echo "'$choice' selected";;
    1)
    echo "Cancel pressed.";;
    255)
    echo "ESC pressed.";;
esac

