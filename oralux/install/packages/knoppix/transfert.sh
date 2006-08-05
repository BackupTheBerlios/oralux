#!/bin/sh

#SRC=/mnt/sda1

#cd initRAMFS/lib/modules/2.6.17

for i in `find . -type f -name "*.ko"`; do
#    cp "/mnt/sda1/lib/modules/2.6.17.nosmp/$i" $i
    j=$(find /lib/modules/2.6.17.nosmp -name $(basename $i))

    cp $j $i
done