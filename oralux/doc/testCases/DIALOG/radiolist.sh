#! /bin/sh
DIALOG=${DIALOG=dialog}
tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/test$$
trap "rm -f $tempfile" 0 1 2 5 15

$DIALOG --backtitle "A backtitle" --title "A title" \
    --radiolist "A text" 20 51 4 \
    "CHOICE1"  "First choice" off\
    "CHOICE2" "Second choice" on\
    "CHOICE3" "Third choice" off\
    "CHOICE4" "Fourth choice" off\
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

