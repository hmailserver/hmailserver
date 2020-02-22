<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();

$action	   = hmailGetVar("action","");

if($action == "save")
{
	// General
	$obSettings->MaxSMTPConnections = hmailGetVar("maxsmtpconnections",0);
	$obSettings->WelcomeSMTP 	    = hmailGetVar("welcomesmtp",0);


	// Delivery of email
	$obSettings->SMTPNoOfTries= hmailGetVar("smtpnooftries",0);
	$obSettings->SMTPMinutesBetweenTry= hmailGetVar("smtpminutesbetweentry",0);
	$obSettings->HostName= hmailGetVar("HostName", "");
   
   
    $obSettings->SMTPRelayer= hmailGetVar("smtprelayer",0);
	$obSettings->SMTPRelayerPort= hmailGetVar("smtprelayerport",0);
    $obSettings->SMTPRelayerRequiresAuthentication = hmailGetVar("SMTPRelayerRequiresAuthentication",0);
    $obSettings->SMTPRelayerUsername = hmailGetVar("SMTPRelayerUsername","");
    $obSettings->SMTPRelayerConnectionSecurity = hmailGetVar("SMTPRelayerConnectionSecurity","0");
	  
    if (hmailGetVar("SMTPRelayerPassword","") != "")
      $obSettings->SetSMTPRelayerPassword(hmailGetVar("SMTPRelayerPassword",""));
   
	
	$obSettings->RuleLoopLimit = hmailGetVar("smtprulelooplimit",0);
	
	$obSettings->MaxMessageSize = hmailGetVar("maxmessagesize",0);
	
	$obSettings->SMTPDeliveryBindToIP = hmailGetVar("smtpdeliverybindtoip", "");
	$obSettings->MaxSMTPRecipientsInBatch = hmailGetVar("maxsmtprecipientsinbatch", "0");
	
	// RFC compliance
	$obSettings->AllowSMTPAuthPlain = hmailGetVar("AllowSMTPAuthPlain",0);
	$obSettings->DenyMailFromNull = hmailGetVar("AllowMailFromNull",0) == "0";
	$obSettings->AllowIncorrectLineEndings = hmailGetVar("AllowIncorrectLineEndings",0);
	$obSettings->DisconnectInvalidClients = hmailGetVar("DisconnectInvalidClients",0);
	$obSettings->MaxNumberOfInvalidCommands = hmailGetVar("MaxNumberOfInvalidCommands",0);
	
	$obSettings->SendStatistics = hmailGetVar("SendStatistics",0);
   
    $obSettings->AddDeliveredToHeader = hmailGetVar("AddDeliveredToHeader",0);
	
	$obSettings->MaxNumberOfMXHosts = hmailGetVar("MaxNumberOfMXHosts", 15);
	
	// Advanced
	$obSettings->SMTPConnectionSecurity = hmailGetVar("SMTPConnectionSecurity", 0) ? CONNECTION_SECURITY_STARTTLSOPTIONAL : CONNECTION_SECURITY_NONE;
}

// General
$maxsmtpconnections = $obSettings->MaxSMTPConnections;     
$welcomesmtp = $obSettings->WelcomeSMTP;     


// Delivery of email
$smtpnooftries = $obSettings->SMTPNoOfTries;     
$smtpminutesbetweentry = $obSettings->SMTPMinutesBetweenTry;
$HostName = $obSettings->HostName;

$smtprelayer = $obSettings->SMTPRelayer;
$smtprelayerport = $obSettings->SMTPRelayerPort;
$SMTPRelayerRequiresAuthentication = $obSettings->SMTPRelayerRequiresAuthentication;
$SMTPRelayerConnectionSecurity = $obSettings->SMTPRelayerConnectionSecurity;
$SMTPRelayerUsername = $obSettings->SMTPRelayerUsername;

$smtprulelooplimit = $obSettings->RuleLoopLimit;

$maxmessagesize = $obSettings->MaxMessageSize;

$smtpdeliverybindtoip = $obSettings->SMTPDeliveryBindToIP;
$maxsmtprecipientsinbatch = $obSettings->MaxSMTPRecipientsInBatch;

$AllowSMTPAuthPlain     = $obSettings->AllowSMTPAuthPlain;
$AllowMailFromNull      = $obSettings->DenyMailFromNull == "0";
$AllowIncorrectLineEndings     = $obSettings->AllowIncorrectLineEndings;
$DisconnectInvalidClients     = $obSettings->DisconnectInvalidClients;
$MaxNumberOfInvalidCommands     = $obSettings->MaxNumberOfInvalidCommands;
$SendStatistics         = $obSettings->SendStatistics;
$AddDeliveredToHeader = $obSettings->AddDeliveredToHeader;

$AllowSMTPAuthPlainChecked = hmailCheckedIf1($AllowSMTPAuthPlain);
$AllowMailFromNullChecked    = hmailCheckedIf1($AllowMailFromNull);
$AllowIncorrectLineEndingsChecked = hmailCheckedIf1($AllowIncorrectLineEndings);
$DisconnectInvalidClientsChecked = hmailCheckedIf1($DisconnectInvalidClients );
$SendStatisticsChecked = hmailCheckedIf1($SendStatistics );

$MaxNumberOfMXHosts = $obSettings->MaxNumberOfMXHosts;

$SMTPConnectionSecurity = $obSettings->SMTPConnectionSecurity == CONNECTION_SECURITY_STARTTLSOPTIONAL;
?>

<h1><?php EchoTranslation("SMTP")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "smtp");
      PrintHidden("action", "save");

   ?>
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>          
        
      	<table border="0" width="100%" cellpadding="5">
         	<tr>
         		<td width="30%"><?php EchoTranslation("Maximum number of simultaneous connections (0 for unlimited)")?></td>
         		<td width="70%"><input type="text" name="maxsmtpconnections" value="<?php echo $maxsmtpconnections?>" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Maximum number of simultaneous connections (0 for unlimited)")?>"></td>
         	</tr> 		
         	<tr>
         		<td><?php EchoTranslation("Welcome message")?></td>
         		<td><input type="text" name="welcomesmtp" value="<?php echo PreprocessOutput($welcomesmtp)?>" size="50"></td>
         	</tr> 	
         	<tr>
         		<td><?php EchoTranslation("Max message size (KB)")?></td>
         		<td><input type="text" name="maxmessagesize" value="<?php echo PreprocessOutput($maxmessagesize)?>" size="5" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Max message size (KB)")?>"></td>
         	</tr>	
         </table>
      </div>
      
      <div class="tabbertab">
         <h2><?php EchoTranslation("Delivery of e-mail")?></h2>   
        
         <table border="0" width="100%" cellpadding="5">
      	<tr>
		
      		<td><?php EchoTranslation("Number of retries")?></td>
      		<td><input type="text" name="smtpnooftries" value="<?php echo PreprocessOutput($smtpnooftries)?>" size="4" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Number of retries")?>"></td>
      	</tr>
         <tr>
      		<td width="30%"><?php EchoTranslation("Minutes between every retry")?></td>
      		<td width="70%"><input type="text" name="smtpminutesbetweentry" value="<?php echo PreprocessOutput($smtpminutesbetweentry)?>" size="4" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Minutes between every retry")?>"></td>
      	</tr>	
         <tr>
      		<td width="30%"><?php EchoTranslation("Local host name")?></td>
      		<td width="70%"><input type="text" name="HostName" value="<?php echo PreprocessOutput($HostName)?>" size="20" checkmessage="<?php EchoTranslation("HostName")?>"></td>
      	</tr>	   				
               
         <tr>
            <td colspan="2">
               <h3><?php EchoTranslation("SMTP relayer")?></h3>  
            </td> 
         </tr>
      	<tr>
      		<td><?php EchoTranslation("Remote host name")?></td>
      		<td><input type="text" name="smtprelayer" value="<?php echo PreprocessOutput($smtprelayer)?>" size="25"></td>
      	</tr>
      	<tr>
      		<td><?php EchoTranslation("Remote TCP/IP port")?></td>
      		<td><input type="text" name="smtprelayerport" value="<?php echo PreprocessOutput($smtprelayerport)?>" size="25" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Remote TCP/IP port")?>"></td>
      	</tr>
         <?php
            PrintCheckboxRow("SMTPRelayerRequiresAuthentication", "Server requires authentication", $SMTPRelayerRequiresAuthentication);
            PrintPropertyEditRow("SMTPRelayerUsername", "User name", $SMTPRelayerUsername);
            PrintPasswordEntry("SMTPRelayerPassword", "Password");
         ?>
	 				
		<tr>
			<td><?php EchoTranslation("Connection security")?></td>
			<td><select name="SMTPRelayerConnectionSecurity">
				<option value="<?php echo CONNECTION_SECURITY_NONE?>" <?php if ($SMTPRelayerConnectionSecurity == CONNECTION_SECURITY_NONE) echo "selected";?> ><?php EchoTranslation("None")?></a>
				<option value="<?php echo CONNECTION_SECURITY_STARTTLSOPTIONAL?>" <?php if ($SMTPRelayerConnectionSecurity == CONNECTION_SECURITY_STARTTLSOPTIONAL) echo "selected";?> ><?php EchoTranslation("STARTTLS (Optional)")?></a>
				<option value="<?php echo CONNECTION_SECURITY_STARTTLSREQUIRED?>" <?php if ($SMTPRelayerConnectionSecurity == CONNECTION_SECURITY_STARTTLSREQUIRED) echo "selected";?> ><?php EchoTranslation("STARTTLS (Required)")?></a>
				<option value="<?php echo CONNECTION_SECURITY_TLS?>" <?php if ($SMTPRelayerConnectionSecurity == CONNECTION_SECURITY_TLS) echo "selected";?> ><?php EchoTranslation("SSL/TLS")?></a>
			</select></td>
		</tr>
		
		 
      	</table>
      </div>
      
      <div class="tabbertab">
         <h2><?php EchoTranslation("Statistics")?></h2> 
   
         <table border="0" width="100%" cellpadding="5">
      	<tr>
      		<td width="50%"><?php EchoTranslation("Send statistics to hMailServer.com")?></td>
      		<td width="50%"><input type="checkbox" name="SendStatistics" value="1" <?php echo $SendStatisticsChecked?>></td>
      	</tr>	
         </table>	
         
      </div>
      
      <div class="tabbertab">
         <h2><?php EchoTranslation("RFC compliance")?></h2> 
  
         <table border="0" width="100%" cellpadding="5">
         	<tr>
         		<td width="50%"><?php EchoTranslation("Allow plain text authentication")?></td>
         		<td width="50%"><input type="checkbox" name="AllowSMTPAuthPlain" value="1" <?php echo $AllowSMTPAuthPlainChecked?>></td>
         	</tr>	   
         	<tr>
         		<td><?php EchoTranslation("Allow empty sender address")?></td>
         		<td><input type="checkbox" name="AllowMailFromNull" value="1" <?php echo $AllowMailFromNullChecked?>></td>
         	</tr>	
         	<tr>
         		<td><?php EchoTranslation("Allow incorrectly formatted line endings")?></td>
         		<td><input type="checkbox" name="AllowIncorrectLineEndings" value="1" <?php echo $AllowIncorrectLineEndingsChecked?>></td>
         	</tr>	   
        	   <tr>
         		<td><?php EchoTranslation("Disconnect client after too many invalid commands")?></td>
         		<td><input type="checkbox" name="DisconnectInvalidClients" value="1" <?php echo $DisconnectInvalidClientsChecked?>></td>
         	</tr>	   
        	   <tr>
         		<td><?php EchoTranslation("Maximum number of invalid commands")?></td>
         		<td><input type="text" name="MaxNumberOfInvalidCommands" value="<?php echo PreprocessOutput($MaxNumberOfInvalidCommands)?>" size="4" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Maximum number of invalid commands")?>"></td>
         	</tr>   			
      	</table>  
      </div>

      <div class="tabbertab">
      
         <h2><?php EchoTranslation("Advanced")?></h2>       
   
         <table border="0" width="100%" cellpadding="5">
      	<tr>
      		<td width="50%"><?php EchoTranslation("Bind to local IP address")?></td>
      		<td width="50%"><input type="text" name="smtpdeliverybindtoip" value="<?php echo PreprocessOutput($smtpdeliverybindtoip)?>" size="20" checkallownull="true" checktype="ipaddress" checkmessage="<?php EchoTranslation("Bind to local IP address")?>"></td>
      	</tr>	   
      	<tr>
      		<td><?php EchoTranslation("Maximum number of recipients in batch")?></td>
      		<td><input type="text" name="maxsmtprecipientsinbatch" value="<?php echo PreprocessOutput($maxsmtprecipientsinbatch)?>" size="4" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Maximum number of recipients in batch")?>"></td>
      	</tr>
		<?php
			PrintCheckboxRow("SMTPConnectionSecurity", "Use STARTTLS if available", $SMTPConnectionSecurity);
		?>		
		<tr>
      		<td><?php EchoTranslation("Rule Loop Limit")?></td>
      		<td><input type="text" name="smtprulelooplimit" value="<?php echo $smtprulelooplimit?>" size="3" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Rule Loop Limit")?>"></td>
      	</tr>		
         <?php
            PrintCheckboxRow("AddDeliveredToHeader", "Add Delivered-To header", $AddDeliveredToHeader);
			PrintPropertyEditRow("MaxNumberOfMXHosts", "Maximum number of recipient hosts", $MaxNumberOfMXHosts);
         ?>
		 
      	</table>
      </div>
    </div>

   <?php 
      PrintSaveButton();
   ?>          
    
</form>
