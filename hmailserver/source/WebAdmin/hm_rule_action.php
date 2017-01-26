<?php
if (!defined('IN_WEBADMIN'))
   exit();

$action = hmailGetVar("action", "");   
$domainid = hmailGetVar("domainid", 0, true);
$accountid = hmailGetVar("accountid", 0, true);
$ruleid = hmailGetVar("ruleid", 0);
$actionid = hmailGetVar("actionid", 0);

if (!GetHasRuleAccess($domainid, $accountid))
   hmailHackingAttemp(); // The user is not server administrator

   
include "include/rule_strings.php";  

 
if ($domainid == 0)
   $rule = $obBaseApp->Rules->ItemByDBID($ruleid);
else
   $rule = $obBaseApp->Domains->ItemByDBID($domainid)->Accounts->ItemByDBID($accountid)->Rules->ItemByDBID($ruleid);
  
  
if ($action == "edit")
{  
   $ruleAction = $rule->Actions->ItemByDBID($actionid);
   
   $To = $ruleAction->To;
   $IMAPFolder = $ruleAction->IMAPFolder;
   $ScriptFunction = $ruleAction->ScriptFunction;
   $FromName = $ruleAction->FromName;
   $FromAddress = $ruleAction->FromAddress;
   $Subject = $ruleAction->Subject;
   $Body = $ruleAction->Body;
   $HeaderName = $ruleAction->HeaderName;
   $Value = $ruleAction->Value;
   $Type = $ruleAction->Type;
}
else
{
   $To = "";
   $IMAPFolder = "";
   $ScriptFunction = "";
   $FromName = "";
   $FromAddress = "";
   $Subject = "";
   $Body = "";
   $HeaderName = "";
   $Value = "";
   $Type = 0;
}  
?>

<h1><?php EchoTranslation("Action")?></h1>

<script type="text/javascript">

function togglePanel() 
{
   hideAllPanels();
   
   var selectElement = document.getElementById("Type");
   
   if (selectElement)
   {
      var element = document.getElementById("panel-" + selectElement.value);
      if (element)
      {
         element.style.display = '';
      }
   }
}

function hideAllPanels()
{
   for (var i = 0; i < 15; i++)
   {
      var panel = document.getElementById("panel-" + i);
      if (panel)
      {
         panel.style.display = 'none';
      }  
   }
}



</script>


<form action="index.php" name="mainForm" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_rule_save");
      PrintHidden("savetype", "action");
      PrintHidden("action", $action);
      PrintHidden("domainid", $domainid);
      PrintHidden("accountid", $accountid);
      PrintHidden("ruleid", $ruleid);
      PrintHidden("actionid", $actionid);
   ?>      
   
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>         
   
      	<table border="0" width="100%" cellpadding="5">
      		<tr>
               <td>
                   <select name="Type" id="Type" onchange="togglePanel()" onkeyup="togglePanel(this)">
                     <?php
                         
                        $eRADeleteEmailSelected = $Type == eRADeleteEmail ? "selected" : "";
                        $eRAForwardEmailSelected = $Type == eRAForwardEmail ? "selected" : "";
                        $eRAReplySelected = $Type == eRAReply ? "selected" : "";
                        $eRAMoveToImapFolderSelected = $Type == eRAMoveToImapFolder ? "selected" : "";
                        $eRARunScriptFunctionSelected = $Type == eRARunScriptFunction ? "selected" : "";
                        $eRAStopRuleProcessingSelected = $Type == eRAStopRuleProcessing ? "selected" : "";
                        $eRASetHeaderValueSelected = $Type == eRASetHeaderValue ? "selected" : "";
                        $eRASendUsingRouteSelected = $Type == eRASendUsingRoute ? "selected" : "";
                        $eRACreateCopy = $Type == eRACreateCopy ? "selected" : "";
						$eRABindToAddress = $Type == eRABindToAddress ? "selected" : "";
                     
                        echo "<option value= " . eRADeleteEmail . " $eRADeleteEmailSelected>" . GetRuleActionString(eRADeleteEmail) .  "</option>";
                        echo "<option value= " . eRAForwardEmail . " $eRAForwardEmailSelected>" . GetRuleActionString(eRAForwardEmail) .  "</option>";
                        echo "<option value= " . eRAReply . " $eRAReplySelected>" . GetRuleActionString(eRAReply) .  "</option>";
                        echo "<option value= " . eRAMoveToImapFolder . " $eRAMoveToImapFolderSelected>" . GetRuleActionString(eRAMoveToImapFolder) .  "</option>";
                        echo "<option value= " . eRASetHeaderValue . " $eRASetHeaderValueSelected>" . GetRuleActionString(eRASetHeaderValue) .  "</option>";
                        echo "<option value= " . eRAStopRuleProcessing . " $eRAStopRuleProcessingSelected>" . GetRuleActionString(eRAStopRuleProcessing) .  "</option>";
						
                        $disabled = hmailGetAdminLevel() == ADMIN_SERVER ? "": "disabled=\"disabled\"";
                        echo "<option $disabled value= " . eRARunScriptFunction . " $eRARunScriptFunctionSelected>" . GetRuleActionString(eRARunScriptFunction) .  "</option>";
                        echo "<option $disabled value= " . eRACreateCopy . " $eRACreateCopy>" . GetRuleActionString(eRACreateCopy) .  "</option>";

						// The following actions makes no sense in account-level rules.
                        if ($accountid == 0)
						{
							echo "<option $disabled value= " . eRASendUsingRoute . " $eRASendUsingRouteSelected>" . GetRuleActionString(eRASendUsingRoute) .  "</option>";
							echo "<option $disabled value= " . eRABindToAddress . " $eRABindToAddress>" . GetRuleActionString(eRABindToAddress) .  "</option>";
						}						
                     ?>
                   </select>
               </td>
      		</tr>
	
      		<tr>
               <td>
                  <div id="panel-<?php echo eRADeleteEmail?>" name="panel-1" style="display: none">
                     <!-- empty panel -->
                  </div>
                  
                  <div id="panel-<?php echo eRAForwardEmail?>" style="display: none">
                     <?php EchoTranslation("To")?><br/>
                     <input type="text" name="To" value="<?php echo PreprocessOutput($To)?>">
                  </div>
                  
                  <div id="panel-<?php echo eRAReply?>" style="display: none">
                     <?php EchoTranslation("From (Name)")?><br/>
                     <input type="text" name="FromName" value="<?php echo PreprocessOutput($FromName)?>"><br/>
                     <br/>
                     <?php EchoTranslation("From (Address)")?><br/>
                     <input type="text" name="FromAddress" value="<?php echo PreprocessOutput($FromAddress)?>"><br/>
                     <br/>
                     <?php EchoTranslation("Subject")?><br/>
                     <input type="text" name="Subject" value="<?php echo PreprocessOutput($Subject)?>"><br/>
                     <br/>
                     <?php EchoTranslation("Body")?><br/>
                     <textarea name="Body" cols="30" rows="5"><?php echo PreprocessOutput($Body)?></textarea>
                     <br/>
                  </div>   

                  <div id="panel-<?php echo eRAMoveToImapFolder?>" style="display: none">
                     <?php EchoTranslation("IMAP folder")?><br/>
                     <input type="text" name="IMAPFolder" value="<?php echo PreprocessOutput($IMAPFolder)?>">
                  </div>  
               
                  <div id="panel-<?php echo eRARunScriptFunction?>" style="display: none">
                     <?php EchoTranslation("Script function")?><br/>
                     <input type="text" name="ScriptFunction" value="<?php echo PreprocessOutput($ScriptFunction)?>">
                  </div>               
               
                  <div id="panel-<?php echo eRAStopRuleProcessing?>" style="display: none">
                    <!-- empty panel -->
                  </div>          

           
                  <div id="panel-<?php echo eRABindToAddress?>" style="display: none">
                     <?php EchoTranslation("IP address")?><br/>
                     <input type="text" name="BindToAddress" value="<?php echo PreprocessOutput($Value)?>"><br/>
                     <br/>
                  </div>   
						  

                  <div id="panel-<?php echo eRASetHeaderValue?>" style="display: none">
                     <?php EchoTranslation("Header name")?><br/>
                     <input type="text" name="HeaderName" value="<?php echo PreprocessOutput($HeaderName)?>"><br/>
                     <br/>
                     <?php EchoTranslation("Value")?><br/>
                     <input type="text" name="Value" value="<?php echo PreprocessOutput($Value)?>"><br/>
                     <br/>
                  </div>   
       
				  
                  <div id="panel-<?php echo eRASendUsingRoute?>" style="display: none">
                     <?php EchoTranslation("Route")?><br/>
                     
                     <select name="RouteID">
                     <?php
                        if (hmailGetAdminLevel() == ADMIN_SERVER)
                        {
                           $obSettings = $obBaseApp->Settings;
                           $obRoutes	= $obSettings->Routes();
                           $Count = $obRoutes->Count();
                        
                           for ($i = 0; $i < $Count; $i++)
                           {
                              $obRoute             = $obRoutes->Item($i);
                              $routename           = $obRoute->DomainName;
                              $routeid             = $obRoute->ID;
                        	
                              $routename = PreprocessOutput($routename);
                           
                              echo "<option value=\"$routeid\">$routename</option>";
                           }  
                        }

                      ?>
                        
                     </select>
                  </div>                     
               </td>
               
      		</tr>   
   
         </table>
       </div>
    </div>  
   <?php 
      PrintSaveButton();
   ?>
   
   <script type="text/javascript">
      togglePanel();
   </script>
</form>
