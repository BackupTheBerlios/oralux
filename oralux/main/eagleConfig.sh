#!/bin/bash

# ----------------------------------------------------------------------------
# eagleConfig.php
# $Id: eagleConfig.sh,v 1.3 2005/01/30 21:43:51 gcasse Exp $
# $Author: gcasse $
# Description: VM settings (php5)
# $Date: 2005/01/30 21:43:51 $ |
# $Revision: 1.3 $ |
# Copyright (C) 2004, 2005 Gilles Casse (gcasse@oralux.org)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# ----------------------------------------------------------------------------

#set -v

aLang=en
case $LANGUAGE in
de|es|fr|it|pl)
     aLang=$LANGUAGE;;
*)
     aLang=en;;
esac

echo $aLang

cd /usr/local/eagle-usb/sbin

./eagleconfig --lang=$aLang

# new symbolic links
# Firstly remove the previous ones:
tempfile=`tempfile`
symlinksfile="/etc/sysconfig/symlinks"
if [ -e $symlinksfile ]; then
grep -v "\#eagle-usb" $symlinksfile >> $tempfile 
fi
mv $tempfile $symlinksfile

# Then update the symlinks file
cat >> $symlinksfile <<EOF 
cd /etc/init.d;ln -s /etc/eagle-usb/scripts/eu_init eagle-usb; #eagle-usb
cd /etc/hotplug/usb;ln -s /etc/eagle-usb/scripts/eu_dsp eu_dsp; #eagle-usb
cd /etc/rc2.d;ln -s /etc/init.d/eagle-usb S99eagle-usb; #eagle-usb
cd /etc/rc3.d;ln -s /etc/init.d/eagle-usb S99eagle-usb; #eagle-usb
cd /etc/rc4.d;ln -s /etc/init.d/eagle-usb S99eagle-usb; #eagle-usb
cd /etc/rc5.d;ln -s /etc/init.d/eagle-usb S99eagle-usb; #eagle-usb
cd /etc/rc0.d;ln -s /etc/init.d/eagle-usb K01eagle-usb; #eagle-usb
cd /etc/rc1.d;ln -s /etc/init.d/eagle-usb K01eagle-usb; #eagle-usb
cd /etc/rc6.d;ln -s /etc/init.d/eagle-usb K01eagle-usb; #eagle-usb
EOF



