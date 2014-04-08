<?php
	if (!defined('IN_WEBADMIN'))
		exit();
      
	if (hmailGetAdminLevel() != ADMIN_SERVER)
		hmailHackingAttemp(); // The user is not server administrator.
  
   $TestType = hmailGetVar("TestType", "");
   $AntiVirusSettings = $obBaseApp->Settings->AntiVirus;
   
   $result = "";
   $message = "";

   switch ($TestType)
   {
	  case "ClamWin":
		$Executable = hmailGetVar("Executable", "");
		$DatabaseFolder = hmailGetVar("DatabaseFolder", "");
		$result = $AntiVirusSettings->TestClamWinScanner($Executable, $DatabaseFolder, $message);
		break;
	  case "ClamAV":
		$Hostname = hmailGetVar("Hostname", "localhost");
		$Port = hmailGetVar("Port", 783);
		$result = $AntiVirusSettings->TestClamAVScanner($Hostname, $Port, $message);
		break;
	  case "External":
		$Executable = hmailGetVar("Executable", "");
		$ReturnValue = hmailGetVar("ReturnValue", 0);
		$result = $AntiVirusSettings->TestCustomerScanner($Executable, $ReturnValue, $message);
		break;
      default:
		die;
   }
     
   echo $result;
?>