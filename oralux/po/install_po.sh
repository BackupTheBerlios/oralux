#!/bin/sh
# install_po.sh : install the new translated messages
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
# The files which are used :
# 
#  - fr.def.po : the po file of the last release
#  - oralux.po : a temporary file (the new po file without no translation)
#  - fr.new.po : the new po file (merge of fr.def.po and oralux.po) 
#

echo "Please, enter your language code (default = $LANG)"
read lg

# if lg is empty, $LANG is used 

case v$lg in
  v) lg=$LANG;;
esac

# The directory ../locale/$lg/LC_MESSAGES is created if it does not exist

msg_dir="../locale/$lg/LC_MESSAGES"

if test ! -d $msg_dir
then
  echo "Creating directory $msg_dir ..."
  mkdir -p $msg_dir    
fi

mv $lg.def.po $lg.def.po.bak
mv $lg.new.po $lg.def.po

msgfmt $lg.def.po -o oralux.mo
mv oralux.mo ../locale/$lg/LC_MESSAGES

echo "...the file oralux.mo is installed in ../locale/$lg/LC_MESSAGES"   
