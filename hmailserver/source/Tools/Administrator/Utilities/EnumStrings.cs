// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace hMailServer.Administrator.Utilities
{
   class EnumStrings
   {
      public static string hMailServerAdministrator
      {
         get
         {
            return "hMailServer Administrator";
         }
      }

      public static string GetStateString(hMailServer.eServerState state)
      {
         string result;
         switch (state)
         {
            case eServerState.hStateRunning:
               result = "Running";
               break;
            case eServerState.hStateStarting:
               result = "Starting";
               break;
            case eServerState.hStateStopped:
               result = "Stopped";
               break;
            case eServerState.hStateStopping:
               result = "Stopping";
               break;
            default:
               result = "Unknown";
               break;
         }

         return Strings.Localize(result);
      }

      public static string GetYesNoString(bool Yes)
      {
         if (Yes)
            return Strings.Localize("Yes");
         else
            return Strings.Localize("No");
      }

      public static string GetMatchTypeString(eRuleMatchType mt)
      {
         string result;
         switch (mt)
         {
            case eRuleMatchType.eMTContains:
               result = "Contains";
               break;
            case eRuleMatchType.eMTEquals:
               result = "Equals";
               break;
            case eRuleMatchType.eMTGreaterThan:
               result = "Greater than";
               break;
            case eRuleMatchType.eMTLessThan:
               result = "Less than";
               break;
            case eRuleMatchType.eMTNotContains:
               result = "Not contains";
               break;
            case eRuleMatchType.eMTNotEquals:
               result = "Not equals";
               break;
            case eRuleMatchType.eMTRegExMatch:
               result = "Regular expression";
               break;
            case eRuleMatchType.eMTUnknown:
               result =  "Unknown";
               break;
            case eRuleMatchType.eMTWildcard:
               result =  "Wildcard";
               break;
            default:
               result = "Unknown";
               break;
         }

         return Strings.Localize(result);
      }

      public static string GetPredefinedFieldString(eRulePredefinedField pf)
      {
         string result = "";
         switch (pf)
         {
            case eRulePredefinedField.eFTBody:
               result = "Body";
               break;
            case eRulePredefinedField.eFTCC:
               result = "CC";
               break;
            case eRulePredefinedField.eFTFrom:
               result = "From";
               break;
            case eRulePredefinedField.eFTMessageSize:
               result = "Message size";
               break;
            case eRulePredefinedField.eFTRecipientList:
               result = "Recipient list";
               break;
            case eRulePredefinedField.eFTSubject:
               result = "Subject";
               break;
            case eRulePredefinedField.eFTTo:
               result = "To";
               break;
            case eRulePredefinedField.eFTUnknown:
               result = "Unknown";
               break;
            case eRulePredefinedField.eFTDeliveryAttempts:
               result = "Delivery attempts";
               break;
         }

         return Strings.Localize(result);
      }

      public static string GetRuleActionString(eRuleActionType ra)
      {
         string result;

         switch (ra)
         {
            case eRuleActionType.eRADeleteEmail:
               result = "Delete e-mail";
               break;
            case eRuleActionType.eRAForwardEmail:
               result = "Forward email";
               break;
            case eRuleActionType.eRAMoveToImapFolder:
               result = "Move to IMAP folder";
               break;
            case eRuleActionType.eRAReply:
               result = "Reply";
               break;
            case eRuleActionType.eRARunScriptFunction:
               result = "Run function";
               break;
            case eRuleActionType.eRASetHeaderValue:
               result = "Set header value";
               break;
            case eRuleActionType.eRAStopRuleProcessing:
               result = "Stop rule processing";
               break;
            case eRuleActionType.eRASendUsingRoute:
               result = "Send using route";
               break;
            case eRuleActionType.eRACreateCopy:
               result = "Create and send copy";
               break;
            case eRuleActionType.eRABindToAddress:
               result = "Bind to local IP address";
               break;
            default:
               result = "Unknown";
               break;
         }

         return Strings.Localize(result);
      }

      public static string GetPermissionTypeString(eACLPermissionType type)
      {
         string result;
         switch (type)
         {
            case eACLPermissionType.ePermissionTypeAnyone:
               result = "Anyone";
               break;
            case eACLPermissionType.ePermissionTypeGroup:
               result = "Group";
               break;
            case eACLPermissionType.ePermissionTypeUser:
               result = "User";
               break;
            default:
               result = "Unknown";
               break;
         }

         return Strings.Localize(result);
      }

   }
}
