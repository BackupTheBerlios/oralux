<?php 

function ExecRequest ($request, $connexion)
{
  $result = mysql_query ($request, $connexion);
  
  if ($result)
    {
      return $result;
    }
  else 
    {  
      echo ("<B>".gettext("Error")." : "."$request</B><BR>");
      echo ("<B>".gettext("Message from MySQL")." : </B>".mysql_error($connexion));
      exit;
    }  
}

function authentification()
{
  // header("HTTP/1.0 401 Unauthorized");
  // For TuxFamily
  header("status: 401 Unauthorized");

  header("WWW-Authenticate: Basic realm=\"PhpWiki\"");
  echo "Invalid login or password.";
  exit;
}

function Connexion ($check_validity=TRUE)
{
  // the following global variables are stored in a distinct file
  global $WIKISERVER, $WIKILOGIN, $WIKIBASEPASSWORD, $BASE;

  if (!isset($_SERVER["PHP_AUTH_USER"]) 
      || !isset($_SERVER["PHP_AUTH_PW"]))
    {
      authentification();
    }

  $connexion = mysql_pconnect($WIKISERVER, $WIKILOGIN, $WIKIBASEPASSWORD);

  if (!$connexion) 
    {
      echo (gettext("Sorry, impossible connexion to server"));
      exit;
    }

  if (!mysql_select_db ($BASE, $connexion)) 
    {
      echo (gettext("Sorry, impossible connexion to database"));
      echo "<B>Message de MySQL :</B> " . mysql_error($connexion);
      exit;
    }


  // from the manual, Chapter 16

  $query="SELECT my_username FROM user WHERE my_login='{$_SERVER["PHP_AUTH_USER"]}' and my_password='{$_SERVER["PHP_AUTH_PW"]}'";

  $result = mysql_query ($query, $connexion);

  if(mysql_num_rows($result)==0)
    {
      authentification();

      echo (gettext("Sorry, wrong username or password"));
      exit;
    }

  return $connexion;
}

// Entry 
$cnx=Connexion ($check_validity=TRUE);

$aMailIsSend=false;

if (isset($_REQUEST["my_destination"]))
{
  $aRequest="select * from user where my_login='{$_SERVER["PHP_AUTH_USER"]}'";
  $aResult=ExecRequest ($aRequest, $cnx);
  $aUser=mysql_fetch_array($aResult);

  $a_header="From: {$aUser["my_username"]} <{$aUser["my_address"]}>\n";

  if ($_REQUEST["my_cc"]!="")
    {
      $a_header.="Cc: {$_REQUEST["my_cc"]}\n";
    }
       
  $a_header.="Content-Type: text/plain; charset=iso-8859-1\n";  

  $aDestination=$_REQUEST["my_destination"];
  $aSubject=$_REQUEST["my_subject"];
  $aMessage=wordwrap(stripSlashes($_REQUEST["my_body"]),72);

  $aMailIsSend=mail($aDestination, $aSubject, $aMessage, $a_header);
}

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="fr" lang="fr">
<head>
<title>Oralux : Mail</title>
<meta http-equiv="Content-Type" content="text/html;
     charset=iso-8859-1" />
</head>
<body id="oralux-org">

  <div id="main">
  <!--h2></h2-->
<?php

if ($aMailIsSend)
{
echo "<p>Your mail has been sent.</p>";
}

?>

  <form action="http://oralux.org/mail.php" method="post">
   <fieldset>
     <legend>Sending a mail</legend>
	<br />
     <label>To:
     <input type="text" name="my_destination" />
     </label>

	<br />
     <label>Cc:
     <input type="text" name="my_cc" />
     </label>

	<br />
     <label>Subject:
     <input type="text" name="my_subject" />
     </label>

	<br />
     <label>Body:
<textarea name="my_body" rows="30" cols="80"></textarea>
     </label>

	<br />
     <input type="submit" value="Send" />
   </fieldset>
  </form>
 </div> <!-- main -->
</body>
</html>
