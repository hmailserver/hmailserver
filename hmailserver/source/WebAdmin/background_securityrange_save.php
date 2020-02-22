<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   if (hmailGetAdminLevel() != ADMIN_SERVER)
   	hmailHackingAttemp(); // The user is not server administrator.
   
   $action	            = hmailGetVar("action","");
   $securityrangeid	   = hmailGetVar("securityrangeid","");
   
   if ($action == "edit")
      $obSecurityRange     = $obBaseApp->Settings->SecurityRanges->ItemByDBID($securityrangeid);
   elseif ($action == "add")
      $obSecurityRange     = $obBaseApp->Settings->SecurityRanges->Add();
   elseif ($action == "delete")
   {
      $obBaseApp->Settings->SecurityRanges->DeleteByDBID($securityrangeid);
      header("Location: index.php?page=securityranges");
   }
      
   // Fetch form
   $securityrangename		= hmailGetVar("securityrangename","");
   $securityrangepriority	= hmailGetVar("securityrangepriority","0");
   $securityrangelowerip	= hmailGetVar("securityrangelowerip","0");
   $securityrangeupperip	= hmailGetVar("securityrangeupperip","0");
   
   $allowsmtpconnections	= hmailGetVar("allowsmtpconnections","0");
   $allowpop3connections	= hmailGetVar("allowpop3connections","0");
   $allowimapconnections	= hmailGetVar("allowimapconnections","0");
   
   $allowlocaltolocal		= hmailGetVar("allowlocaltolocal","0");
   $allowlocaltoremote		= hmailGetVar("allowlocaltoremote","0");
   $allowremotetolocal		= hmailGetVar("allowremotetolocal","0");
   $allowremotetoremote		= hmailGetVar("allowremotetoremote","0");

   $enablespamprotection	= hmailGetVar("enablespamprotection","0");
   $EnableAntiVirus         = hmailGetVar("EnableAntiVirus","0");
   
   $IsForwardingRelay	   = hmailGetVar("IsForwardingRelay","0");
   $RequireSSLTLSForAuth   = hmailGetVar("RequireSSLTLSForAuth","0");
   
   $Expires	   = hmailGetVar("Expires",0);
   $ExpiresTime	   = hmailGetVar("ExpiresTime","");
   
   // Save the changes
   $obSecurityRange->Name = $securityrangename;
   $obSecurityRange->Priority = $securityrangepriority;
   $obSecurityRange->LowerIP = $securityrangelowerip;
   $obSecurityRange->UpperIP = $securityrangeupperip;
   
   $obSecurityRange->AllowSMTPConnections = $allowsmtpconnections;
   $obSecurityRange->AllowPOP3Connections = $allowpop3connections;
   $obSecurityRange->AllowIMAPConnections = $allowimapconnections;
   
   $obSecurityRange->AllowDeliveryFromLocalToLocal = $allowlocaltolocal;
   $obSecurityRange->AllowDeliveryFromLocalToRemote = $allowlocaltoremote;
   $obSecurityRange->AllowDeliveryFromRemoteToLocal = $allowremotetolocal;
   $obSecurityRange->AllowDeliveryFromRemoteToRemote = $allowremotetoremote;

   $obSecurityRange->RequireSMTPAuthLocalToLocal = hmailGetVar("RequireSMTPAuthLocalToLocal", 0);
   $obSecurityRange->RequireSMTPAuthLocalToExternal = hmailGetVar("RequireSMTPAuthLocalToExternal", 0);
   $obSecurityRange->RequireSMTPAuthExternalToLocal = hmailGetVar("RequireSMTPAuthExternalToLocal", 0);
   $obSecurityRange->RequireSMTPAuthExternalToExternal = hmailGetVar("RequireSMTPAuthExternalToExternal", 0);

   $obSecurityRange->EnableSpamProtection = $enablespamprotection;
   $obSecurityRange->EnableAntiVirus = $EnableAntiVirus;
   $obSecurityRange->IsForwardingRelay = $IsForwardingRelay;
   $obSecurityRange->RequireSSLTLSForAuth = $RequireSSLTLSForAuth;
   
   $obSecurityRange->Expires = $Expires;
   $obSecurityRange->ExpiresTime = $ExpiresTime;

   $obSecurityRange->Save();
   
   $securityrangeid = $obSecurityRange->ID;
   
   header("Location: index.php?page=securityrange&action=edit&securityrangeid=$securityrangeid");
?>

