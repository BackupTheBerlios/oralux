#!/bin/sh
DIALOG=${DIALOG=dialog}

COUNT=10
(
    while test $COUNT != 110
      do
      echo $COUNT
      echo "XXX"
      echo "The new message ($COUNT percent)"
      echo "XXX"
      COUNT=`expr $COUNT + 10`
      sleep 1
    done
) | $DIALOG --title "Gauge title" --gauge "Hi, this is a gauge widget" 20 70 0



