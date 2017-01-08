<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp(); // The user is not server administrator

$securityrangeid	= hmailGetVar("securityrangeid",0);
$action	         	= hmailGetVar("action","");
	
$securityrangename="";
$securityrangepriority = 100;
$securityrangelowerip = "0.0.0.0";
$securityrangeupperip = "255.255.255.255";

$allowsmtpconnections = 1;
$allowpop3connections = 1;
$allowimapconnections = 1;

$allowlocaltolocal = 1;
$allowlocaltoremote = 1;
$allowremotetolocal = 1;
$allowremotetoremote = 0;

$enablespamprotection = 0;
$IsForwardingRelay = 0;
$EnableAntiVirus = 0;
$RequireSSLTLSForAuth = 0;

$RequireSMTPAuthLocalToLocal = true;
$RequireSMTPAuthLocalToExternal = true;
$RequireSMTPAuthExternalToLocal = false;
$RequireSMTPAuthExternalToExternal = true;

$Expires = false;
$ExpiresTime = "";

if ($action == "edit")
{
   $obSecurityRange = $obBaseApp->Settings->SecurityRanges->ItemByDBID($securityrangeid);

   $securityrangename		= $obSecurityRange->Name;
   $securityrangepriority	= $obSecurityRange->Priority;
   $securityrangelowerip	= $obSecurityRange->LowerIP;
   $securityrangeupperip	= $obSecurityRange->UpperIP;
   
   $allowsmtpconnections	= $obSecurityRange->AllowSMTPConnections;
   $allowpop3connections	= $obSecurityRange->AllowPOP3Connections;
   $allowimapconnections	= $obSecurityRange->AllowIMAPConnections;
   
   $allowlocaltolocal		= $obSecurityRange->AllowDeliveryFromLocalToLocal;
   $allowlocaltoremote		= $obSecurityRange->AllowDeliveryFromLocalToRemote;
   $allowremotetolocal		= $obSecurityRange->AllowDeliveryFromRemoteToLocal;
   $allowremotetoremote		= $obSecurityRange->AllowDeliveryFromRemoteToRemote;
   
   $enablespamprotection	= $obSecurityRange->EnableSpamProtection;
   $EnableAntiVirus	        = $obSecurityRange->EnableAntiVirus;
   $IsForwardingRelay	    = $obSecurityRange->IsForwardingRelay;
   $RequireSSLTLSForAuth	= $obSecurityRange->RequireSSLTLSForAuth;
   
   $RequireSMTPAuthLocalToLocal           = $obSecurityRange->RequireSMTPAuthLocalToLocal;
   $RequireSMTPAuthLocalToExternal        = $obSecurityRange->RequireSMTPAuthLocalToExternal;
   $RequireSMTPAuthExternalToLocal        = $obSecurityRange->RequireSMTPAuthExternalToLocal;
   $RequireSMTPAuthExternalToExternal     = $obSecurityRange->RequireSMTPAuthExternalToExternal;
   
   $Expires                = $obSecurityRange->Expires;
   $ExpiresTime            = $obSecurityRange->ExpiresTime;
}

$allowsmtpconnectionschecked	= hmailCheckedIf1($allowsmtpconnections);
$allowpop3connectionschecked	= hmailCheckedIf1($allowpop3connections);
$allowimapconnectionschecked	= hmailCheckedIf1($allowimapconnections);

$allowlocaltolocalchecked	= hmailCheckedIf1($allowlocaltolocal);
$allowlocaltoremotechecked	= hmailCheckedIf1($allowlocaltoremote);
$allowremotetolocalchecked	= hmailCheckedIf1($allowremotetolocal);
$allowremotetoremotechecked = hmailCheckedIf1($allowremotetoremote);

$IsForwardingRelayChecked   = hmailCheckedIf1($IsForwardingRelay);
?>

<h1><?php EchoTranslation("IP range")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_securityrange_save");
      PrintHidden("action", $action);
      PrintHidden("securityrangeid", $securityrangeid);
   ?>
	
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>         
   
   
      	<table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td width="30%"><?php EchoTranslation("Name")?></td>
      			<td width="70%">
         			<input type="text" name="securityrangename" value="<?php echo PreprocessOutput($securityrangename)?>" size="30"  checkallownull="false" checkmessage="<?php EchoTranslation("Name")?>">
               </td>			
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("Priority")?></td>
      			<td><input type="text" name="securityrangepriority" value="<?php echo PreprocessOutput($securityrangepriority)?>" size="4" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Priority")?>"></td>
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("Lower IP")?></td>
               <td><input type="text" name="securityrangelowerip" value="<?php echo PreprocessOutput($securityrangelowerip)?>" size="25" checkallownull="false" checktype="ipaddress" checkmessage="<?php EchoTranslation("Lower IP")?>"></td>
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("Upper IP")?></td>
      			<td><input type="text" name="securityrangeupperip" value="<?php echo PreprocessOutput($securityrangeupperip)?>" size="25" checkallownull="false" checktype="ipaddress" checkmessage="<?php EchoTranslation("Upper IP")?>"></td>
      		</tr>		
            <?php
               PrintCheckboxRow("Expires", "Expires", $Expires);
            ?>
            <tr>
               <td>&nbsp;</td>
               <td><input type="text" name="ExpiresTime" value="<?php echo PreprocessOutput($ExpiresTime)?>" size="25"> (YYYY-MM-DD HH:MM:SS)</td>
            </tr> 
         </table>
       </div>
      <div class="tabbertab">
         <h2><?php EchoTranslation("Allow connections")?></h2>             
         <table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>            
         	<tr>
         		<td><?php EchoTranslation("SMTP")?></td>
         		<td><input type="checkbox" name="allowsmtpconnections" value="1" <?php echo $allowsmtpconnectionschecked?>></td>
         	</tr> 
         	<tr>
         		<td><?php EchoTranslation("POP3")?></td>
         		<td><input type="checkbox" name="allowpop3connections" value="1" <?php echo $allowpop3connectionschecked?>></td>
         	</tr> 
         	<tr>
         		<td><?php EchoTranslation("IMAP")?></td>
         		<td><input type="checkbox" name="allowimapconnections" value="1" <?php echo $allowimapconnectionschecked?>></td>
         	</tr>    
         </table>
      </div>
      <div class="tabbertab">
         <h2><?php EchoTranslation("Allow deliveries from")?></h2>          
         <table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>            
         	<tr>
         		<td><?php EchoTranslation("Local to local e-mail addresses")?></td>
         		<td><input type="checkbox" name="allowlocaltolocal" value="1" <?php echo $allowlocaltolocalchecked?>></td>
         	</tr>  
         	<tr>
         		<td><?php EchoTranslation("Local to external e-mail addresses")?></td>
         		<td><input type="checkbox" name="allowlocaltoremote" value="1" <?php echo $allowlocaltoremotechecked?>></td>
         	</tr>  
         	<tr>
         		<td><?php EchoTranslation("External to local e-mail addresses")?></td>
         		<td><input type="checkbox" name="allowremotetolocal" value="1" <?php echo $allowremotetolocalchecked?>></td>
         	</tr>  
         	<tr>
         		<td><?php EchoTranslation("External to external e-mail addresses")?></td>
         		<td><input type="checkbox" name="allowremotetoremote" value="1" <?php echo $allowremotetoremotechecked?>></td>
         	</tr>   
         </table>
      </div>
      
      <div class="tabbertab">
         <h2><?php EchoTranslation("Require SMTP authentication")?></h2>         
         <table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>            
            <?php
               PrintCheckboxRow("RequireSMTPAuthLocalToLocal", "Local to local e-mail addresses", $RequireSMTPAuthLocalToLocal);
               PrintCheckboxRow("RequireSMTPAuthLocalToExternal", "Local to external e-mail addresses", $RequireSMTPAuthLocalToExternal);
               PrintCheckboxRow("RequireSMTPAuthExternalToLocal", "External to local e-mail addresses", $RequireSMTPAuthExternalToLocal);
               PrintCheckboxRow("RequireSMTPAuthExternalToExternal", "External to external e-mail addresses", $RequireSMTPAuthExternalToExternal);
            ?>
         </table>
      </div>
      
      <div class="tabbertab">
         <h2><?php EchoTranslation("Other")?></h2>         
         <table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>            
            <?php
               PrintCheckboxRow("enablespamprotection", "Anti-spam", $enablespamprotection);
               PrintCheckboxRow("EnableAntiVirus", "Anti-virus", $EnableAntiVirus);
			   PrintCheckboxRow("RequireSSLTLSForAuth", "Require SSL/TLS for authentication", $RequireSSLTLSForAuth);
            ?>
  		  	
         </table>
      </div>
    </div>  
   <?php 
      PrintSaveButton();
   ?>
</form>
