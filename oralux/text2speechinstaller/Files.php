<?PHP
// ----------------------------------------------------------------------------
// Files.php
// $Id: Files.php,v 1.1 2004/09/27 20:30:30 gcasse Exp $
// $Author: gcasse $
// Description: A few tools to manage files or directories. 
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

require_once("../main/Constants.php");

// LookForDirectory
// This function looks for a directory in any partition of the local hard or scsi disks.
//
// Input: 
// $theDirectory: This is the name of the expected directory
//
// Output:
// $thePath: The path to the found directory. 
// Empty is returned if nothing is found.
//
// Returned Value: 
// true if $theDirectory is found
//
// Notices
// 1. The directory may be placed in an unmounted partition.
//    Then, the partition is mounted on /mnt2.
// 2. The directory is supposed to be at the top level of the tree
//
function LookForDirectory($theDirectory, &$thePath)
{
  Debug("LookForDirectory");
  Debug("Input: theDirectory=$theDirectory");
  $thePath="";

//  system("mount -t proc /proc /proc");

  // First, looking for the mounted partitions
  $aFile=fopen("/proc/mounts","r");

  $i=0; // I prefer using an index...
  while(!feof($aFile)) 
  {
    // Filtering the comments, just keep e.g. /dev/hda1
    $a=fgets($aFile,"1024");

    // The line which is read looks like that:
    //    /dev/hda7 / ext3 rw 0 0
    $fields=split (" ",$a);

    if (preg_match("/\/dev\/[hs]d.\d\d?/",$fields[0],$b))
    {
      $aMountedFileSystem[$i]["PARTITION"]=$fields[0];
      $aMountedFileSystem[$i]["PATH"]=$fields[1];
      $aMountedFileSystem[$i]["TYPE"]=$fields[2];
      $aMountedFileSystem[$i]["ACCESS"]=$fields[3];
      $i++;
    }
  }
  fclose($aFile);
//  system("umount /proc");

  unset($i);
  Debug ("\nMounted Filesystem:");
  unset($aPath);
  $isFound=FALSE;
  if (is_array($aMountedFileSystem))
  {
    foreach($aMountedFileSystem as $aFileSystem)
      {
        Debug ("\n++Dir:".$aFileSystem["PATH"]."/".$theDirectory."\n");
        if (@is_dir($aFileSystem["PATH"]."/".$theDirectory)) 
        {
          Debug("\n++Yes, found!\n");

          if ($aFileSystem["TYPE"] != "ntfs")
          {
            // To be sure that the partition may be written
            system("mount -o remount,rw ".$aFileSystem["PATH"]."  &>/dev/null"); //Output filtered to avoid spoken words !!!
            Debug("\n++Remounted\n");

            // RAF(GC): Restoring the initial access rights once dtk is uncompressed
          }
          $thePath=$aFileSystem["PATH"];
          $isFound=TRUE;
          break;
        }
      }
  }

  if (TRUE==$isFound)
  {
    Debug("Output: thePath=$thePath");
    Debug("Returned value: $isFound");
    return TRUE;
  }

  // Second, trying with the unmounted partition

  // 1. Looking for the swap
//  system("mount -t proc /proc /proc");
  $aFile=fopen("/proc/swaps","r");
  $aSwap=$fields[0];
  while(!feof($aFile)) 
  {
    // Filtering the comments, just keep e.g. /dev/hda1
    $a=fgets($aFile,"1024");
    $fields=split (" ",$a);

    if (preg_match("/\/dev\/[hs]d.\d\d?/",$fields[0],$b))
    {
      $aSwap=$fields[0];
    }
  }
  fclose($aFile);

  // 2. Looking for the current partitions (on hda,hdb,... and sda,...)
  $aFile=fopen("/proc/partitions","r");
  unset($aPartition); // To be sure that the array is empty

  // Storing each of the found partitions in a array 
  while(!feof($aFile))
  {
    // Filtering the comments, just keep e.g. hda1...
    $a=fgets($aFile,"1024");
    $fields=split (" +",$a);
    $aDeviceName=$fields[4]; // There is a final /n
    if (preg_match("/[hs]d.\d\d?/",$aDeviceName,$b)
      && ("/dev/".$b[0] != $aSwap))
    {
      $aPartition[]="/dev/".$b[0];
    }
  }
  fclose($aFile);
//  system("umount /proc");

  Debug("\nTotal=".count($aPartition)."Mounted".count($aMountedFileSystem));

  // The unmounted partitions are present in the variable 'aPartition'
  // but absent in 'aMountedFileSystem'
  unset($aDevice);
  if (is_array($aMountedFileSystem))
  {
    foreach($aMountedFileSystem as $aArray)
      {
        $aDevice[]=$aArray["PARTITION"];
      }
  }
  if (isset($aDevice))
  {
    $anUnmountedFileSystem=array_diff($aPartition, $aDevice);
  }
  else
  {
    $anUnmountedFileSystem=$aPartition;
  }

  // We will need to mount each partition on DIR_MOUNT
  if (!@is_dir(DIR_MOUNT))
  {
    mkdir(DIR_MOUNT,"700");
  }

  Debug ("\nUnmounted Filesystem:");

  foreach($anUnmountedFileSystem as $aFileSystem)
    {
      Debug ("\n$aFileSystem");
      // RAF(GC): remembering that the filesystem was not mounted...
      system("mount -o rw $aFileSystem ".DIR_MOUNT." &>/dev/null");
      if (@is_dir(DIR_MOUNT."/".$theDirectory)) 
      {
        Debug("\n++Found:$aFileSystem==\n");
        $thePath=DIR_MOUNT;
        $isFound=TRUE;
        break;
      }
      @system("umount ".DIR_MOUNT);
    }

  Debug("Final value=".count($anUnmountedFileSystem));
  Debug("Output: thePath=$thePath");
  Debug("Returned value: $isFound");

  return $isFound;
}


// CopyFiles
// A quite short function which recursively copies a number of directory from one directory to another one.It works too with a list of files.
//
// Input:
// $theSourcePath
// $theDestinationPath
// $theDirectoryList: the files or directories to be copied
// 
function CopyFiles( $theSourcePath, $theDestinationPath, $theDirectoryList)
{
  Debug("CopyFiles");
  Debug("theSourcePath=$theSourcePath, theDestinationPath=$theDestinationPath");
  Debug($theDirectoryList);

  // To be sure that the destination exists
  system("mkdir -p $theDestinationPath");

  if (!@is_writable($theDestinationPath))
  {
    say(sprintf("Sorry, impossible to write in directory %s\n"), $theDestinationPath);
    exit(1);
  }

  $aCurrentDirectory=getcwd();
  chdir($theDestinationPath);
  foreach( $theDirectoryList as $aDirectory)
    {
      Debug("copied aDirectory=$aDirectory");
      system("cp -pR $theSourcePath/$aDirectory .");
    }
  chdir($aCurrentDirectory);
}


