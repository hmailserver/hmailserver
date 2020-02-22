<?php

   if (!defined('IN_WEBADMIN'))
      exit();

   if (hmailGetAdminLevel() != 2)
   	hmailHackingAttemp(); // Domain admin but not for this domain.
   
   $action	            = hmailGetVar("action","");
   $routeid	   = hmailGetVar("routeid","");
   
   if ($action == "edit")
      $obRoute     = $obBaseApp->Settings->Routes->ItemByDBID($routeid);
   elseif ($action == "add")
      $obRoute    = $obBaseApp->Settings->Routes->Add();
   elseif ($action == "delete")
   {
      $obBaseApp->Settings->Routes->DeleteByDBID($routeid);
      header("Location: index.php?page=routes");
      exit();
   }
   
   
   $routedomainname  = hmailGetVar("routedomainname","");
   $routetargetsmtphost   = hmailGetVar("routetargetsmtphost","0");
   $routetargetsmtpport   = hmailGetVar("routetargetsmtpport","0");
   $TreatSenderAsLocalDomain   = hmailGetVar("TreatSenderAsLocalDomain","0");
   $TreatRecipientAsLocalDomain   = hmailGetVar("TreatRecipientAsLocalDomain","0");
   
   $routenumberoftries        = hmailGetVar("routenumberoftries","0");
   $routemminutesbetweentry   = hmailGetVar("routemminutesbetweentry","0");
   $routerequiresauth   = hmailGetVar("routerequiresauth","0");
   $routeauthusername   = hmailGetVar("routeauthusername","0");
   $routeauthpassword   = hmailGetVar("routeauthpassword","0");
   $ConnectionSecurity   = hmailGetVar("ConnectionSecurity","0");
   
   $obRoute->DomainName = $routedomainname;
   $obRoute->TargetSMTPHost = $routetargetsmtphost;
   $obRoute->TargetSMTPPort = $routetargetsmtpport;
   $obRoute->TreatSenderAsLocalDomain = $TreatSenderAsLocalDomain;
   $obRoute->TreatRecipientAsLocalDomain = $TreatRecipientAsLocalDomain;
   
   $obRoute->NumberOfTries = $routenumberoftries;
   $obRoute->MinutesBetweenTry = $routemminutesbetweentry;
   $obRoute->RelayerRequiresAuth = $routerequiresauth;
   $obRoute->RelayerAuthUsername = $routeauthusername;
   
   $obRoute->AllAddresses = hmailGetVar("AllAddresses","0");
   
   $obRoute->ConnectionSecurity = $ConnectionSecurity;
   
   if ($routeauthpassword != "")
      $obRoute->SetRelayerAuthPassword($routeauthpassword);

   $obRoute->Save();
   
   $routeid = $obRoute->ID;
   
   header("Location: index.php?page=route&action=edit&routeid=$routeid");
?>

