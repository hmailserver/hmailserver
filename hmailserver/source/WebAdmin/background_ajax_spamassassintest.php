<?php
   if (!defined('IN_WEBADMIN'))
      exit();
      
   if (hmailGetAdminLevel() != ADMIN_SERVER)
   	hmailHackingAttemp(); // The user is not server administrator.
   
   $Hostname = hmailGetVar("Hostname", "localhost");
   $Port = hmailGetVar("Port", 783);
   
   $message = "";
   $AntiSpam = $obBaseApp->Settings->AntiSpam;
   $result = $AntiSpam->TestSpamAssassinConnection($Hostname, $Port, $message);
   
   echo $result;
?>