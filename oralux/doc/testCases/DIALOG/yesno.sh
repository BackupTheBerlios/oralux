#!/bin/bash
DIALOG=${DIALOG=dialog}

$DIALOG --title "A title" --clear \
        --yesno "A text" 10 30

case $? in
  0)
    echo "Yes";;
  1)
    echo "No";;
  255)
    echo "ESC";;
esac


