#!/bin/bash
DIALOG=${DIALOG=dialog}

tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
trap "rm -f $tempfile" 0 1 2 5 15

$DIALOG --backtitle "A backtitle" --title "A title" --extra-button --cancel-label Previous --ok-label Next --extra-label \
    "Quit" --help-button --help-label "Help" --passwordbox "Input your password" 20 70 2>$tempfile

a=$?
cat $tempfile
echo
case $a in
  0)
    echo "OK";;
  1)
    echo "Cancel";;
  2)
    echo "Help";;
  3)
    echo "Quit";;
  255)
    echo "ESC pressed.";;
esac

