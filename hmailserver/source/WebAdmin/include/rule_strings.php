<?php
if (!defined('IN_WEBADMIN'))
   exit();
  
define("eFTUnknown", 0);
define("eFTFrom", 1);
define("eFTTo", 2);
define("eFTCC", 3);
define("eFTSubject", 4);
define("eFTBody", 5);
define("eFTMessageSize", 6);
define("eFTRecipientList", 7);
define("eFTDeliveryAttempts", 8);
   
define("eMTUnknown", 0);
define("eMTEquals", 1);
define("eMTContains", 2);
define("eMTLessThan", 3);
define("eMTGreaterThan", 4);
define("eMTRegExMatch", 5);
define("eMTNotContains", 6);
define("eMTNotEquals", 7);
define("eMTWildcard", 8);

define("eRAUnknown", 0);
define("eRADeleteEmail", 1);
define("eRAForwardEmail", 2);
define("eRAReply", 3);
define("eRAMoveToImapFolder", 4);
define("eRARunScriptFunction", 5);
define("eRAStopRuleProcessing", 6);
define("eRASetHeaderValue", 7);
define("eRASendUsingRoute", 8);
define("eRACreateCopy", 9);
define("eRABindToAddress", 10);
   
function GetPredefinedFieldString($predefinedField)
{
   global $obLanguage;
   
   $result = "";
   
   switch ($predefinedField)
   {
      case eFTBody:
         $result = "Body";
         break;
      case eFTCC:
         $result = "CC";
         break;
      case eFTFrom:
         $result = "From";
         break;
      case eFTMessageSize:
         $result = "Message size";
         break;
      case eFTRecipientList:
         $result = "Recipient list";
         break;
      case eFTSubject:
         $result = "Subject";
         break;
      case eFTTo:
         $result = "To";
         break;
      case eFTUnknown:
         $result = "Unknown";
         break;
      case eFTDeliveryAttempts:
         $result = "Delivery attempts";
         break;
   }

   return $obLanguage->String($result);
}

function GetMatchTypeString($matchType)
{

   global $obLanguage;
   
   $result = 0;
   
   switch ($matchType)
   {
      case eMTContains:
         $result = "Contains";
         break;
      case eMTEquals:
         $result = "Equals";
         break;
      case eMTGreaterThan:
         $result = "Greater than";
         break;
      case eMTLessThan:
         $result = "Less than";
         break;
      case eMTNotContains:
         $result = "Not contains";
         break;
      case eMTNotEquals:
         $result = "Not equals";
         break;
      case eMTRegExMatch:
         $result = "Regular expression";
         break;
      case eMTUnknown:
         $result =  "Unknown";
         break;
      case eMTWildcard:
         $result =  "Wildcard";
         break;
      default:
         $result = "Unknown";
         break;
   }

   return $obLanguage->String($result);
}

function GetRuleActionString($eRuleActionType)
{
   global $obLanguage;

   $result = "";

   switch ($eRuleActionType)
   {
      case eRADeleteEmail:
         $result = "Delete e-mail";
         break;
      case eRAForwardEmail:
         $result = "Forward email";
         break;
      case eRAMoveToImapFolder:
         $result = "Move to IMAP folder";
         break;
      case eRAReply:
         $result = "Reply";
         break;
      case eRARunScriptFunction:
         $result = "Run function";
         break;
      case eRASetHeaderValue:
         $result = "Set header value";
         break;
      case eRAStopRuleProcessing:
         $result = "Stop rule processing";
         break;
      case eRASendUsingRoute:
         $result = "Send using route";
         break;
      case eRACreateCopy:
         $result = "Create and send copy";
		 break;
	  case eRABindToAddress:
         $result = "Bind to local IP address";		
		 break;
      default:
         $result = "Unknown";
         break;
   }

   return $obLanguage->String($result);
}


?>
