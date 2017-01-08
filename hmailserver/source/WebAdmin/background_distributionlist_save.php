<?php

   if (!defined('IN_WEBADMIN'))
      exit();
   
   $domainid	= hmailGetVar("domainid",0,true);
   $distributionlistid 	= hmailGetVar("distributionlistid",0);
   $action	   = hmailGetVar("action","");
   
   if (hmailGetAdminLevel() == 0)
      hmailHackingAttemp();
   
   if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
   	hmailHackingAttemp(); // Domain admin but not for this domain.

   $obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);

   if ($action == "add")
   {
      $result = IsAddAllowed($obDomain);
      
      if ($result > 0)
      {
         header("Location: index.php?page=distributionlist&action=$action&domainid=$domainid&distributionlistid=$distributionlistid&error_message=$result");  
         exit();        
      } 
      
   }    
   
   if ($action == "edit")
      $obList = $obDomain->DistributionLists->ItemByDBID($distributionlistid);  
   elseif ($action == "add")
      $obList = $obDomain->DistributionLists->Add();  
   elseif ($action == "delete")
   {
      $obDomain->DistributionLists->DeleteByDBID($distributionlistid);  
      header("Location: index.php?page=distributionlists&domainid=$domainid");
      exit();
   }
   
   $domainname = $obDomain->Name;
   	
   $listaddress  = hmailGetVar("listaddress","");
   $listactive   = hmailGetVar("listactive","0");
   $listrequiresmtpauth  = hmailGetVar("listrequiresmtpauth","0");
   $RequireSenderAddress  = hmailGetVar("RequireSenderAddress","");
   $Mode  = hmailGetVar("Mode","");
   
   
   $obList->Address = $listaddress . "@" . $domainname;
   $obList->RequireSMTPAuth = $listrequiresmtpauth;
   $obList->Active = $listactive;
   $obList->RequireSenderAddress = $RequireSenderAddress;
   $obList->Mode = $Mode;
   
   try
   {
      $obList->Save();
   }
   catch(Exception $exception)
   {
      ExceptionHandler($exception);
      die;
   }
   
   $distributionlistid = $obList->ID;
   
   header("Location: index.php?page=distributionlist&action=edit&domainid=$domainid&distributionlistid=$distributionlistid");
   
   function IsAddAllowed($obDomain)
   {
      if (!$obDomain->MaxNumberOfDistributionListsEnabled)
         return 0;
      
      if ($obDomain->DistributionLists->Count >= $obDomain->MaxNumberOfDistributionLists)
         return STR_DISTRIUBTIONLIST_COULD_NOT_BE_ADDED_MAX_REACHED;
        
      return 0;
   }   
?>

