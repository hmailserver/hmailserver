<?php
if (!defined('IN_WEBADMIN'))
   exit();

$action = hmailGetVar("action", "");   
$domainid = hmailGetVar("domainid", 0, true);
$accountid = hmailGetVar("accountid", 0, true);
$ruleid = hmailGetVar("ruleid", 0, true);
   
// check permissions
if (!GetHasRuleAccess($domainid, $accountid))
	hmailHackingAttemp(); // The user has no rule editing permissions.

include "include/rule_strings.php";

if ($ruleid != 0)
{
   if ($domainid != 0)
   {
      $domain = $obBaseApp->Domains->ItemByDBID($domainid);
      $account = $domain->Accounts->ItemByDBID($accountid);
      $rule = $account->Rules->ItemByDBID($ruleid);
   }
   else
   {
      $rule = $obBaseApp->Rules->ItemByDBID($ruleid);
   }
   
   $Name = $rule->Name;
   $Active = $rule->Active;
   $UseAND = $rule->UseAND;
}
else
{
   $Name = "";
   $Active = 0;
   $UseAND = 1;
}

  $str_delete = $obLanguage->String("Remove");
  $str_add = $obLanguage->String("Add");
?>

<h1><?php EchoTranslation("Rule")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_rule_save");
      PrintHidden("savetype", "rule");
      PrintHidden("action", $action);
      PrintHidden("domainid", $domainid);
      PrintHidden("accountid", $accountid);
      PrintHidden("ruleid", $ruleid);
   ?>      
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>         
   
      	<table border="0" width="100%" cellpadding="5">
            <?php
               PrintPropertyEditRow("Name", "Name", $Name);
               PrintCheckboxRow("Active", "Active", $Active);
               PrintCheckboxRow("UseAND", "Use a AND", $UseAND);
            ?>
         </table>
         
         <?php
            if ($ruleid == 0)
            {
               echo "<div align=center>";
               echo "<br/>";
               echo $obLanguage->String("You must save the rule before you can edit criteria and actions.");
               echo "<br/>";
               echo "<br/>";
               echo "</div>";
            }
            else
            {
               ?>
                  <h3>&nbsp;&nbsp;<?php EchoTranslation("Criteria")?> <?php echo "(<a href=\"?page=rule_criteria&action=add&domainid=$domainid&accountid=$accountid&ruleid=$ruleid\"><i>$str_add</i></a>)";?></h3>     
               	<table border="0" width="100%" cellpadding="5">
                      <tr>
                        <td width="25%"><i><?php EchoTranslation("Field")?></i></td>
                        <td width="25%"><i><?php EchoTranslation("Comparison")?></i></td>
                        <td width="25%"><i><?php EchoTranslation("Value")?></i></td>
                        <td width="25%"></td>
                      </tr>
                      <?php
                         $Criterias = $rule->Criterias;
                         $count = $Criterias->Count;
                         for ($i = 0; $i < $count; $i++)
                         {
                           $criteria = $Criterias->Item($i);
 
                           $criteriaid = $criteria->ID;
                           
                           if ($criteria->UsePredefined)
                              $fieldName = GetPredefinedFieldString($criteria->PredefinedField);
                           else
                              $fieldName = $criteria->HeaderField;
                          
                          $matchType = GetMatchTypeString($criteria->MatchType);
                          $matchValue = $criteria->MatchValue;
                         
                           ?>
                            <tr>
                              <td width="25%"><?php echo "<a href=\"?page=rule_criteria&action=edit&domainid=$domainid&accountid=$accountid&ruleid=$ruleid&criteriaid=$criteriaid\">$fieldName</a>";?></td>
                              <td width="25%"><?php echo PreprocessOutput($matchType)?></td>
                              <td width="25%"><?php echo PreprocessOutput($matchValue)?></td>
                              <td width="25%"><?php echo "<a href=\"?page=background_rule_save&csrftoken=$csrftoken&savetype=criteria&action=delete&domainid=$domainid&accountid=$accountid&&ruleid=$ruleid&criteriaid=$criteriaid\">$str_delete</a>";?></td>
                            </tr>                           
                           <?php
                         }
                      
                      ?>
                      
                      
                      
                  </table>               
   
                  <h3>&nbsp;&nbsp;<?php EchoTranslation("Actions")?> <?php echo "(<a href=\"?page=rule_action&action=add&domainid=$domainid&accountid=$accountid&ruleid=$ruleid\"><i>$str_add</i></a>)";?></h3>                    
               	<table border="0" width="100%" cellpadding="5">
                      <tr>
                        <td width="80%"><i><?php EchoTranslation("Action")?></i></td>
                        <td width="20%"></td>
                      </tr>
                      <?php
                         $Actions = $rule->Actions;
                         $count = $Actions->Count;
                         
                         for ($i = 0; $i < $count; $i++)
                         {
                           $action = $Actions->Item($i);
 
                           $actionid = $action->ID;
                           $actionName = GetRuleActionString($action->Type);
                           ?>
                            <tr>
                              <td width="25%"><?php echo "<a href=\"?page=rule_action&action=edit&domainid=$domainid&accountid=$accountid&ruleid=$ruleid&actionid=$actionid\">$actionName</a>";?></td>
                              <td width="25%"><?php echo "<a href=\"?page=background_rule_save&csrftoken=$csrftoken&savetype=action&action=delete&domainid=$domainid&accountid=$accountid&ruleid=$ruleid&actionid=$actionid\">$str_delete</a>";?></td>
                            </tr>                           
                           <?php
                         }
                      
                      ?>
                  </table>                 
               
               <?php
            
            }
         ?>


         
       </div>
    </div>  
   <?php 
      PrintSaveButton();
   ?>
</form>
