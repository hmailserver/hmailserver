<?php

if (!defined('IN_WEBADMIN'))
   exit();

$domainid	= hmailGetVar("domainid",0,true);
$accountid	= hmailGetVar("accountid",0,true);
$faid 		= hmailGetVar("faid",0, true);
$action	   = hmailGetVar("action","");

if (hmailGetAdminLevel() == 0 && ($accountid != hmailGetAccountID() || $domainid != hmailGetDomainID()))
   hmailHackingAttemp();

if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
	hmailHackingAttemp(); // Domain admin but not for this domain.

$obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);
$obAccount = $obDomain->Accounts->ItemByDBID($accountid);  

if ($action == "edit")
{
   $obFetchAccount = $obAccount->FetchAccounts->ItemByDBID($faid);
	
   $Enabled = $obFetchAccount->Enabled;
   $Name = $obFetchAccount->Name;
   $DaysToKeepMessages    = $obFetchAccount->DaysToKeepMessages;
   $MinutesBetweenFetch   = $obFetchAccount->MinutesBetweenFetch;
   $Port  				  = $obFetchAccount->Port;
   $ProcessMIMERecipients = $obFetchAccount->ProcessMIMERecipients;
   $ProcessMIMEDate       = $obFetchAccount->ProcessMIMEDate;
   $ServerAddress  		  = $obFetchAccount->ServerAddress;
   $ServerType  		  = $obFetchAccount->ServerType;
   $Username   			  = $obFetchAccount->Username;
   $UseAntiSpam           = $obFetchAccount->UseAntiSpam;
   $UseAntiVirus          = $obFetchAccount->UseAntiVirus;
   $EnableRouteRecipients = $obFetchAccount->EnableRouteRecipients;
   $ConnectionSecurity    = $obFetchAccount->ConnectionSecurity;
}
else 
{
   $Enabled = 1;
   $Name = "";
   $DaysToKeepMessages = 0;
   $MinutesBetweenFetch = 30;
   $Port = 110;
   $ProcessMIMERecipients = 0;
   $ProcessMIMEDate = 0;
   $ServerAddress = "";
   $ServerType = 0;
   $Username = "";
   $UseAntiSpam = 0;
   $UseAntiVirus = 0;
   $EnableRouteRecipients = 0;
   $ConnectionSecurity = 0;
}

$EnabledChecked = hmailCheckedIf1($Enabled);
$ProcessMIMERecipientsChecked = hmailCheckedIf1($ProcessMIMERecipients);
$ProcessMIMEDateChecked = hmailCheckedIf1($ProcessMIMEDate);

$DaysToKeepMessagesValue = 0;
if ($DaysToKeepMessages > 0)
	$DaysToKeepMessagesValue = $DaysToKeepMessages;
?>

<h1><?php EchoTranslation("External account")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">
	
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_account_externalaccount_save");
      PrintHidden("action", $action);
      PrintHidden("faid", $faid);
      PrintHidden("domainid", $domainid);
      PrintHidden("accountid", $accountid);
   ?>
   
   <div class="tabber">
      <div class="tabbertab">
          <h2><?php EchoTranslation("General")?></h2>            
   
         	<table border="0" width="100%" cellpadding="5">
         		<tr>
         			<td width="30%"><?php EchoTranslation("Enabled")?></td>
         			<td width="70%">
            			<?php
            			echo "<input type=\"checkbox\" name=\"Enabled\" value=\"1\" $EnabledChecked>";
            			?>				
                  	</td>			
         		</tr>
         		<tr>
         			<td colspan="2"><?php EchoTranslation("Server information")?></td>
         		</tr>
         		<tr>
         			<td><?php EchoTranslation("Name")?></td>
         			<td>
            				<input type="text" name="Name" value="<?php echo PreprocessOutput($Name)?>" checkallownull="false" checkmessage="<?php EchoTranslation("Name")?>">
                  	</td>			
         		</tr>	
         		<tr>
         			<td><?php EchoTranslation("Type")?></td>
         			<td>
            				<select name="Type"  style="font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif">
            					<option value="0" selected>POP3</option>
            				</select>
                  	</td>			
         		</tr>	
         		<tr>
         			<td colspan="2">
         				<table width="350">
         					<tr>
         						<td><?php EchoTranslation("Server address")?></td>
         						<td><?php EchoTranslation("TCP/IP port")?></td>
         					</tr>
         					<tr>
         						<td><input type="text" name="ServerAddress" size="25" value="<?php echo PreprocessOutput($ServerAddress)?>" checkallownull="false" checkmessage="<?php EchoTranslation("Server address")?>"></td>
         						<td><input type="text" name="Port" size="25" value="<?php echo PreprocessOutput($Port)?>"  checktype="number" checkallownull="false" checkmessage="<?php EchoTranslation("TCP/IP port")?>"></td>
         					</tr>					
         				</table>
         			</td>
         		</tr>	
				<tr>
					<td><?php EchoTranslation("Connection security")?></td>
					<td><select name="ConnectionSecurity">
						<option value="<?php echo CONNECTION_SECURITY_NONE?>" <?php if ($ConnectionSecurity == CONNECTION_SECURITY_NONE) echo "selected";?> ><?php EchoTranslation("None")?></a>
						<option value="<?php echo CONNECTION_SECURITY_STARTTLSREQUIRED?>" <?php if ($ConnectionSecurity == CONNECTION_SECURITY_STARTTLSREQUIRED) echo "selected";?> ><?php EchoTranslation("STARTTLS (Required)")?></a>
						<option value="<?php echo CONNECTION_SECURITY_TLS?>" <?php if ($ConnectionSecurity == CONNECTION_SECURITY_TLS) echo "selected";?> ><?php EchoTranslation("SSL/TLS")?></a>
					</select></td>
				</tr>
				
         		<tr>
         			<td colspan="2">
         				<table width="350">
         					<tr>
         						<td><?php EchoTranslation("User name")?></td>
         						<td><?php EchoTranslation("Password")?></td>
         					</tr>
         					<tr>
         						<td><input type="text" name="Username" value="<?php echo PreprocessOutput($Username)?>" size="25" checkallownull="false" checkmessage="<?php EchoTranslation("User name")?>"></td>
         						<td><input type="password" name="Password" value=""  size="25" autocomplete="off"></td>
         					</tr>					
         				</table>
         			</td>
         		</tr>
         		<tr>
         			<td colspan="2"><?php EchoTranslation("Settings")?></td>
         		</tr>
         		<tr>
         			<td><?php EchoTranslation("Minutes between download")?></td>
         			<td>
            				<input type="text" name="MinutesBetweenFetch" value="<?php echo PreprocessOutput($MinutesBetweenFetch)?>" checktype="number" checkallownull="false" checkmessage="<?php EchoTranslation("Minutes between download")?>">
                  	</td>			
         		</tr>			
         		<tr>
         			<td><?php EchoTranslation("Deliver to recipients in MIME headers")?></td>
         			<td>
         			   	<?php
            					echo "<input type=\"checkbox\" name=\"ProcessMIMERecipients\" value=\"1\" $ProcessMIMERecipientsChecked>";
            				?>				
                  	</td>			
         		</tr>	
         		<tr>
         			<td><?php EchoTranslation("Retrieve date from Received header")?></td>
         			<td>
         			   	<?php
            					echo "<input type=\"checkbox\" name=\"ProcessMIMEDate\" value=\"1\" $ProcessMIMEDateChecked>";
            				?>				
                  	</td>			
         		</tr>			
         		
               <?php
                  PrintCheckboxRow("UseAntiSpam", "Anti-spam", $UseAntiSpam);
                  PrintCheckboxRow("UseAntiVirus", "Anti-virus", $UseAntiVirus);
                  PrintCheckboxRow("EnableRouteRecipients", "Allow route recipients", $EnableRouteRecipients);
                  
               ?>
               
         		<tr>
         			<td colspan="2">
         				<table width="350">
         					<tr>
         						<td><input type="radio" name="DaysToKeepMessages" value="-1" <?php if ($DaysToKeepMessages == -1) echo "checked";?> ></td>
         						<td><?php EchoTranslation("Delete messages immediately")?></td>
         						<td>&nbsp;</td>
         					</tr>
         					<tr>
         						<td><input type="radio" name="DaysToKeepMessages" value="0" <?php if ($DaysToKeepMessages == 0) echo "checked";?> ></td>
         						<td><?php EchoTranslation("Do not delete messages")?></td>
         						<td>&nbsp;</td>
         					</tr>
         					<tr>
         						<td><input type="radio" name="DaysToKeepMessages" value=""  <?php if ($DaysToKeepMessages > 0) echo "checked";?>></td>
         						<td><?php EchoTranslation("Delete messages after")?></td>
         						<td><input type="text" name="DaysToKeepMessagesValue" value="<?php echo PreprocessOutput($DaysToKeepMessagesValue)?>"> <?php EchoTranslation("days")?></td>
         					</tr>				
         				</table>
         			</td>
         		</tr>		
            </table>
         </div>
      </div>
			
      <?php 
         PrintSaveButton();
      ?>
</form>
