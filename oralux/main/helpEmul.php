<?php
define("DEBUG", is_file("helpEmulDebug"));

function ENTER($theString, $theLine=NULL)
{
  if (DEBUG)
    {
      print("> $theString ($theLine)\n");
    }
}

function SHOW($theString)
{
  if (DEBUG)
    {
      printf(">>$theString<<\n");
    }
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