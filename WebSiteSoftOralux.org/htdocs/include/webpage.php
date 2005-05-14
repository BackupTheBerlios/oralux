<?PHP
// ----------------------------------------------------------------------------
// webpage.php
// $Id: webpage.php,v 1.1 2005/05/14 01:32:08 gcasse Exp $
// $Author: gcasse $
// Description: the skeleton which eases building any web page of the site
// $Date: 2005/05/14 01:32:08 $ |
// $Revision: 1.1 $ |
// Copyright (C) 2003 Gilles Casse (gcasse@oralux.org)
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
ini_set('arg_separator.output', '&amp;');
// To be compliant with xhtml 1.0 strict since php may add &PHPSESSID=... on the url of a page.
//ini_set("arg_seperator.output", "&amp;")); // to have an url compliant with xhtml
//ini_set("session.use_cookies",false); // url to avoid pb with isp
// for avoiding the php bug which adds an input hidden field just after the form (do not validate)
//ini_set("url_rewriter.tags","a=href,area=href,frame=src,input=src");

session_start(); // first line, because it sends the http header

require_once("header.php");
require_once("doctype.php");
require_once("head.php");
require_once("footer.php");
require_once("tools.php");

class webPage
{
  var $_mydocType=null;
  var $_myHtmlHead=null;
  var $_myHeader=null;
  var $_myFooter=null;

  // webPage
  // INPUT:
  // theEntry: this is the current entry of the menu
  // theDate: to display in the head of the page, the current date of change
  // theStyles: an array of style sheets (e.g. "faq.css") which will complete the default ones.
  // theForm: array used to initialize the input fields of the form. 
  // e.g. theForm["salutation"]="mister";
  // theData: array supplying dynamic content (e.g. translation).
  function webPage($theEntry, $theDate, $theStyles=null, $theForm=NULL, $theData=NULL)
    {
      $this->_mydocType=new docType();      
      $this->_myHtmlHead=new head($theEntry, $theDate, $theStyles);
      $this->_myHeader=new header($theEntry);
      $this->_myFooter=new footer($theEntry);
      $this->_myForm=$theForm;
      $this->_myData=$theData;
    }

  // Embedded in the html code
  function printSelect( $theField, $theValue)
    {
      $aValue="";
      if ($this->_myForm["$theField"]=="$theValue") 
	{
	  $aValue=" selected='selected'";
	}
      return $aValue;
    }
  
  // Embedded in the html code
  function printField( $theField)
    {
      return htmlentities($this->_myForm["$theField"],ENT_QUOTES);
    }

  function printTag($theHtmlBody)
    {
// Printing the web page
// First, doctype!
     $this->_mydocType->printTag();
     $lang=getLang();
     echo("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"$lang\" lang=\"$lang\">\n");
     echo("<head>\n");
     $this->_myHtmlHead->printTag();
     echo("</head>\n");

     echo("<body id=\"oralux-org\">\n");
     $this->_myHeader->printTag();
     
     if (!empty($this->_myForm))
       {
	 $FORM=& $this->_myForm;
       }
     if (!empty($this->_myData))
       {
	 $DATA=& $this->_myData;
       }
     include(GetTranslatedFile($theHtmlBody));
     $this->_myFooter->printTag();
     echo("</body>\n");
     echo("</html>\n");
   }

 function printBuffer($theBuffer)
   {
// Printing the web page
// First, doctype!
     $this->_mydocType->printTag();
     $lang=getLang();
     echo("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"$lang\" lang=\"$lang\">\n");
     echo("<head>\n");
     $this->_myHtmlHead->printTag();
     echo("</head>\n");

     echo("<body id=\"oralux-org\">\n");
     $this->_myHeader->printTag();

     echo $theBuffer;
     $this->_myFooter->printTag();
     echo("</body>\n");
     echo("</html>\n");
   }

} // class webPage

?>







