<?PHP

// ----------------------------------------------------------------------------
// GenericTextToSpeech.php
// $Id: GenericTextToSpeech.php,v 1.1 2004/09/27 20:30:30 gcasse Exp $
// $Author: gcasse $
// Description: Using the generic TextToSpeech which is embedded with the distribution. 
// $Date: 2004/09/27 20:30:30 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2003, 2004 Gilles Casse (gcasse@oralux.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// ----------------------------------------------------------------------------

function say($the_sentence_to_say)
{
  ENTER("say");
  $fd=fopen("/tmp/a.txt","w");
  fwrite($fd, "q \{$the_sentence_to_say}\nd\n");
  fclose($fd);
  system("eflite -f /tmp/a.txt");
  echo ($the_sentence_to_say);
}

?>