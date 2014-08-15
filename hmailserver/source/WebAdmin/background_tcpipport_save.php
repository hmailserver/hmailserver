<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   if (hmailGetAdminLevel() != 2)
      hmailHackingAttemp(); // Server admin required

   $tcpipportid 	= hmailGetVar("tcpipportid",0);
   $protocol	   = hmailGetVar("protocol",0);
   $portnumber	   = hmailGetVar("portnumber",0);
   $action	      = hmailGetVar("action","");
   $ConnectionSecurity	      = hmailGetVar("ConnectionSecurity","0");
   $SSLCertificateID	      = hmailGetVar("SSLCertificateID","0");
   
   $obSettings   = $obBaseApp->Settings();
   $obTCPIPPorts  = $obSettings->TCPIPPorts;

   if ($action == "edit")
      $obTCPIPPort = $obTCPIPPorts->ItemByDBID($tcpipportid);
   elseif ($action == "add")
      $obTCPIPPort = $obTCPIPPorts->Add();
   elseif ($action == "delete")
   {
   	  $obTCPIPPorts->DeleteByDBID($tcpipportid);
      header("Location: index.php?page=tcpipports");
      exit();
   }

   $obTCPIPPort->Protocol = $protocol;
   $obTCPIPPort->PortNumber = $portnumber;
   $obTCPIPPort->ConnectionSecurity = $ConnectionSecurity;
   $obTCPIPPort->SSLCertificateID = $SSLCertificateID;
   $obTCPIPPort->Address = hmailGetVar("Address","0");
   
   $obTCPIPPort->Save();
   
   $obBaseApp->Stop();
   $obBaseApp->Start();
   
   $tcpipportid = $obTCPIPPort->ID;
   
   header("Location: index.php?page=tcpipport&action=edit&tcpipportid=$tcpipportid");

?>

