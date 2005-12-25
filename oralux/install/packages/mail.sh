#! /bin/sh
# ----------------------------------------------------------------------------
# mail.sh
# $Id: mail.sh,v 1.4 2005/12/25 21:02:35 gcasse Exp $
# $Author: gcasse $
# Description: Mail
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

set -vx

export LIST="exim4 gnus mutt vm mime-codecs nmh \
mh-e gnus fetchmail procmail bbdb mew"

#exim

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

    apt-get install $LIST

    # TODO: update vm 
    cd /tmp
    cp $INSTALL_PACKAGES/mail/qp-decode.c .
    gcc -o qp-decode qp-decode.c
    install -m 755 qp-decode /usr/bin

    # S99fetchmail
    update-rc.d -f fetchmail remove

    # S20exim4
    update-rc.d -f exim4 remove

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

    chroot $BUILD apt-get install $LIST

    cd /tmp
    cp $INSTALL_PACKAGES/mail/qp-decode.c .
    gcc -o qp-decode qp-decode.c
    install -m 755 qp-decode $BUILD/usr/bin

    chroot $BUILD bash -c 'update-rc.d -f fetchmail remove;update-rc.d -f exim4 remove'

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

