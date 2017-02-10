<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp(); // Domain admin but not for this domain.

$routeid	         = hmailGetVar("routeid",0);
$action	         = hmailGetVar("action","");

$obRoutes	= $obSettings->Routes();

$routetargetsmtpport  = 25;
$routenumberoftries  = 4;
$routemminutesbetweentry = 60;

$routedomainname = "";
$routetargetsmtphost= "";
$TreatRecipientAsLocalDomain = 0;
$TreatSenderAsLocalDomain = 0;
$ConnectionSecurity = 0;
$routerequiresauth=0;
$routeauthusername="";
$AllAddresses = true;

if ($action == "edit")
{
   $obRoute = $obRoutes->ItemByDBID($routeid);

   $routedomainname = $obRoute->DomainName;
   $routetargetsmtphost = $obRoute->TargetSMTPHost;
   $routetargetsmtpport = $obRoute->TargetSMTPPort;
   $TreatRecipientAsLocalDomain = $obRoute->TreatRecipientAsLocalDomain;
   $TreatSenderAsLocalDomain = $obRoute->TreatSenderAsLocalDomain;
   
   $routenumberoftries = $obRoute->NumberOfTries;
   $routemminutesbetweentry = $obRoute->MinutesBetweenTry;
   $routerequiresauth = $obRoute->RelayerRequiresAuth;
   $routeauthusername = $obRoute->RelayerAuthUsername;
   $ConnectionSecurity = $obRoute->ConnectionSecurity;
   $AllAddresses = $obRoute->AllAddresses;
}


?>

<h1><?php EchoTranslation("Route")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_route_save");
      PrintHidden("action", $action);
      PrintHidden("routeid", $routeid);
   ?>      
   
   
      <div class="tabber">
         <div class="tabbertab">
            <h2><?php EchoTranslation("General")?></h2>      
      
         	<table border="0" width="100%" cellpadding="5">
         		<tr>
         			<td width="30%"><?php EchoTranslation("Domain")?></td>
         			<td width="70%">
            			<input type="text" name="routedomainname" value="<?php echo PreprocessOutput($routedomainname)?>" size="30"  checkallownull="false" checkmessage="<?php EchoTranslation("Domain")?>">
                  </td>			
         		</tr>
         		<tr>
         			<td><?php EchoTranslation("Target SMTP host")?></td>
         			<td><input type="text" name="routetargetsmtphost" value="<?php echo PreprocessOutput($routetargetsmtphost)?>" size="25" checkallownull="false" checkmessage="<?php EchoTranslation("Target SMTP host")?>"></td>
         		</tr>
         		<tr>
         			<td><?php EchoTranslation("TCP/IP port")?></td>
         			<td><input type="text" name="routetargetsmtpport" value="<?php echo PreprocessOutput($routetargetsmtpport)?>" size="4" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("TCP/IP port")?>"></td>
         		</tr>
            </table>
         </div>
         
         <div class="tabbertab">
            <h2><?php EchoTranslation("Delivery")?></h2>            
         	<table border="0" width="100%" cellpadding="5">            
            	<tr>
            		<td width="30%"><?php EchoTranslation("Number of retries")?></td>
            		<td width="70%"><input type="text" name="routenumberoftries" value="<?php echo PreprocessOutput($routenumberoftries)?>" size="4" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Number of retries")?>"></td>
            	</tr> 
            	<tr>
            		<td><?php EchoTranslation("Minutes between every retry")?></td>
            		<td><input type="text" name="routemminutesbetweentry" value="<?php echo PreprocessOutput($routemminutesbetweentry)?>" size="4" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Minutes between every retry")?>"></td>
            	</tr> 
               <?php
                  PrintCheckboxRow("routerequiresauth", "Server requires authentication", $routerequiresauth);
               ?>
            	<tr>
            		<td><?php EchoTranslation("User name")?></td>
            		<td><input type="text" name="routeauthusername" value="<?php echo PreprocessOutput($routeauthusername)?>" size="30"></td>
            	</tr> 
            	<tr>
            		<td><?php EchoTranslation("Password")?></td>
            		<td><input type="password" name="routeauthpassword" value="" size="20" autocomplete="off"></td>
            	</tr>
				<tr>
					<td><?php EchoTranslation("Connection security")?></td>
					<td><select name="ConnectionSecurity">
						<option value="<?php echo CONNECTION_SECURITY_NONE?>" <?php if ($ConnectionSecurity == CONNECTION_SECURITY_NONE) echo "selected";?> ><?php EchoTranslation("None")?></a>
						<option value="<?php echo CONNECTION_SECURITY_STARTTLSOPTIONAL?>" <?php if ($ConnectionSecurity == CONNECTION_SECURITY_STARTTLSOPTIONAL) echo "selected";?> ><?php EchoTranslation("STARTTLS (Optional)")?></a>
						<option value="<?php echo CONNECTION_SECURITY_STARTTLSREQUIRED?>" <?php if ($ConnectionSecurity == CONNECTION_SECURITY_STARTTLSREQUIRED) echo "selected";?> ><?php EchoTranslation("STARTTLS (Required)")?></a>
						<option value="<?php echo CONNECTION_SECURITY_TLS?>" <?php if ($ConnectionSecurity == CONNECTION_SECURITY_TLS) echo "selected";?> ><?php EchoTranslation("SSL/TLS")?></a>
					</select></td>
				</tr>
               <?php 
                  PrintCheckboxRow("AllAddresses", "Deliver to all addresses", $AllAddresses);
                  
                     
   
               ?>
            	<tr>
            		<td><?php EchoTranslation("When sender matches route, treat sender domain as")?></td>
            		<td>
                  <input type="radio" name="TreatSenderAsLocalDomain" value="1" <?php if ($TreatSenderAsLocalDomain == 1) echo "checked"; ?> > <?php EchoTranslation("A local domain")?><br/>
                  <input type="radio" name="TreatSenderAsLocalDomain" value="0" <?php if ($TreatSenderAsLocalDomain == 0) echo "checked"; ?>> <?php EchoTranslation("An external domain")?>
                  </td>
            	</tr>        
            	<tr>
            		<td><?php EchoTranslation("When recipient matches route, treat recipient domain as")?></td>
            		<td>
                  <input type="radio" name="TreatRecipientAsLocalDomain" value="1" <?php if ($TreatRecipientAsLocalDomain == 1) echo "checked"; ?> > <?php EchoTranslation("A local domain")?><br/>
                  <input type="radio" name="TreatRecipientAsLocalDomain" value="0" <?php if ($TreatRecipientAsLocalDomain == 0) echo "checked"; ?>> <?php EchoTranslation("An external domain")?>
                  </td>
            	</tr>        
               </table>
          </div>
       </div>
               
               <?php
                  PrintSaveButton();
               ?>       
</form>
