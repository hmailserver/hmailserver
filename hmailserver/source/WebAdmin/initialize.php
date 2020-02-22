<?php
if (!defined('IN_WEBADMIN'))
   die;

require_once("include/functions.php");
require_once("include_versioncheck.php");

session_start();

// Enable CSRF protection
ensure_csrf_session_token_exists();

// Connect to hMailServer
try
{
   $obBaseApp = new COM("hMailServer.Application", NULL, CP_UTF8);
}
catch(Exception $e)
{
   echo $e->getMessage();
   echo "<br>";
   echo "This problem is often caused by DCOM permissions not being set.";
   die;
}


if ($obBaseApp->Version != REQUIRED_VERSION)
{
   echo "<br>";
   echo "The hMailServer version does not match the WebAdmin version.<br>";
   echo "hMailServer version: " . $obBaseApp->Version . "<br>";
   echo "WebAdmin version: " . REQUIRED_VERSION . "<br>";
   echo "Please make sure you are using the PHPWebAdmin version which came with your hMailServer installation.<br>";
   die;
}

try
{
   $obBaseApp->Connect();
   
   if (isset($_SESSION['session_username']) && 
       isset($_SESSION['session_password']))
   {
   	// Authenticate the user
   	$obBaseApp->Authenticate($_SESSION['session_username'], $_SESSION['session_password']);
   }
}
catch(Exception $e)
{
   echo $e->getMessage();
   die;
}


$obLanguage = $obBaseApp->GlobalObjects->Languages->ItemByName($hmail_config['defaultlanguage']);

?>