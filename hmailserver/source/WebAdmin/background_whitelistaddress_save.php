<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   if (hmailGetAdminLevel() != 2)
   	hmailHackingAttemp(); // Only server admins can change this.
   
   $ID 		= hmailGetVar("ID",0);
   $action	      = hmailGetVar("action","");
   
   $obWhiteListAddresses	= $obBaseApp->Settings()->AntiSpam()->WhiteListAddresses;

   if ($action == "edit")
      $obAddress = $obWhiteListAddresses->ItemByDBID($ID);  
   elseif ($action == "add")
      $obAddress = $obWhiteListAddresses->Add();  
   elseif ($action == "delete")
   {
      $obWhiteListAddresses->DeleteByDBID($ID);  
      header("Location: index.php?page=whitelistaddresses");
      exit();
   }
      
   $LowerIPAddress = hmailGetVar("LowerIPAddress",0);
   $UpperIPAddress = hmailGetVar("UpperIPAddress",0);
   $EmailAddress   = hmailGetVar("EmailAddress",0);
   $Description    = hmailGetVar("Description",0);
   
   if ($LowerIPAddress == "")
      $LowerIPAddress = "0.0.0.0";
   
   if ($UpperIPAddress == "")
      $UpperIPAddress = "255.255.255.255";

   if ($EmailAddress == "")
      $EmailAddress = "*";

   $obAddress->LowerIPAddress  = $LowerIPAddress;
   $obAddress->UpperIPAddress  = $UpperIPAddress;
   $obAddress->EmailAddress    = $EmailAddress;
   $obAddress->Description     = $Description;
   
   $obAddress->Save();
   
   
   
   header("Location: index.php?page=whitelistaddresses");
?>

