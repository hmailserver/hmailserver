<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   $domainid	= hmailGetVar("domainid",0,true);
   $action	   = hmailGetVar("action","");
   $domainname     = hmailGetVar("domainname","");
   $domainactive   = hmailGetVar("domainactive","0");
   $domainpostmaster   =   hmailGetVar("domainpostmaster","");
   $domainmaxsize   = hmailGetVar("domainmaxsize","0");
   $domainmaxmessagesize   = hmailGetVar("domainmaxmessagesize","0");
   $domainplusaddressingenabled = hmailGetVar("domainplusaddressingenabled","0");
   $domainplusaddressingcharacter = hmailGetVar("domainplusaddressingcharacter","+");
   $domainantispamenablegreylisting = hmailGetVar("domainantispamenablegreylisting","0");
   
   $SignatureEnabled   = hmailGetVar("SignatureEnabled","0");
   $SignatureHTML  	  = hmailGetVar("SignatureHTML","");
   $SignaturePlainText = hmailGetVar("SignaturePlainText","");
   $SignatureMethod    = hmailGetVar("SignatureMethod","1");
   
   $AddSignaturesToLocalMail = hmailGetVar("AddSignaturesToLocalMail","0");
   $AddSignaturesToReplies   = hmailGetVar("AddSignaturesToReplies","0");
   
   $MaxAccountSize       = hmailGetVar("MaxAccountSize","0");
   
   $MaxNumberOfAccounts            = hmailGetVar("MaxNumberOfAccounts","0");
   $MaxNumberOfAliases             = hmailGetVar("MaxNumberOfAliases","0");
   $MaxNumberOfDistributionLists   = hmailGetVar("MaxNumberOfDistributionLists","0");
   
   $MaxNumberOfAccountsEnabled          = hmailGetVar("MaxNumberOfAccountsEnabled","0");
   $MaxNumberOfAliasesEnabled           = hmailGetVar("MaxNumberOfAliasesEnabled","0");
   $MaxNumberOfDistributionListsEnabled = hmailGetVar("MaxNumberOfDistributionListsEnabled","0");
   
   $DKIMSignEnabled = hmailGetVar("DKIMSignEnabled", "0");
   $DKIMSignAliasesEnabled = hmailGetVar("DKIMSignAliasesEnabled", "0");
   $DKIMPrivateKeyFile = hmailGetVar("DKIMPrivateKeyFile", "");
   $DKIMSelector = hmailGetVar("DKIMSelector", "");
   
   $DKIMHeaderCanonicalizationMethod = hmailGetVar("DKIMHeaderCanonicalizationMethod", "2");
   $DKIMBodyCanonicalizationMethod = hmailGetVar("DKIMBodyCanonicalizationMethod", "2");
   $DKIMSigningAlgorithm = hmailGetVar("DKIMSigningAlgorithm", "2");
   
   if ($domainactive == "")
      $domainactive = 0;
   
   if (hmailGetAdminLevel() == 1 && ($domainid != hmailGetDomainID() || $action != "edit"))
   	hmailHackingAttemp(); // Domain admin but not for this domain.   

   if ($action == "edit")   
      $obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);
   elseif ($action == "add")
      $obDomain	= $obBaseApp->Domains->Add();
   elseif ($action == "delete")
   {
      $obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);
      $obDomain->Delete();
      
      header("Location: index.php?page=domains");
      exit();
      
   }
      
   $obDomain->Postmaster = $domainpostmaster;
   
   $obDomain->PlusAddressingEnabled = $domainplusaddressingenabled == "1";
   $obDomain->PlusAddressingCharacter = $domainplusaddressingcharacter;
   $obDomain->AntiSpamEnableGreylisting = $domainantispamenablegreylisting == "1";
   
   $obDomain->SignatureEnabled   = $SignatureEnabled == "1";
   $obDomain->SignaturePlainText = $SignaturePlainText;
   $obDomain->SignatureHTML      = $SignatureHTML;
   $obDomain->SignatureMethod    = $SignatureMethod;
      
   $obDomain->AddSignaturesToLocalMail = $AddSignaturesToLocalMail;
   $obDomain->AddSignaturesToReplies   = $AddSignaturesToReplies;
   
   $obDomain->DKIMSignEnabled = $DKIMSignEnabled;
   if ($obDomain->DomainAliases->Count > 0){
      $obDomain->DKIMSignAliasesEnabled = $DKIMSignAliasesEnabled;
   }
   else {
      $obDomain->DKIMSignAliasesEnabled = 0;
   }
   $obDomain->DKIMPrivateKeyFile = $DKIMPrivateKeyFile;
   $obDomain->DKIMSelector = $DKIMSelector;
   $obDomain->DKIMHeaderCanonicalizationMethod = $DKIMHeaderCanonicalizationMethod;
   $obDomain->DKIMBodyCanonicalizationMethod = $DKIMBodyCanonicalizationMethod;
   $obDomain->DKIMSigningAlgorithm = $DKIMSigningAlgorithm;
   
   if (hmailGetAdminLevel() == 2)
   {
      // Save other properties
      $obDomain->Active = $domainactive;
      $obDomain->Name = $domainname;
      $obDomain->MaxSize = $domainmaxsize;
      $obDomain->MaxMessageSize = $domainmaxmessagesize;
      $obDomain->MaxAccountSize      = $MaxAccountSize;
      
      $obDomain->MaxNumberOfAccounts = $MaxNumberOfAccounts;
      $obDomain->MaxNumberOfAliases  = $MaxNumberOfAliases;
      $obDomain->MaxNumberOfDistributionLists = $MaxNumberOfDistributionLists;

      $obDomain->MaxNumberOfAccountsEnabled = $MaxNumberOfAccountsEnabled;
      $obDomain->MaxNumberOfAliasesEnabled  = $MaxNumberOfAliasesEnabled;
      $obDomain->MaxNumberOfDistributionListsEnabled = $MaxNumberOfDistributionListsEnabled;
   }

   $obDomain->Save();
   $domainid = $obDomain->ID;
   
   header("Location: index.php?page=domain&action=edit&domainid=$domainid");
?>

