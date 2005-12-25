#! /bin/sh
# ----------------------------------------------------------------------------
# emacspeak.sh
# $Id: emacspeak-ss.sh,v 1.4 2005/12/25 21:02:35 gcasse Exp $
# $Author: gcasse $
# Description: Installing emacspeak. Thanks to the Nath's howto: 
# emacspeak-dtk-soft-debinst-howto.htm
# $Date: 2005/12/25 21:02:35 $ |
# $Revision: 1.4 $ |
# Copyright (C) 2003, 2004, 2005 Gilles Casse (gcasse@oralux.org)
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
####
source ../oralux.conf
EMACSPEAK_SS=emacspeak-ss-1.9.1.tar.gz
ARCH_EMACSPEAK_SS=$ARCH/$EMACSPEAK_SS
LISTE="accent apollo braillenspeak ciber doubletalk pchablado"

####
# Installing the package in the current tree
InstallPackage()
{
    cd /tmp
    rm -rf emacspeak*
    tar -zxvf $ARCH_EMACSPEAK_SS
    cd /tmp/emacspeak*; ./configure; make; make install

    cd /usr/share/emacs/site-lisp/emacspeak/servers

    for i in  $LISTE
      do 
      a=`head -1 $i`
      if [ '#!/usr/bin/tcl' != "$a" ]; then
	  echo "$i: Adding tcl command line"
	  echo '#!/usr/bin/tcl' > fic.tmp
	  cat $i >> fic.tmp
	  mv fic.tmp $i
      fi
      chmod a+x $i   
    done

    # Clearing /tmp
    cd /tmp
    rm -rf emacspeak*
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    cd $BUILD/tmp
    rm -rf emacspeak*
    tar -zxvf $ARCH_EMACSPEAK_SS
    chroot $BUILD bash -c "cd /tmp/emacspeak*; ./configure; make; make install"

    cd $BUILD/usr/share/emacs/site-lisp/emacspeak/servers

    for i in  $LISTE
      do 
      a=`head -1 $i`
      if [ '#!/usr/bin/tcl' != "$a" ]; then
	  echo "$i: Adding tcl command line"
	  echo '#!/usr/bin/tcl' > fic.tmp
	  cat $i >> fic.tmp
	  mv fic.tmp $i
      fi
      chmod a+x $i   
    done

    # Clearing /tmp
    cd $BUILD/tmp
    rm -rf emacspeak*
}

if [ ! -e $ARCH_EMACSPEAK_SS ]
    then
    cd $ARCH
    echo "Downloading $EMACSPEAK_SS"
    wget -nd http://fr.rpmfind.net/linux/blinux/emacspeak/blinux/$EMACSPEAK_SS
fi

case $1 in
    i|I)
        InstallPackage
        ;;
    b|B)
        Copy2Oralux
        ;;
    *)
        echo "I (install) or B(new tree) is expected"
        ;;
esac

exit 0
