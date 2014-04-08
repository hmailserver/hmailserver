<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   if (hmailGetAdminLevel() != ADMIN_SERVER)
   	hmailHackingAttemp(); // The user is not server administrator.
   
   $action	            = hmailGetVar("action","");
   $relayid	   = hmailGetVar("relayid",0);
   
   if ($action == "edit")
      $obIncomingRelay     = $obBaseApp->Settings->IncomingRelays->ItemByDBID($relayid);
   elseif ($action == "add")
      $obIncomingRelay     = $obBaseApp->Settings->IncomingRelays->Add();
   elseif ($action == "delete")
   {
      $obBaseApp->Settings->IncomingRelays->DeleteByDBID($relayid);
      header("Location: index.php?page=incomingrelays");
   }
      
   // Fetch form
   $relayname		         = hmailGetVar("relayname","0");
   $relaylowerip	         = hmailGetVar("relaylowerip","0");
   $relayupperip	         = hmailGetVar("relayupperip","0");

   // Save the changes
   $obIncomingRelay->Name = $relayname;
   $obIncomingRelay->LowerIP = $relaylowerip;
   $obIncomingRelay->UpperIP = $relayupperip;

   $obIncomingRelay->Save();
   
   $relayid = $obIncomingRelay->ID;
   
   header("Location: index.php?page=incomingrelay&action=edit&relayid=$relayid");
?>

