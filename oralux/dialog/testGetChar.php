<?php
require_once("terminal.php");

$aTerminal=new enhancedTerminal();
if (!$aTerminal->isBuild())
{
  $aTerminal=new dumbTerminal($theTerminal);
}

echo "type a character please: ";
$aTerminal->getChar( $aChar);

printf("\nThe value of the char is: 0x%x\n",ord($aChar));

?>