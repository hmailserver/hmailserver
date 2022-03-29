<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   $domainid	= hmailGetVar("domainid",0, true);
   $accountid 	= hmailGetVar("accountid",0,true);
   $faid 		= hmailGetVar("faid",0, true);
   $action	   = hmailGetVar("action","");
   
   if (hmailGetAdminLevel() == 0 && ($accountid != hmailGetAccountID() || $domainid != hmailGetDomainID()))
      hmailHackingAttemp();

	if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
		hmailHackingAttemp(); // Domain admin but not for this domain.
	
	$obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);
	$obAccount  = $obDomain->Accounts->ItemByDBID($accountid);  
	$obFetchAccounts = $obAccount->FetchAccounts();

   if ($action == "edit")
      $obFA = $obFetchAccounts->ItemByDBID($faid);  
   elseif ($action == "add")
      $obFA = $obFetchAccounts->Add();  
   elseif ($action == "delete")
   {
      $obFetchAccounts->DeleteByDBID($faid);  
      header("Location: index.php?page=account_externalaccounts&domainid=$domainid&accountid=$accountid");
      exit();
   }
   elseif ($action == "downloadnow")
   {
      $obFA = $obFetchAccounts->ItemByDBID($faid); 
      $obFA->DownloadNow();
      header("Location: index.php?page=account_externalaccounts&domainid=$domainid&accountid=$accountid");
      exit();       
   }
   
   $DaysToKeepMessages      = hmailGetVar("DaysToKeepMessages",0);
   $DaysToKeepMessagesValue = hmailGetVar("DaysToKeepMessagesValue",0);
   
   $obFA->Enabled               = hmailGetVar("Enabled",0);
   $obFA->Name                  = hmailGetVar("Name",0);
   $obFA->MinutesBetweenFetch   = hmailGetVar("MinutesBetweenFetch",0);
   $obFA->Port                  = hmailGetVar("Port",0);
   $obFA->MIMERecipientHeaders  = hmailGetVar("MIMERecipientHeaders","To,CC,X-RCPT-To,X-Envelope-To");
   if (strlen($obFA->MIMERecipientHeaders) > 0)
      $obFA->ProcessMIMERecipients = hmailGetVar("ProcessMIMERecipients",0);
   else
      $obFA->ProcessMIMERecipients = 0;
   $obFA->ProcessMIMEDate       = hmailGetVar("ProcessMIMEDate",0);
   $obFA->ServerAddress         = hmailGetVar("ServerAddress",0);
   $obFA->ServerType            = hmailGetVar("ServerType",0);
   $obFA->Username              = hmailGetVar("Username",0);
   $obFA->UseAntiVirus          = hmailGetVar("UseAntiVirus",0);
   $obFA->UseAntiSpam           = hmailGetVar("UseAntiSpam",0);
   if ($obFA->ProcessMIMERecipients != 0)
      $obFA->EnableRouteRecipients = hmailGetVar("EnableRouteRecipients",0);
   else
      $obFA->EnableRouteRecipients = 0;
   $obFA->ConnectionSecurity 	= hmailGetVar("ConnectionSecurity",0);
   
   if (strlen($DaysToKeepMessages) > 0 && $DaysToKeepMessages <= 0)
      $obFA->DaysToKeepMessages = $DaysToKeepMessages; 
   else 
      $obFA->DaysToKeepMessages = $DaysToKeepMessagesValue; 
   
   $Password = hmailGetVar("Password",0);
   
   if (strlen($Password) > 0)
      $obFA->Password = $Password;
   
   $obFA->Save();
   
   $faid = $obFA->ID;
   
   
   
   header("Location: index.php?page=account_externalaccount&action=edit&domainid=$domainid&accountid=$accountid&faid=$faid");
?>

