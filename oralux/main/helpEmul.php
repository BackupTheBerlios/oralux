<?php
define("DEBUG",true);
// $__Function__ must be updated

function ENTER($theString, $theLine=NULL)
{
  if (!DEBUG)
    {
      return;
    }
  echo "> $theString ($theLine)\n";
}
function SHOW($theString)
{
  if (!DEBUG)
    {
      return;
    }
  echo ">>$theString<<\n";
}
?>