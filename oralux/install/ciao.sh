#!/bin/sh
for i in `cat remove_all.txt|grep -v "#"`
do
apt-get --purge remove $i
echo $i
done
