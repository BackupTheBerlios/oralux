<?php
define("DEBUG",false);
//define("DEBUG",true);
// $__Function__ must be updated

function ENTER($theString, $theLine=NULL)
{
  if (!DEBUG)
    {
      return;
    }
  print("> $theString ($theLine)\n");
}

function SHOW($theString)
{
  if (!DEBUG)
    {
      return;
    }
  printf(">>$theString<<\n");
}
?>