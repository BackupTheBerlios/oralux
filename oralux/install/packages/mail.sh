#! /bin/sh
# ----------------------------------------------------------------------------
# mail.sh
# $Id: mail.sh,v 1.1 2004/09/27 20:29:59 gcasse Exp $
# $Author: gcasse $
# Description: Mail
# $Date: 2004/09/27 20:29:59 $ |
# $Revision: 1.1 $ |
# Copyright (C) 2003, 2004 Gilles Casse (gcasse@oralux.org)
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

####
# Installing the package in the current tree
InstallPackage()
{
#     echo "Select 1 (No conf)"
#     apt-get install --purge postfix
#     cp $INSTALL_PACKAGES/mail/main.cf /etc/postfix

#     mkdir /home/knoppix/mail
#     cp $INSTALL_PACKAGES/mail/.procmailrc /home/knoppix
#     cp $INSTALL_PACKAGES/mail/.forward /home/knoppix
#     cp $INSTALL_PACKAGES/mail/.procmailrc /home/knoppix
#     cp $INSTALL_PACKAGES/mail/.vm /home/knoppix

##    dpkg-reconfigure postfix

    apt-get install gnus
    apt-get install mutt
    apt-get install vm
    apt-get install mime-codecs
    apt-get install nmh
    apt-get install mh-e
    apt-get install gnus
    apt-get install fetchmail
    apt-get install procmail
    apt-get install bbdb

#     cd /tmp
#     rm -rf vm-pcrisis*
#     wget http://www.robf.de/Hacking/elisp/vm-pcrisis-0.85b.tgz
#     cd vm-pcrisis*
#     emacs -batch -f batch-byte-compile vm-pcrisis.el 2>/dev/null
#     cp vm-pcrisis.el* ~/.emacs.d
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
#    chroot $BUILD apt-get install --purge postfix

    chroot $BUILD apt-get install gnus
    chroot $BUILD apt-get install mutt
    chroot $BUILD apt-get install vm
    chroot $BUILD apt-get install mime-codecs
    chroot $BUILD apt-get install nmh
    chroot $BUILD apt-get install mh-e
    chroot $BUILD apt-get install gnus
    chroot $BUILD apt-get install fetchmail
    chroot $BUILD apt-get install procmail
    chroot $BUILD apt-get install bbdb

#     cd /tmp
#     rm -rf vm-pcrisis*
#     wget http://www.robf.de/Hacking/elisp/vm-pcrisis-0.85b.tgz
#     cd vm-pcrisis*
#     emacs -batch -f batch-byte-compile vm-pcrisis.el 2>/dev/null
#     cp vm-pcrisis.el* ~/.emacs.d

}

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

