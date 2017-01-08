<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   if (hmailGetAdminLevel() != ADMIN_SERVER)
   	hmailHackingAttemp(); // Only server can change these settings.      
      
   $domainid	= hmailGetVar("domainid",0,true);
   $aliasid	   = hmailGetVar("aliasid",0);
   $action	   = hmailGetVar("action","");
   $aliasname  = hmailGetVar("aliasname","");

   $obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);
    
   if ($action == "add")
   {
      $alias =  $obDomain->DomainAliases->Add();
      $alias->AliasName = $aliasname;
      $alias->Save();
   }
   elseif ($action == "delete")
   {
      $obDomain->DomainAliases->DeleteByDBID($aliasid);
   }
   
   header("Location: index.php?page=domain&action=edit&domainid=$domainid");
?>

