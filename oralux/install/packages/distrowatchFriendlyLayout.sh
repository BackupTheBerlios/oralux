#!/bin/sh

getRelease()
{
  aPackage=$1
  a=$(dpkg -l $aPackage 2>/dev/null | grep $aPackage | awk '{print $3}')
  b=$(echo $a | cut -c 1)
  if [ -z $a ]; then
     echo "--"
  elif [ "$b" == "<" ]; then
      echo "--"
  else 
      echo $a
  fi
}

release="0.6 alpha"
date="2004/05/20"
kernel=$(uname -a | awk '{print $3}')

echo "Feature: $release"
echo "Release Date: $date"
echo "Price (US$): Free"
echo "CDs: 1"
echo "Free Download: ISO"

echo "Installation: --"
echo "Default Desktop: Emacspeak"
echo "Package Management: DEB"
echo "Office Suite: --"
echo "Processor Architecture: i386"
echo "Journaled File Systems: --"
echo "Multilingual: de, en, es, fr, ru"
echo "Asian Language Support: --"

echo "Package: $release"
echo "abiword: $(getRelease abiword)"
echo "alsa-lib: $(getRelease alsa-headers)"
echo "apache: $(getRelease apache)"
echo "bind : $(getRelease bind9)"
echo "cdrtools: $(getRelease cdrecord)"
echo "cups: $(getRelease cupsys)"
echo "db: $(getRelease libdb4.1)"
echo "dhcp: $(getRelease dhcp3-server)"
echo "emacs: $(getRelease emacs21)"
echo "evolution: $(getRelease evolution)"
echo "fetchmail: $(getRelease fetchmail)"
echo "gaim: $(getRelease gaim)"
echo "gcc: $(getRelease gcc)"
echo "gimp: $(getRelease gimp)"
echo "glibc: $(getRelease libc6)"
echo "gnucash: $(getRelease gnucash)"
echo "gnumeric: $(getRelease gnumeric)"
echo "httpd: $(getRelease httpd)"
echo "gtk+: $(getRelease libgtk2.0-0)"
echo "iptables: $(getRelease iptables)"
echo "k3b: $(getRelease k3b)"
echo "kdebase: $(getRelease kdebase)"
echo "koffice: $(getRelease koffice)"
echo "libgnome: $(getRelease libgnome)"
echo "linux: $kernel"
echo "Package: $release"
echo "modutils: $(getRelease modutils)"
echo "mozilla: $(getRelease mozilla)"
echo "MPlayer: $(getRelease mplayer)"
echo "mysql: $(getRelease mysql-common)"
echo "nautilus: $(getRelease nautilus)"
echo "OpenOffice.org: $(getRelease openoffice)"
echo "openssh: $(getRelease ssh)"
echo "openssl: $(getRelease openssl)"
echo "perl: $(getRelease perl)"
echo "php: $(getRelease php4)"
echo "postfix: $(getRelease postfix)"
echo "Python: $(getRelease python)"
echo "qt-x11-free: $(getRelease qt-x11-free)"
echo "quanta: $(getRelease quanta)"
echo "rpm: $(getRelease rpm)"
echo "samba: $(getRelease samba)"
echo "scribus: $(getRelease scribus)"
echo "sendmail: $(getRelease sendmail)"
echo "vim: $(getRelease vim)"
echo "webmin: $(getRelease webmin)"
echo "XFree86: $(getRelease xfree86-common)"
echo "xine-lib: $(getRelease xine-lib)"
echo "xinetd: $(getRelease xinetd)"
echo "xmms: $(getRelease xmms)"
