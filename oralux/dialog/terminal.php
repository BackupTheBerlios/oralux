<?php
// ----------------------------------------------------------------------------
// terminal.php
// $Id: terminal.php,v 1.4 2005/02/07 00:13:56 gcasse Exp $
// $Author: gcasse $
// Description: managing the dumb terminals (emacs) or the others (so called enhanced)  
// $Date: 2005/02/07 00:13:56 $ |
// $Revision: 1.4 $ |
// Copyright (C) 2004, 2005 Gilles Casse (gcasse@oralux.org)
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

// Notice: php5 is customized to have a non canonical input mode (concerned file: ext/dio/dio.c)

$BASE=dirname(__FILE__);
require_once("$BASE/../main/helpEmul.php");

// {{{ class enhancedTerminal

class enhancedTerminal
{
  function isDumbTerminal(){return false;}

  var $_myLastAnswer;
  protected $_myObjectIsBuilt=false;
  protected $_myUpArrowKey;
  protected $_myDownArrowKey;
  protected $_myEscapeKey;

  function isBuild()
    {
      ENTER("enhancedTerminal::isBuild",__LINE__);
      return $this->_myObjectIsBuilt;
    }

  // {{{ constructor, destructor

  function __construct()
    {
      ENTER("enhancedTerminal::__construct",__LINE__);
      if (getenv("TERM")!="dumb")
	{
	  $this->_myObjectIsBuilt=dio_open_stdin();
	  $this->_myUpArrowKey=sprintf("%c[A",0x1b);
	  $this->_myDownArrowKey=sprintf("%c[B",0x1b);
	  $this->_myRightArrowKey=sprintf("%c[C",0x1b);
	  $this->_myLeftArrowKey=sprintf("%c[D",0x1b);
	  $this->_myEscapeKey=sprintf("%c",0x1b);
	  $this->_myDelKey=sprintf("%c",0x7f);
	  $this->_myBackspaceKey=sprintf("%c",0x08);
	}
    }

   function __destruct() 
     {
      ENTER("enhancedTerminal::__destruct",__LINE__);
       if ($this->_myObjectIsBuilt)
	 {
	   dio_close_stdin();
	 }
     }

   // }}}
   
   // {{{ getChar: return the first typed character

   function getChar( & $theChar)
     {
      ENTER("enhancedTerminal::getChar",__LINE__);
       $aResult=getCharOK;
       $theChar=dio_read_stdin();

       if ($theChar==$this->_myEscapeKey)
	 {
	   $aResult=getCharEscape;
	 }
       else if ($theChar==$this->_myDownArrowKey)
	 {
	   $aResult=getCharDownArrowKey;
	 }
       else if ($theChar==$this->_myUpArrowKey)
	 {
	   $aResult=getCharUpArrowKey;
	 }
       return $aResult;
     }

   // }}}

   // {{{ getLine


   function getLine( & $theLine, $theLastInput=""  )
     {
      ENTER("enhancedTerminal::getLine",__LINE__);
       $aResult=getCharOK;

       define("DATA_LENGTH", 1024);
       $aEndOfLineFound=false;
       
       if ($theLastInput!="")
	 {
	    $theLine=$theLastInput;
	    $aIndex=strlen( $theLastInput);
	    echo $theLine;
	 }
       else
	 {
	   $theLine="";
	   $aIndex=0;
	 }

       while( !$aEndOfLineFound && ($aIndex < DATA_LENGTH))
	 {
	   $aChar=dio_read_stdin();
	
	    
	   switch($aChar)
	     {
	      case $this->_myEscapeKey:
		$aResult=getCharEscape;
		$aEndOfLineFound=true;
		break;
		
	     case $this->_myDownArrowKey:
		$aResult=getCharDownArrowKey;
		$aEndOfLineFound=true;
		break;
		
	      case $this->_myUpArrowKey:
		$aResult=getCharUpArrowKey;
		$aEndOfLineFound=true;
		break;
		
	      case $this->_myDelKey:
	      case $this->_myBackspaceKey:
		if ($aIndex>0)
		  {
		    $aIndex--;
		    $aChar=$theLine{$aIndex};
		    //say($aChar); // Pronounce the erased character

		    $theLine=substr ( $theLine, 0, $aIndex);
		    echo( $this->_myLeftArrowKey);
		    echo " ";
		    echo( $this->_myLeftArrowKey);
		  }
		break;

	      default:
		$aValue=ord($aChar);
		if (($aValue==0x0A) || ($aValue==0x0D))
		  {
		    echo $aChar;
		    $aResult=getCharOK;
		    $aEndOfLineFound=true;
		  }
		else
		  {
		    echo $aChar;
		    $aIndex++;
		    $theLine .= $aChar;
		    break;
		  }
	      }
	  }
	
       return $aResult;
     }

   // }}}

   // {{{ display the message according to the terminal

   function getMessage( $theMessageType, & $theMessage, $theValue=NULL)
     {
      ENTER("enhancedTerminal::getMessage",__LINE__);
       switch( $theMessageType)
	 {
	 case MessageNavigationMenu:
	   $theMessage[verbose][] = gettext("Press the up or down arrow key to read another item.\n");
	   $theMessage[verbose][] = gettext("And type return to select your preferred item.\n");
	   break;

	 case MessageNavigationRadio:
	   $theMessage[verbose][] = gettext("Press the up or down arrow key to read another option.\n");
	   $theMessage[verbose][] = gettext("And type return to select your preferred option.\n");
	   break;

	 case MessageNavigationCheckbox:
	   $theMessage[verbose][] = gettext("Press the up or down arrow key to read another option.\n");
	   $theMessage[verbose][] = gettext("And type yes to select an option, or type no to unselect it.\n");
	   $theMessage[verbose][] = gettext("Note that you can select several options.\n");
	   break;

	 case MessageNavigationInputBox:
// 	   $theMessage[verbose][] = "";  //gettext("Type your answer in this input field\n");
// 	   $theMessage[notVerbose][] = ""; //gettext("Input\n");
	   break;

	 case MessageNavigationInputBoxDefaultButton:
	   $theMessage[verbose][] = sprintf(gettext("Just press return if you accept this proposal: %s\n"), $theValue);
	   $theMessage[notVerbose][] = sprintf(gettext("Proposal: %s\n"), $theValue);
	   break;

	 default:
	   echo "display: unknown message $theMessage\n";
	   break;
	 }
     }

   // }}}
}

// }}}
// {{{ class dumbTerminal

class dumbTerminal
{
  var $_myLastAnswer;
  function isDumbTerminal(){return true;}

  protected $_myFileDescriptor=NULL;

 // {{{ constructor, destructor

  function __construct()
    {
      ENTER("dumbTerminal::__construct",__LINE__);
      $this->_myFileDescriptor=fopen("php://stdin","r");
      if ($this->_myFileDescriptor===false)
	{
	  die(sprintf("Fatal error: fopen (%s %s)",__FILE__,__LINE__));
	}
      SHOW($this->_myFileDescriptor);
    }

   function __destruct() 
     {
       ENTER("dumbTerminal::__destruct",__LINE__);
       SHOW($this->_myFileDescriptor);
       $a=fclose($this->_myFileDescriptor);
       if ($a===false)
	 {
	   die(sprintf("Fatal error: fclose (%s %s)",__FILE__,__LINE__));
	 }
     }
   // }}}

   // {{{ getChar: return the first typed character
   function getChar( & $theChar)
     {
      ENTER("dumbTerminal::getChar",__LINE__);
       $aResult=getCharOK;

       SHOW("value=");
       SHOW($this->_myFileDescriptor);

       $aString=fgets( $this->_myFileDescriptor);
       
       if ($aString===false)
	 {
	   die(sprintf("Fatal error: fgets (%s %s)",__FILE__,__LINE__));
	 }   

       $theChar=$aString{0};

       if ($theChar=="\n")
	 {
	   $aResult=getCharDownArrowKey;
	 }

       SHOW($theChar);
       return $aResult;
     }
   // }}}

   // {{{ getLine

   function getLine( & $theLine, $theLastInput="" )
     {
      ENTER("dumbTerminal::getLine",__LINE__);
       $aResult=getCharOK;
       $theLine=fgets( $this->_myFileDescriptor);
       if ($theLine===false)
	 {
	   //die(sprintf("Fatal error: fgets (%s %s)",__FILE__,__LINE__));
	 }
       SHOW($theLine);
       return $aResult;
     }

   // }}}

   // {{{ display the message according to the terminal

   function getMessage( $theMessageType, & $theMessage, $theValue=NULL)
     {
      ENTER("dumbTerminal::getMessage",__LINE__);
       switch( $theMessageType)
	 {
	 case MessageNavigationMenu:
	   $theMessage[verbose][] = gettext("Press return to read the next item.\n");
	   $theMessage[verbose][] = gettext("And type yes to select your preferred item.\n");
	   break;

	 case MessageNavigationRadio:
	   $theMessage[verbose][] = gettext("Press return to read the next option.\n");
	   $theMessage[verbose][] = gettext("And type yes to select your preferred option.\n");
	   break;

	 case MessageNavigationCheckbox:
	   $theMessage[verbose][] = gettext("Press return to read the next option.\n");
	   $theMessage[verbose][] = gettext("And type yes to select an option, or type no to unselect it.\n");
	   $theMessage[verbose][] = gettext("Note that you can select several options.\n");
	   break;
	   
	 case MessageNavigationInputBox:
	   $theMessage[verbose][] = gettext("Type your answer in this input field\n");
	   $theMessage[notVerbose][] = gettext("Input\n");
	   break;

	 case MessageNavigationInputBoxDefaultButton:
	   $theMessage[verbose][] = sprintf(gettext("Just type yes if you accept this proposal: %s\n"), $theValue);
	   $theMessage[notVerbose][] = sprintf(gettext("Proposal: %s\n"), $theValue);
	   break;

	 default:
	   echo "display: unknown message $theMessage\n";
	   break;
	 }
     }

   // }}}
}

// }}}
