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

function ErrorMessage( $theError, $theLine, $theFile, $theRevision)
{
  fwrite(STDERR, "\n***\n");
  fwrite(STDERR, gettext("Thanks to report this error to contact@oralux.org\n"));
  fwrite(STDERR, $theError);
  fwrite(STDERR, sprintf("Line: %s\n", $theLine));
  fwrite(STDERR, sprintf("File: %s\n", $theFile));
  fwrite(STDERR, sprintf("Revision: %s\n", $theRevision));
  fwrite(STDERR, "***\n");
}

?>