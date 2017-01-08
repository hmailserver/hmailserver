<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   $distributionlistid 	= hmailGetVar("distributionlistid",0);
   $recipientid	      = hmailGetVar("recipientid",0);
   $domainid	         = hmailGetVar("domainid",0,true);
   $action	            = hmailGetVar("action","");
   $recipientaddress    = hmailGetVar("recipientaddress","");
   
   if (hmailGetAdminLevel() == 0)
      hmailHackingAttemp();
   
   if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
   	hmailHackingAttemp(); // Domain admin but not for this domain.

   $obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);
   $obList = $obDomain->DistributionLists->ItemByDBID($distributionlistid);

   if ($action == "edit")
      $obRecipient = $obList->Recipients->ItemByDBID($recipientid);
   elseif ($action == "add")
      $obRecipient = $obList->Recipients->Add();
   elseif ($action == "delete")
   {
      $obRecipient = $obList->Recipients->ItemByDBID($recipientid);
      $obRecipient->Delete();
      
      header("Location: index.php?page=distributionlist_recipients&domainid=$domainid&distributionlistid=$distributionlistid");
      exit();
      
   }
   
   $obRecipient->RecipientAddress = $recipientaddress;
   $obRecipient->Save();
   
   $recipientid = $obRecipient->ID;
   
   header("Location: index.php?page=distributionlist_recipient&action=edit&domainid=$domainid&distributionlistid=$distributionlistid&recipientid=$recipientid");

?>

