#!/bin/bash

# ----------------------------------------------------------------------------
# eagleConfig.php
# $Id: eagleConfig.sh,v 1.1 2004/10/10 21:46:36 gcasse Exp $
# $Author: gcasse $
# Description: VM settings (php5)
# $Date: 2004/10/10 21:46:36 $ |
# $Revision: 1.1 $ |
# Copyright (C) 2004 Gilles Casse (gcasse@oralux.org)
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

set -v

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
echo "/etc/hotplug/usb/eu_dsp" >> /etc/sysconfig/symlinks
echo "/etc/rc0.d/K01eagle-usb" >> /etc/sysconfig/symlinks
echo "/etc/rc1.d/K01eagle-usb" >> /etc/sysconfig/symlinks
echo "/etc/rc2.d/S99eagle-usb" >> /etc/sysconfig/symlinks
echo "/etc/rc3.d/S99eagle-usb" >> /etc/sysconfig/symlinks
echo "/etc/rc4.d/S99eagle-usb" >> /etc/sysconfig/symlinks
echo "/etc/rc5.d/S99eagle-usb" >> /etc/sysconfig/symlinks
echo "/etc/rc6.d/K01eagle-usb" >> /etc/sysconfig/symlinks




