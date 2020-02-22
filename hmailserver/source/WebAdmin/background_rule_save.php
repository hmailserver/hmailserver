<?php

   if (!defined('IN_WEBADMIN'))
      exit();

   $action	   = hmailGetVar("action","");
   $domainid   = hmailGetVar("domainid", 0, true);
   $accountid  = hmailGetVar("accountid", 0, true);
   $ruleid     = hmailGetVar("ruleid", 0);
   $criteriaid = hmailGetVar("criteriaid", 0);
   $actionid   = hmailGetVar("actionid", 0);
   $savetype   = hmailGetVar("savetype", 0);
      
   if (!GetHasRuleAccess($domainid, $accountid))
   	hmailHackingAttemp();

   include "include/rule_strings.php";
      
   $rule_link = "index.php?page=rule&action=edit&domainid=$domainid&accountid=$accountid&ruleid=$ruleid";
   
   if ($action == "add" && $savetype == "rule")
   {
      if ($domainid == 0)
         $rule = $obBaseApp->Rules->Add();
      else
         $rule = $obBaseApp->Domains->ItemByDBID($domainid)->Accounts->ItemByDBID($accountid)->Rules->Add();
   }
   else
   {
      if ($domainid == 0)
         $rule = $obBaseApp->Rules->ItemByDBID($ruleid);
      else
         $rule = $obBaseApp->Domains->ItemByDBID($domainid)->Accounts->ItemByDBID($accountid)->Rules->ItemByDBID($ruleid);
   }  
 

   if ($action == "delete")
   {
   
      if ($savetype == "criteria")
         $rule->Criterias->ItemByDBID($criteriaid)->Delete();
      else if ($savetype == "action")
         $rule->Actions->ItemByDBID($actionid)->Delete();
      else if ($savetype == "rule")
         $rule->Delete();
      
      if ($savetype == "criteria" || $savetype == "action")
         header("Location: $rule_link");
      else
      {
         if ($domainid == 0)
            header("Location: index.php?page=rules");
         else
            header("Location: index.php?page=account&action=edit&accountid=$accountid&domainid=$domainid");
      }
         
      die;
   }
   
   if ($savetype == "criteria")
   {
      
      if ($action == "edit")
         $criteria = $rule->Criterias->ItemByDBID($criteriaid);
      else if ($action == "add")
      {
         $criteria = $rule->Criterias->Add();
      }
   
      $criteria->UsePredefined = hmailGetVar("UsePredefined", 0);
      $criteria->PredefinedField = hmailGetVar("PredefinedField", 0);
      $criteria->MatchType = hmailGetVar("MatchType", 0);
      $criteria->MatchValue = hmailGetVar("MatchValue", 0);
      $criteria->HeaderField = hmailGetVar("HeaderField", 0);
      
      $criteria->Save();
      
      $rule->Save();
    
      header("Location: $rule_link");
      die;
   }
   else if ($savetype == "action")
   {
   
      if ($action == "edit")
         $actionObj = $rule->Actions->ItemByDBID($actionid);
      else if ($action == "add")
         $actionObj = $rule->Actions->Add();
   
      $type = hmailGetVar("Type", 0);
      
      if (hmailGetAdminLevel() != ADMIN_SERVER)
      {
         if ($type != eRADeleteEmail && 
             $type != eRAForwardEmail &&
             $type != eRAReply &&
             $type != eRAMoveToImapFolder &&
             $type != eRAStopRuleProcessing &&
             $type != eRASetHeaderValue)
         {
            hmailHackingAttemp();
         }  
      }
   
      $actionObj->To = hmailGetVar("To", "");
      $actionObj->IMAPFolder = hmailGetVar("IMAPFolder", "");
      $actionObj->ScriptFunction = hmailGetVar("ScriptFunction", "");
      $actionObj->FromName = hmailGetVar("FromName", "");
      $actionObj->FromAddress = hmailGetVar("FromAddress", "");
      $actionObj->Subject = hmailGetVar("Subject", "");
      $actionObj->Body = hmailGetVar("Body", "");
      $actionObj->HeaderName = hmailGetVar("HeaderName", "");
      
	  switch ($type)
	  {
	    case eRASetHeaderValue:
			$actionObj->Value = hmailGetVar("Value", "");
			break;
		case eRABindToAddress:
			$actionObj->Value = hmailGetVar("BindToAddress", "");
			break;
	  }
      
	  $actionObj->Type = $type;

      $actionObj->Save();
      
      $rule->Save();
      
      header("Location: $rule_link");   
      die;
   }
   else if ($savetype == "rule")
   {
      $rule->Name = hmailGetVar("Name", "");
      $rule->Active = hmailGetVar("Active", "") == "1";
      $rule->UseAND = hmailGetVar("UseAND", "") == "1";
      $rule->Save();
      
      $ruleid = $rule->ID;
      
      // can't re-use rule_link since the rule id might be new (if add)
      header("Location: index.php?page=rule&action=edit&domainid=$domainid&accountid=$accountid&ruleid=$ruleid");   
      die;
   }

   
?>

