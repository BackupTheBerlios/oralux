#!/bin/sh
# new_po.sh : create or update xx.new.po, (xx = fr,...) the file which 
# includes the translation for the xx language.
#
# This little script gathers the Oralux messages (in english) 
# and merges them with the file xx.def.po which owns all the previous 
# translations (if they exist).
# Finally, it creates xx.new.po which will receive the new translations.
#  
# ----------------------------------------------------------------------------
# Copyright (C) 2005 Gilles Casse (gcasse@oralux.org)
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
#
# The files which are used :
# 
#  - xx.def.po : the po file of the last release
#  - oralux.pot : a temporary file (the pot file without any translation)
#  - xx.new.po : the new po file (merge of xx.def.po and oralux.pot) 
#

echo "Please, enter your language code (default = $LANG)"
read lg

# if lg is empty, $LANG is used 

case v$lg in
  v) lg=$LANG;;
esac

if test -s $lg.new.po
then
  echo "The $lg.new.po file already exists.";
  echo "Do you really want to remove it ? (y/n)";
  read q
  case $q in
    y|Y) ;;
    *) echo "ok, we stop here.";exit 1;;
  esac
fi

touch $lg.def.po $lg.new.po oralux.pot
chmod +w $lg.def.po $lg.new.po oralux.pot

echo "Building the new $lg.new.po..."   

#xgettext -C -s `ls *.php` fic.c -o oralux.pot
xgettext --no-location -s `ls ../dialog/*.php ../main/*.php` -o oralux.pot
#xgettext -s `ls *.php` fic.c -o oralux.pot
msgmerge $lg.def.po oralux.pot -o $lg.new.po

echo "...the file $lg.new.po is ready"   
