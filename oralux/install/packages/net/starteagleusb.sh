#!/bin/bash
source /etc/sysconfig/knoppix 
export LANG=$LANGUAGE

for i in `ls /etc/eagle-usb/lang`
do
  if [ "a$LANG" == "a$i" ]; then
      found=1
  fi
done

if [ -z "$found" ]; then
    export LANG=en
fi

echo LANG=$LANG

cd /usr/local/eagle-usb/sbin
./eagleconfig

for i in `ls /etc/rc?.d/[SK]??eagle-usb`
do
  cp -pL $i $i.$$
  rm -rf $i 
  mv $i.$$ $i
done
