#!/usr/bin/php4 -q
<?PHP 
// ----------------------------------------------------------------------------
// listPartitions.php
// $Id: listPartitions.php,v 1.1 2004/09/27 20:30:25 gcasse Exp $
// $Author: gcasse $
// Description: Listing partitions with a few informations (size, file system,...) 
// $Date: 2004/09/27 20:30:25 $ |
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


function GetSizeInNearestUnit(&$theSizeInKiloBytes, &$theUnit)
{
  $theUnit="Kilo Bytes";
  if ($theSizeInKiloBytes>=1000000)
  {
    $theSizeInKiloBytes=sprintf("%.1f",$theSizeInKiloBytes/pow(2,20));
    $theUnit="Giga Bytes";
  }
  else if ($theSizeInKiloBytes>=1000)
  {
    $theSizeInKiloBytes=sprintf("%.1f",$theSizeInKiloBytes/pow(2,10));
    $theUnit="Mega Bytes";
  }
}

// Expected arrays:
// aListOfPartitions["hda1"]["Size"]
// Size, or Path, Type, Access

// Features of the partitions
$aFile=fopen("/proc/partitions","r");
$i=0;
while(!feof($aFile)) 
{
  // Filtering the comments, just keep e.g. /dev/hda1
  $a=trim(fgets($aFile,"1024"));
  if (preg_match("/\d+ [hs]d.\d\d?$/",$a,$b))
  {
    $c=split(" ",$b[0]);
    $aListOfPartitions["$c[1]"]["Size"]=$c[0];
    $aListOfPartitions["$c[1]"]["Name"]=$c[1];
  }
}
fclose($aFile);

// Is it already mounted
// First, looking for the mounted partitions
$aFile=fopen("/proc/mounts","r");

while(!feof($aFile)) 
{
  // Filtering the comments, just keep e.g. /dev/hda1
  $a=trim(fgets($aFile,"1024"));
  
  // The line which is read looks like that:
  //    /dev/hda7 / ext3 rw 0 0
  $fields=split (" ",$a);
  
  if (preg_match("/\/dev\/[hs]d.\d\d?/",$fields[0],$b))
  {
    $aDevice=split ("/",$fields[0]);
    $aListOfPartitions["$aDevice[2]"]["Path"]=$fields[1];
    $aListOfPartitions["$aDevice[2]"]["Type"]=$fields[2];
    $anAccess=split (",",$fields[3]);
    $aListOfPartitions["$aDevice[2]"]["Access"]=$anAccess[0];
  }
}
fclose($aFile);

// Looking for the swap
$aFile=fopen("/proc/swaps","r");
while(!feof($aFile)) 
{
  // Filtering the comments, just keep e.g. /dev/hda1
  $a=trim(fgets($aFile,"1024"));
  
  if (preg_match("/^\/dev\/[hs]d.\d\d?/", $a, $b))
  {
    $aDevice=split ("/", $b[0]);
    $aListOfPartitions["$aDevice[2]"]["Type"]="swap";
  }
}
fclose($aFile);

// Features of the hd device
for ($i=0; $i<2; $i++)
{
  for ($j=0; $j<2; $j++)
  {
    $a = chr( 0x61 + 2*$i + $j);
    $aDevice="/proc/ide/ide$i/hd$a";
    if (@is_dir($aDevice))
    {
      $aFile=fopen("$aDevice/capacity","r");
      $aListOfDevices["hd$a"]["CapacityInKiloBytes"]=trim(fgets($aFile,"1024"));
      fclose($aFile);
      $aFile=fopen("$aDevice/media","r");
      $aListOfDevices["hd$a"]["media"]=trim(fgets($aFile,"1024"));
      fclose($aFile);
    }
  }
}

// Grouping the partitions under each devices
for ($i=0x61; $i<=0x7a; $i++)
{
  $a=chr($i);
  if (is_array($aListOfDevices["hd$a"]))
  {
    // Related partitions
    unset($aText);
    $j=0;
     foreach ($aListOfPartitions as $aPartition => $anArray)
      {
        if (preg_match("/hd$a\d\d?/",$aPartition,$b))
        {
          $aListOfDevices["hd$a"]["partition"][$j]=$anArray;
          $j++;
        }
      }     
//          $aText.=sprintf("\n$aPartition: type=%s, size=%s, access=%s",$anArray["Type"],$anArray["Size"],$anArray["Access"])  
  }
}

// Displaying results
foreach ($aListOfDevices as $aName => $aDevice)
{
//    printf("\nDevice %s: %s of %s",);
  $aSize=$aDevice["CapacityInKiloBytes"];
  GetSizeInNearestUnit($aSize, $aUnit);
  printf("\n\n%s: %s (%s %s)", $aName, $aDevice["media"], $aSize, $aUnit);

  if (is_array($aDevice["partition"]))
  {
    printf("\n%s partitions:",count($aDevice["partition"]));
    foreach ($aDevice["partition"] as $aPartitionName => $aPartition)
      {
        $aSize=$aPartition["Size"];
        GetSizeInNearestUnit($aSize, $aUnit);
        switch($aPartition["Access"])
        {
        case "ro":
          $anAccess="Read only";
          break;
        case "rw":
          $anAccess="Read and Write possible";
          break;
        default:
          if ($aPartition["Type"]=="swap")
          {
            $anAccess="";
          }
          else
          {
            $anAccess="Not mounted";
          }
          break;
        }

        switch($aPartition["Type"])
        {
        case "swap":
          $aType="swap";
          break;
        case "ntfs":
          $aType="ntfs";
          break;
        default:
          $aType="";
          break;
        }

        printf("\n%s: %s %s (%s %s)",$aPartition["Name"],$aType,$anAccess,$aSize,$aUnit);  
      }
  }
}






      
//       if ($aCapacityInKiloBytes<1000)
//       {
//         sprintf("%d Mega Bytes\n", $aCapacityInKiloBytes/1000);
//       }
//       else
//       {
//         printf("%d Giga Bytes\n", $aCapacityInKiloBytes/1000000);
//       }


//       if ($aCapacityInKiloBytes<1000)
//       {
//         sprintf("%d Mega Bytes\n", $aCapacityInKiloBytes/1000);
//       }
//       else
//       {
//         printf("%d Giga Bytes\n", $aCapacityInKiloBytes/1000000);
//       }
//fclose($aFile);


//   $aFile=fopen("/etc/fstab","r");


//   $aFile=fopen("/etc/fstab","r");




//   $i=0;
//   while(!feof($aFile)) 
//   {
//     // Filtering the comments, just keep e.g. /dev/hda1
//     $a=fgets($aFile,"1024");

//     if (preg_match("/^\/dev\/[hs]d.\d\d?/",$a,$b))
//     {
//       echo "\n$b[0]";
//     }
//   }

//   fclose($aFile);

?>
