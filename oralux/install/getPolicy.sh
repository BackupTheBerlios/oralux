#!/bin/bash

for i in `dpkg-query -W --showformat='${Package}\n'`
do 
  echo -n "$i   "
  apt-cache policy $i | egrep -E "[ ]+[0-9]{3}[ ]+.*" | sort | tail -1 | sed 's/\([ ]*[0-9][0-9][0-9][ ]*\)\(.*\)/\2/'
done