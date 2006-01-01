#!/bin/sh
for x in `dpkg -S $(grep open /tmp/log|perl -pe 's!.* open\(\"([^\"]*).*!$1!' |grep "^/"| sort | uniq|grep -v "^\(/tmp\|/dev\|/proc\)" ) 2>/dev/null|cut -f1 -d":"| sort | uniq`;
do
echo -n "$x (>=" `dpkg -s $x|grep ^Version|cut -f2 -d":"` "), "; 
done
