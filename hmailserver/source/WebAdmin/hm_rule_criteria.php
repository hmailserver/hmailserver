<?php
if (!defined('IN_WEBADMIN'))
   exit();

$action = hmailGetVar("action", "");   
$domainid = hmailGetVar("domainid", 0, true);
$accountid = hmailGetVar("accountid", 0, true);
$ruleid = hmailGetVar("ruleid", 0);
$criteriaid = hmailGetVar("criteriaid", 0);

if (!GetHasRuleAccess($domainid, $accountid))
   hmailHackingAttemp(); // The user is not server administrator
   
include "include/rule_strings.php";  
  
if ($domainid == 0)
   $rule = $obBaseApp->Rules->ItemByDBID($ruleid);
else
   $rule = $obBaseApp->Domains->ItemByDBID($domainid)->Accounts->ItemByDBID($accountid)->Rules->ItemByDBID($ruleid);
  
if ($action == "edit")
{  
   $ruleCriteria = $rule->Criterias->ItemByDBID($criteriaid);
   
   $UsePredefined = $ruleCriteria->UsePredefined;
   $PredefinedField = $ruleCriteria->PredefinedField;
   $MatchType = $ruleCriteria->MatchType;
   $MatchValue = $ruleCriteria->MatchValue;
   $HeaderField = $ruleCriteria->HeaderField;
}
else
{
   $UsePredefined = 1;
   $PredefinedField = 0;
   $MatchType = 0;
   $MatchValue = "";
   $HeaderField = "";
}  
?>

<h1><?php EchoTranslation("Criteria")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_rule_save");
      PrintHidden("savetype", "criteria");
      PrintHidden("action", $action);
      PrintHidden("domainid", $domainid);
      PrintHidden("accountid", $accountid);
      PrintHidden("ruleid", $ruleid);
      PrintHidden("criteriaid", $criteriaid);
   ?>      
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>         
   
      	<table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>            
      		<tr>
               <td>
                   <input type="radio" name="UsePredefined" value="1" <?php if ($UsePredefined == 1) echo "checked"?>> <?php EchoTranslation("Predefined field")?>:<br/>
                   <select name="PredefinedField">
                     <?php
                         
                        $eFTFromSelected = $PredefinedField == eFTFrom ? "selected" : "";
                        $eFTToSelected = $PredefinedField == eFTTo ? "selected" : "";
                        $eFTCCSelected = $PredefinedField == eFTCC ? "selected" : "";
                        $eFTSubjectSelected = $PredefinedField == eFTSubject ? "selected" : "";
                        $eFTBodySelected = $PredefinedField == eFTBody ? "selected" : "";
                        $eFTMessageSizeSelected = $PredefinedField == eFTMessageSize ? "selected" : "";
                        $eFTRecipientList = $PredefinedField == eFTRecipientList ? "selected" : "";
                        $eFTDeliveryAttempts = $PredefinedField == eFTDeliveryAttempts ? "selected" : "";
                     
                        echo "<option value= " . eFTFrom . " $eFTFromSelected>" . GetPredefinedFieldString(eFTFrom) .  "</option>";
                        echo "<option value= " . eFTTo . " $eFTToSelected>" . GetPredefinedFieldString(eFTTo) .  "</option>";
                        echo "<option value= " . eFTCC . " $eFTCCSelected>" . GetPredefinedFieldString(eFTCC) .  "</option>";
                        echo "<option value= " . eFTSubject . " $eFTSubjectSelected>" . GetPredefinedFieldString(eFTSubject) .  "</option>";
                        echo "<option value= " . eFTBody . " $eFTBodySelected>" . GetPredefinedFieldString(eFTBody) .  "</option>";
                        echo "<option value= " . eFTMessageSize . " $eFTMessageSizeSelected>" . GetPredefinedFieldString(eFTMessageSize) .  "</option>";
                        echo "<option value= " . eFTRecipientList . " $eFTRecipientList>" . GetPredefinedFieldString(eFTRecipientList) .  "</option>";
                        echo "<option value= " . eFTDeliveryAttempts . " $eFTDeliveryAttempts>" . GetPredefinedFieldString(eFTDeliveryAttempts) .  "</option>";
                     ?>
                     
                   </select>
               </td>
               <td>
                  <input type="radio" name="UsePredefined" value="0" <?php if ($UsePredefined == 0) echo "checked"?>> <?php EchoTranslation("Custom header field")?>:<br/>
                  <input type="text" name="HeaderField" value="<?php echo PreprocessOutput($HeaderField);?>">
               </td>
      		</tr>
	
      		<tr>
               <td>
                  <?php EchoTranslation("Search type")?><br/>
                   <select name="MatchType">
                     <?php
                         
                        $eMTEqualsSelected = $MatchType == eMTEquals ? "selected" : "";
                        $eMTContainsSelected = $MatchType == eMTContains ? "selected" : "";
                        $eMTLessThanSelected = $MatchType == eMTLessThan ? "selected" : "";
                        $eMTGreaterThanSelected = $MatchType == eMTGreaterThan ? "selected" : "";
                        $eMTRegExMatchSelected = $MatchType == eMTRegExMatch ? "selected" : "";
                        $eMTNotContainsSelected = $MatchType == eMTNotContains ? "selected" : "";
                        $eMTNotEqualsSelected = $MatchType == eMTNotEquals ? "selected" : "";
                        $eMTWildcardSelected = $MatchType == eMTWildcard ? "selected" : "";
                     
                        echo "<option value= " . eMTEquals . " $eMTEqualsSelected>" . GetMatchTypeString(eMTEquals) .  "</option>";
                        echo "<option value= " . eMTContains . " $eMTContainsSelected>" . GetMatchTypeString(eMTContains) .  "</option>";
                        echo "<option value= " . eMTLessThan . " $eMTLessThanSelected>" . GetMatchTypeString(eMTLessThan) .  "</option>";
                        echo "<option value= " . eMTGreaterThan . " $eMTGreaterThanSelected>" . GetMatchTypeString(eMTGreaterThan) .  "</option>";
                        echo "<option value= " . eMTRegExMatch . " $eMTRegExMatchSelected>" . GetMatchTypeString(eMTRegExMatch) .  "</option>";
                        echo "<option value= " . eMTNotContains . " $eMTNotContainsSelected>" . GetMatchTypeString(eMTNotContains) .  "</option>";
                        echo "<option value= " . eMTNotEquals . " $eMTNotEqualsSelected>" . GetMatchTypeString(eMTNotEquals) .  "</option>";
                        echo "<option value= " . eMTWildcard . " $eMTWildcardSelected>" . GetMatchTypeString(eMTWildcard) .  "</option>";
                     ?>
                     
                   </select>
               </td>
               <td>
                  <?php EchoTranslation("Value")?><br/>
                  <input type="text" name="MatchValue" value="<?php echo PreprocessOutput($MatchValue);?>">
               </td>
      		</tr>   
   
         </table>
       </div>
    </div>  
   <?php 
      PrintSaveButton();
   ?>
</form>
