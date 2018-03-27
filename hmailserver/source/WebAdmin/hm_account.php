<?php
if (!defined('IN_WEBADMIN'))
   exit();

$domainid	= hmailGetVar("domainid",0,true);
$accountid	= hmailGetVar("accountid",0,true);
$action	   = hmailGetVar("action","");

$error_message	   = hmailGetVar("error_message","");

if (hmailGetAdminLevel() == 0 && ($accountid != hmailGetAccountID() || $action != "edit" || $domainid != hmailGetDomainID()))
   hmailHackingAttemp();

if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
	hmailHackingAttemp(); // Domain admin but not for this domain.

$obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);

$admin_rights = (hmailGetAdminLevel()  === ADMIN_SERVER || 
				     hmailGetAdminLevel()  === ADMIN_DOMAIN);
				
$accountactive = 1;
$accountmaxsize = 0;
$accountsize = 0;				
$accountaddress = "";
$accountlastlogontime = "";
$accountadminlevel = 0;
$PersonFirstName = "";
$PersonLastName = "";

$vacationmessageon = 0;
$vacationsubject = "";
$vacationmessage = "";

$vacationmessageexpires = 0;
$vacationmessageexpiresdate = "";

$forwardenabled = 0;
$forwardaddress = "";
$forwardkeeporiginal = 0;

$adenabled       = 0;
$addomain        = "";
$adusername      = "";

$SignatureEnabled = 0;
$SignatureHTML = "";
$SignaturePlainText = "";

if ($action == "edit")
{
   $obAccount = $obDomain->Accounts->ItemByDBID($accountid);  

   $accountmaxsize = $obAccount->MaxSize;
   $accountaddress = $obAccount->Address;
   $accountactive  = $obAccount->Active;
   $accountsize    = $obAccount->Size();
   $accountlastlogontime = $obAccount->LastLogonTime();
   $accountadminlevel = $obAccount->AdminLevel();
   
   $accountaddress = substr($accountaddress, 0, strpos($accountaddress, "@"));
   
   $PersonFirstName = $obAccount->PersonFirstName;
   $PersonLastName = $obAccount->PersonLastName;
   
   $vacationmessageon = $obAccount->VacationMessageIsOn;
   $vacationsubject = $obAccount->VacationSubject;
   $vacationmessage = $obAccount->VacationMessage;

   $vacationmessageexpires     = $obAccount->VacationMessageExpires;
   $vacationmessageexpiresdate = $obAccount->VacationMessageExpiresDate;
   $vacationmessageexpiresdate = substr($vacationmessageexpiresdate, 0, 10);
     
   
   
   $forwardenabled = $obAccount->ForwardEnabled;
   $forwardaddress = $obAccount->ForwardAddress;
   $forwardkeeporiginal = $obAccount->ForwardKeepOriginal;
   
   $adenabled       = $obAccount->IsAD;
   $addomain        = $obAccount->ADDomain;
   $adusername      = $obAccount->ADUsername;
  
   $SignatureEnabled   = $obAccount->SignatureEnabled;
   $SignatureHTML 	   = $obAccount->SignatureHTML;
   $SignaturePlainText = $obAccount->SignaturePlainText;
}

$SignatureEnabledChecked = hmailCheckedIf1($SignatureEnabled);   
$vacationmessageexpireschecked = hmailCheckedIf1($vacationmessageexpires);   
$accountactivechecked = hmailCheckedIf1($accountactive);

$domainname = $obDomain->Name;

$str_user   = $obLanguage->String("User");
$str_domain = $obLanguage->String("Domain");
$str_server = $obLanguage->String("Server");

?>

<h1><?php EchoTranslation("Account")?></h1>

<?php
   if (strlen($error_message) > 0)
   {
      $error_message = PreprocessOutput($obLanguage->String($error_message));
      echo "<font color=\"red\"><b>$error_message</b></font><br><br>";
   }
?>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

	
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_account_save");
      PrintHidden("action", $action);
      PrintHidden("domainid", $obDomain->ID);
      PrintHidden("accountid", $accountid);
   ?>
	
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>   
   
      	<table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td width="30%"><?php EchoTranslation("Address")?></td>
      			<td width="70%">
         			<?php
         			$str_address = $obLanguage->String("Address");
                  
                  $accountaddress = PreprocessOutput($accountaddress);
                  
         			if ($admin_rights)
         			   echo "<input type=\"text\" name=\"accountaddress\" value=\"$accountaddress\" checkallownull=\"false\" checkmessage=\"$str_address\">";
         			else
         				echo $accountaddress;
                     
         			?>@<?php echo $domainname?>
               </td>			
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("Password")?></td>
      			<td><input type="password" name="accountpassword" value="" autocomplete="off"></td>
      		</tr>
         
      		<tr>
      			<td><?php EchoTranslation("Maximum size (MB)")?></td>
      			<td>
         			<?php
         			$str_mailboxsize = $obLanguage->String("Maximum size (MB)");
                  
                  $accountmaxsize = PreprocessOutput($accountmaxsize);
                  
         			if ($admin_rights)
         			   echo "<input type=\"text\" name=\"accountmaxsize\" value=\"$accountmaxsize\" checktype=\"number\" size=\"5\"checkallownull=\"false\" checkmessage=\"$str_mailboxsize\">";
         			else
         				echo $accountmaxsize;
         			?>
         	   </td>
      		</tr>
            
            <?php
               PrintPropertyRow("Size (MB)", Round($accountsize,3));
               PrintPropertyRow("Last logon time", $accountlastlogontime);
            ?>
            
      		<tr>
      			<td><?php EchoTranslation("Administration level")?></td>
      			<td>
      		      <select name="accountadminlevel" 
      		         <?php if ($admin_rights == 0) echo " disabled ";?>
      		         >
      		         <option value="0" <?php if ($accountadminlevel == 0) echo " selected "; ?>><?php echo $str_user; ?></option>
      		         <option value="1" <?php if ($accountadminlevel == 1) echo " selected "; ?>><?php echo $str_domain; ?></option>
      		         <option value="2" <?php if ($accountadminlevel == 2) echo " selected "; ?>><?php echo $str_server; ?></option>
      		      </select>

         	   </td>
      		</tr>		
      		<tr>
      			<td><?php EchoTranslation("Enabled")?></td>
      			<td>
      			<?php
      			if ($admin_rights)
      			   echo "<input type=\"checkbox\" name=\"accountactive\" value=\"1\" $accountactivechecked>";
      			else
      			{
      				if ($accountactive == 1)
      				   echo $obLanguage->String("Yes");
      				else
      				   echo $obLanguage->String("No");
      		   }
      			?>			
      			</td>
      		</tr>        
         </table>
       </div>
       
       
       <div class="tabbertab">
          <h2><?php EchoTranslation("Auto-reply")?></h2>       
   
      	<table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>
            <?php
               PrintCheckboxRow("vacationmessageon", "Enabled", $vacationmessageon);
               PrintPropertyEditRow("vacationsubject", "Subject", $vacationsubject);
            ?>
      		<tr>
      			<td><?php EchoTranslation("Text")?></td>
      			<td><textarea name="vacationmessage" rows="6" cols="55"><?php echo PreprocessOutput($vacationmessage)?></textarea></td>
      		</tr>
      		<tr>
      			<td valign="top"><?php EchoTranslation("Automatically expire")?></td>
      			<td valign="top">
         			<?php
         			echo "<input type=\"checkbox\" name=\"vacationmessageexpires\" value=\"1\" $vacationmessageexpireschecked>";
         			?>
         		
         			<input type="text" name="vacationmessageexpiresdate" value="<?php echo PreprocessOutput($vacationmessageexpiresdate)?>"> (YYYY-MM-DD)
               </td>		
      		</tr>		
      	</table>
      </div>
       
      <div class="tabbertab">
          <h2><?php EchoTranslation("Forwarding")?></h2>       
   
         <table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>
            <?php
               PrintCheckboxRow("forwardenabled", "Enabled", $forwardenabled);
               PrintPropertyEditRow("forwardaddress", "Address", $forwardaddress);
               PrintCheckboxRow("forwardkeeporiginal", "Keep original message", $forwardkeeporiginal);
               
            ?>
         </table>
      </div>       
       
       
      <div class="tabbertab">
          <h2><?php EchoTranslation("Signature")?></h2>       
   
      	<table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>   
      		<?php
               PrintCheckboxRow("SignatureEnabled", "Enabled", $SignatureEnabled);
            
            ?>

      		<tr>
      			<td valign="top"><?php EchoTranslation("Plain text signature")?></td>
      			<td>
      				<textarea cols="50" rows="4" name="SignaturePlainText"><?php echo PreprocessOutput($SignaturePlainText)?></textarea>
      			</td>
      		</tr>
      		<tr>
      			<td valign="top"><?php EchoTranslation("HTML signature")?></td>
      			<td>
      				<textarea cols="50" rows="4" name="SignatureHTML"><?php echo PreprocessOutput($SignatureHTML)?></textarea>
      			</td>
      		</tr>	
         </table>
      </div>       
       
      <?php if (hmailGetAdminLevel() != ADMIN_USER)
      {
      ?>
         <div class="tabbertab">
             <h2><?php EchoTranslation("Active Directory")?></h2>            
            
            <table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>               
               <?php
                  PrintCheckboxRow("adenabled", "Enabled", $adenabled);
                  PrintPropertyEditRow("addomain", "Domain", $addomain);
                  PrintPropertyEditRow("adusername", "User name", $adusername);
               ?>
             </table>
          </div>
       <?php
       }
       
       if (GetHasRuleAccess($domainid, $accountid))
       {
       ?>
         <div class="tabbertab">
         
            <h2><?php EchoTranslation("Rules")?></h2>            
         
            <?php
            if ($accountid == 0)
            {
               echo "<div align=center>";
               echo "<br/>";
               echo $obLanguage->String("You must save the account before you can edit rules.");
               echo "<br/>";
               echo "<br/>";
               echo "</div>";
            }
            else
            {
               ?>
               <table border="0" width="100%" cellpadding="5">
               <tr>
                  <td width="50%">
                     <i><?php EchoTranslation("Name");?></i>
                  </td>
                  <td width="25%">
                     <i><?php EchoTranslation("Enabled");?></i>
                  </td>
                  
                  <td>
                  </td>
               </tr>
               <?php
               $rules = $obAccount->Rules();
               $Count = $rules->Count();

               $str_delete = $obLanguage->String("Remove");
               $str_yes = $obLanguage->String("Yes");
               $str_no = $obLanguage->String("No");
               
               $bgcolor = "#EEEEEE";

               for ($i = 0; $i < $Count; $i++)
               {
               	$rule = $rules->Item($i);
               	$rulename = $rule->Name;
               	$ruleid = $rule->ID;
                  $enabled = $rule->Active ? $str_yes : $str_no;
               	
                  $rulename = PreprocessOutput($rulename);
                  
               	echo "<tr bgcolor=\"$bgcolor\">";
               	echo "<td><a href=\"?page=rule&action=edit&domainid=$domainid&accountid=$accountid&ruleid=$ruleid&\">$rulename</a></td>";
                  echo "<td><a href=\"?page=rule&action=edit&domainid=$domainid&accountid=$accountid&ruleid=$ruleid&\">$enabled</a></td>";
               	echo "<td><a href=\"?page=background_rule_save&savetype=rule&csrftoken=$csrftoken&action=delete&domainid=$domainid&accountid=$accountid&action=delete&ruleid=$ruleid\">$str_delete</a></td>";
               	echo "</tr>";
               	
               	if ($bgcolor == "#EEEEEE")
               	   $bgcolor = "#DDDDDD";
               	else
               	   $bgcolor = "#EEEEEE";
               }            
               
               echo "</table>";
               
               $str_add = $obLanguage->String("Add");
               echo "<a href=\"?page=rule&domainid=$domainid&accountid=$accountid&action=add\"><i>$str_add</i></a>";
            }

            ?>
            
            
            
            
          </div>
       
       <?php
       }
       ?>

       
       <div class="tabbertab">
         <h2><?php EchoTranslation("Advanced")?></h2>   
   
      	<table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>            
      		<tr>
      			<td><?php EchoTranslation("First name")?></td>
      			<td><?php EchoTranslation("Last name")?></td>
      		</tr>
            
      		<tr>
      			<td>
                  <input type="text" name="PersonFirstName" value="<?php echo PreprocessOutput($PersonFirstName)?>" size="40">
               </td>
      			<td>
                  <input type="text" name="PersonLastName" value="<?php echo PreprocessOutput($PersonLastName)?>" size="40">
               </td>
      		</tr>
         </table>
      </div>
   </div>
         
   <?php
      PrintSaveButton();
   ?>

</form>
