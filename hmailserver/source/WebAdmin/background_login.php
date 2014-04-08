<?php
if (!defined('IN_WEBADMIN'))
   exit();

$username	= hmailGetVar("username","");
$password	= hmailGetVar("password","");


if (Login($username, $password))
{
	header("refresh: 0; url=" . $hmail_config['rooturl']);
	exit();		
}
else
{
   // Login failed.  
   LoginError();
}


function Login($username, $password)
{
   global $obBaseApp;
   
   if($username == "" || $password == "")
   {
      LoginError();
   }
   
   $obAccount = $obBaseApp->Authenticate($username, $password);
   if (!isset($obAccount))
      LoginError();
   
   $_SESSION['session_loggedin'] = 1;
   $_SESSION['session_adminlevel'] = $obAccount->AdminLevel();
   $_SESSION['session_username'] = $obAccount->Address;
   $_SESSION['session_password'] = $password;
   $_SESSION['session_domainid'] = $obAccount->DomainID();
   $_SESSION['session_accountid'] = $obAccount->ID();
   
   return true;
}

function LoginError()
{
	global $hmail_config;
	header("refresh: 0; url=" . $hmail_config['rooturl'] . "index.php?page=login&error=1");
	exit();	   
}


?>

