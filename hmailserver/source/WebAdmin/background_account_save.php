<?php

   if (!defined('IN_WEBADMIN'))
      exit();

   $domainid	= hmailGetVar("domainid",0,true);
   $accountid	= hmailGetVar("accountid",0,true);
   $action	   = hmailGetVar("action","");
   
   $obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);
   
   if (hmailGetAdminLevel() == 0 && ($accountid != hmailGetAccountID() || $action != "edit"))
      hmailHackingAttemp();
   
   if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
   	hmailHackingAttemp(); // Domain admin but not for this domain.
   	
   $accountpassword  = hmailGetVar("accountpassword","");
   $accountmaxsize   = hmailGetVar("accountmaxsize","0");
   $accountaddress   = hmailGetVar("accountaddress","") . "@". $obDomain->Name;
   $accountactive    = hmailGetVar("accountactive","0");
   $accountadminlevel  = hmailGetVar("accountadminlevel","0");
   $PersonFirstName  = hmailGetVar("PersonFirstName","0");
   $PersonLastName   = hmailGetVar("PersonLastName","0");
   
   $vacationmessageon  = hmailGetVar("vacationmessageon","");
   $vacationsubject   = hmailGetVar("vacationsubject","0");
   $vacationmessage   =   hmailGetVar("vacationmessage","");
   $vacationmessageexpires   =   hmailGetVar("vacationmessageexpires","0");
   $vacationmessageexpiresdate   =   hmailGetVar("vacationmessageexpiresdate","2001-01-01");
   
   $forwardenabled  = hmailGetVar("forwardenabled","0");
   $forwardaddress   = hmailGetVar("forwardaddress","");
   $forwardkeeporiginal   =   hmailGetVar("forwardkeeporiginal","0");
   
   $adenabled   = hmailGetVar("adenabled","");
   $addomain    = hmailGetVar("addomain","0");
   $adusername  =   hmailGetVar("adusername","");
  
   $SignatureEnabled     = hmailGetVar("SignatureEnabled","0");
   $SignatureHTML        = hmailGetVar("SignatureHTML","");
   $SignaturePlainText   =   hmailGetVar("SignaturePlainText","0");

  
   if ($action == "edit")
      $obAccount = $obDomain->Accounts->ItemByDBID($accountid);  
   elseif ($action == "add")
      $obAccount = $obDomain->Accounts->Add();  
   elseif ($action == "delete")
   {
      $obAccount = $obDomain->Accounts->DeleteByDBID($accountid);  
      header("Location: index.php?page=accounts&domainid=$domainid");
      exit();
   }
  
   // If this is the current user, we need to update the session password.
   if ($action == "edit" &&
       $accountid == hmailGetAccountID())
   {
      if ($accountpassword != "")
         $_SESSION['session_password'] = $accountpassword;  
   }
   
   if ($accountpassword != "")
      $obAccount->Password = "$accountpassword";
   
   $obAccount->PersonFirstName = $PersonFirstName;
   $obAccount->PersonLastName = $PersonLastName;
   
   $obAccount->VacationMessageIsOn = $vacationmessageon == "1";
   $obAccount->VacationSubject     = $vacationsubject;
   $obAccount->VacationMessage     = $vacationmessage;
   $obAccount->VacationMessageExpires      = $vacationmessageexpires;
   $obAccount->VacationMessageExpiresDate  = $vacationmessageexpiresdate;

   $obAccount->ForwardEnabled		= $forwardenabled == "1";
   $obAccount->ForwardAddress		= $forwardaddress;
   $obAccount->ForwardKeepOriginal	= $forwardkeeporiginal == "1";

   $obAccount->SignatureEnabled		= $SignatureEnabled == "1";
   $obAccount->SignatureHTML		   = $SignatureHTML;
   $obAccount->SignaturePlainText	= $SignaturePlainText;
     
   
   if (hmailGetAdminLevel() != ADMIN_USER)
   {
      $accountmaxsize = str_replace(".", ",", $accountmaxsize);

      // Save other properties
      $obAccount->Address = $accountaddress;
      $obAccount->MaxSize = $accountmaxsize;
      $obAccount->Active  = $accountactive;
      
      $obAccount->IsAD         = $adenabled == "1";
      $obAccount->ADDomain     = $addomain;
      $obAccount->ADUsername   = $adusername;   
      
      if (hmailGetAdminLevel() == 1)
      {
         // The web user is domain administrator. Don't allow him
         // to change the user to server admin, unless he already
         // is this.
         
         if ($accountadminlevel == 0 || $accountadminlevel == 1)
         {
            $obAccount->AdminLevel = $accountadminlevel;
         }
      }
      else if (hmailGetAdminLevel() == 2)
      {
         // The web user is server administrator. Allow any change
         $obAccount->AdminLevel = $accountadminlevel;
      }
   }
   
   
   $obAccount->Save();
   $accountid = $obAccount->ID;
   
   header("Location: index.php?page=account&action=edit&domainid=$domainid&accountid=$accountid");
   

?>

