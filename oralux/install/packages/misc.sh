#! /bin/sh
# ----------------------------------------------------------------------------
# misc.sh
# $Id: misc.sh,v 1.8 2005/01/30 21:43:51 gcasse Exp $
# $Author: gcasse $
# Description: Miscellaneous packages
# $Date: 2005/01/30 21:43:51 $ |
# $Revision: 1.8 $ |
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

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get install aumix
#RAF    apt-get install sndconfig
    apt-get install brltty
    rm -f /etc/brltty.conf
    apt-get install elinks
    apt-get install links-ssl
    apt-get install lynx-ssl

    apt-get install python
    apt-get install gobjc

    apt-get install vim
    apt-get install w3-el-e21
    apt-get install w3m-el
    apt-get install zsh

    apt-get install workbone
    apt-get install cdtool; cd /usr/bin; ln -s ./cdtool cdstart
    apt-get install tnt
    apt-get install erc
    apt-get install xsltproc
    apt-get install aspell-fr
    apt-get install aspell-de
    apt-get install aspell-es
    apt-get install aspell-en
    
    cd /usr/bin; ln -s aspell ispell
    
#RUF    apt-get install console-cyrillic

cd /tmp; rm -rf aspell-ru-*; wget ftp://ftp.gnu.org/gnu/aspell/dict/ru/aspell-ru-0.50-2.tar.bz2; tar -jxvf aspell-ru-*; cd aspell-ru-*; ./configure; make; make install; cd /tmp; rm -rf  aspell-ru-*

    apt-get install ecasound
    apt-get install vsound
    apt-get install ne
    apt-get install nano

    apt-get install nethack
    apt-get install nethack-console
    apt-get install angband
    apt-get install angband-doc
    apt-get install bsdgames

    apt-get install xpdf-utils
    apt-get install catdoc
    apt-get install pdftohtml
    apt-get install wv
    apt-get install xlhtml
    apt-get install ppthtml

    apt-get install zinf
    apt-get install zinf-extras
    apt-get install zinf-plugin-alsa
    apt-get install zinf-plugin-esound
    apt-get install toolame    

    # Lirc
    # Answers during the installation
    # Create nodes: yes
    # Select device: Cancel
    apt-get install lirc

  #    apt-get install sawfish
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    chroot $BUILD apt-get install aumix
    chroot $BUILD apt-get install sndconfig
    chroot $BUILD bash -c "apt-get install brltty; rm -f /etc/brltty.conf"
    chroot $BUILD apt-get install elinks
    chroot $BUILD apt-get install links-ssl
    chroot $BUILD apt-get install lynx-ssl

    chroot $BUILD apt-get install python
    chroot $BUILD apt-get install gobjc

    chroot $BUILD apt-get install vim
    chroot $BUILD apt-get install w3-el-e21
    chroot $BUILD apt-get install w3m-el
    chroot $BUILD bash -c "apt-get install zsh; echo 'alias su=\"sudo su\"' >> /etc/zsh/zshrc"

    chroot $BUILD apt-get install workbone
    chroot $BUILD bash -c "apt-get install cdtool; cd /usr/bin; ln -s ../lib/cdtool/cdtool cdstart"
    chroot $BUILD apt-get install tnt
    chroot $BUILD apt-get install erc
    chroot $BUILD apt-get install xsltproc
    chroot $BUILD apt-get install ppthtml

    chroot $BUILD apt-get install aspell-fr
    chroot $BUILD apt-get install aspell-de
    chroot $BUILD apt-get install aspell-es
    chroot $BUILD apt-get install aspell-en

    chroot $BUILD bash -c "cd /usr/bin; ln -s aspell ispell"

#RUF    chroot $BUILD apt-get install console-cyrillic

    cd $BUILD/var/tmp; rm -rf aspell-ru-*; wget ftp://ftp.gnu.org/gnu/aspell/dict/ru/aspell-ru-0.50-2.tar.bz2; tar -jxvf aspell-ru-*;
    chroot $BUILD bash -c "cd /var/tmp/aspell-ru-*; ./configure; make; make install; cd /var/tmp; rm -rf  aspell-ru-*"

    chroot $BUILD apt-get install audacity
    chroot $BUILD apt-get install ecasound
    chroot $BUILD apt-get install vsound
#    chroot $BUILD apt-get install sawfish

    chroot $BUILD apt-get install ne
    chroot $BUILD apt-get install nano

    chroot $BUILD apt-get install nethack
    chroot $BUILD apt-get install nethack-console
    chroot $BUILD apt-get install bsdgames

    chroot $BUILD apt-get install xpdf-utils
    chroot $BUILD apt-get install catdoc
    chroot $BUILD apt-get install pdftohtml
    chroot $BUILD apt-get install wv
    chroot $BUILD apt-get install xlhtml

    chroot $BUILD apt-get install toolame    

    # Lirc
    # Answers during the installation
    # Create nodes: yes
    # Select device: Cancel
    chroot $BUILD apt-get install lirc

    # Doc
    cp -pR $DOC/* $BUILD/usr/share/oralux/doc

    # Copyright
    mkdir $BUILD/usr/share/doc/fdimage
    cp -pR $INSTALL_PACKAGES/fdimage/fdimage.c $BUILD/usr/share/doc/fdimage/copyright
    cp -pR $DOC/license/festival $BUILD/usr/share/doc/    
    cp -pR $DOC/license/mbrola $BUILD/usr/share/doc/
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
