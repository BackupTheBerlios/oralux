#!/bin/bash

for i in `dpkg-query -W --showformat='${Package}\n'`
do 
  echo -n "$i   "
  apt-cache show $i | grep "Filename:" | sort 
done