<?php
if (!defined('IN_WEBADMIN'))
   exit();

$domainid  = hmailGetVar("domainid",null);
$distributionlistid	         = hmailGetVar("distributionlistid",0);
$action	                     = hmailGetVar("action","");

$error_message	   = hmailGetVar("error_message","");

if (hmailGetAdminLevel() == 0)
   hmailHackingAttemp();

if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
	hmailHackingAttemp(); // Domain admin but not for this domain.

$obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);

$listaddress="";
$listactive=0;
$listrequiresmtpauth=0;


$Mode = 0;

if ($action == "edit")
{
   $obList = $obDomain->DistributionLists->ItemByDBID($distributionlistid);

   $listaddress = $obList->Address;
   $listactive = $obList->Active;
   $listrequiresmtpauth = $obList->RequireSMTPAuth;
   $Mode = $obList->Mode;
   $RequireSenderAddress = $obList->RequireSenderAddress;

   $listaddress = substr($listaddress, 0, strpos($listaddress, "@"));
   
}
else
{
   $RequireSenderAddress = "";
}

$domainname = $obDomain->Name;

$listactivechecked = hmailCheckedIf1($listactive);
$listrequiresmtpauthchecked = hmailCheckedIf1($listrequiresmtpauth);
?>

<h1><?php EchoTranslation("Distribution list")?></h1>

<?php
   if (strlen($error_message) > 0)
   {
      $error_message = $obLanguage->String($error_message);
      echo "<font color=\"red\"><b>$error_message</b></font><br><br>";
   }
?>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_distributionlist_save");
      PrintHidden("action", $action);
      PrintHidden("distributionlistid", $distributionlistid);
      PrintHidden("domainid", $domainid);
   ?>   
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>       
   
      	<table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td width="30%"><?php EchoTranslation("Address")?></td>
      			<td width="70%">
         				<input type="text" name="listaddress" value="<?php echo PreprocessOutput($listaddress)?>" size="30" checkallownull="false" checkmessage="<?php EchoTranslation("Address")?>"> @<?php echo $domainname?>
                  </td>			
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("Enabled")?></td>
      			<td><input type="checkbox" name="listactive" value="1" <?php echo $listactivechecked?>></td>
      		</tr>
         </table>
      </div>
      <div class="tabbertab">
         <h2><?php EchoTranslation("Security")?></h2>       
         <table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td><?php EchoTranslation("Mode")?></td>
      			<td>
      				<select name="Mode" style="font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif">
      					<option value="0" <?php if ($Mode == 0) echo "selected";?> ><?php EchoTranslation("Public - Anyone can send to the list")?></option>
      					<option value="1" <?php if ($Mode == 1) echo "selected";?> ><?php EchoTranslation("Membership - Only members can send to the list")?></option>
      					<option value="2" <?php if ($Mode == 2) echo "selected";?> ><?php EchoTranslation("Announcements - Only allow messages from the following address:")?></option>
      				</select>
      		
      			</td>
      		</tr>   	
      		<tr>
      			<td><?php EchoTranslation("Address")?></td>
      			<td>
         				<input type="text" name="RequireSenderAddress" value="<?php echo PreprocessOutput($RequireSenderAddress)?>" size="30">
                  </td>			
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("Require SMTP Authentication")?></td>
      			<td><input type="checkbox" name="listrequiresmtpauth" value="1" <?php echo $listrequiresmtpauthchecked?>></td>
      		</tr>

      	</table>
       </div>
    </div>
   <?php 
      PrintSaveButton();
   ?>    
</form>
