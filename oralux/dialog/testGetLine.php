<?php
require_once("terminal.php");

$aTerminal=new enhancedTerminal();
if (!$aTerminal->isBuild())
{
  $aTerminal=new dumbTerminal($theTerminal);
}

echo "Enter a few words and press return: ";
$aTerminal->getLine( $aLine);
printf("\nThe line is: %s\n",$aLine);

?>