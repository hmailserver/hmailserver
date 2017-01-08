<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp(); // Only server admins can change this.

$ID 		   = hmailGetVar("ID",0);
$action	   = hmailGetVar("action","");

$obWhiteListAddresses	= $obBaseApp->Settings()->AntiSpam()->WhiteListAddresses;

if ($action == "edit")
{
   $obAddress = $obWhiteListAddresses->ItemByDBID($ID);
	
   $LowerIPAddress       = $obAddress->LowerIPAddress;
   $UpperIPAddress       = $obAddress->UpperIPAddress;
   $EmailAddress         = $obAddress->EmailAddress;
   $Description          = $obAddress->Description;
}
else 
{
   $LowerIPAddress = "";
   $UpperIPAddress = "";
   $EmailAddress = "";
   $Description = "";
}
?>

<h1><?php EchoTranslation("White listing")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_whitelistaddress_save");
      PrintHidden("action", "$action");
      PrintHidden("ID", "$ID");
   ?>
	
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>             
   
      	<table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td width="30%"><?php EchoTranslation("Description")?></td>
      			<td width="70%">
         		   <input type="text" name="Description" value="<?php echo PreprocessOutput($Description)?>" size="50" checkallownull="false" checkmessage="<?php EchoTranslation("Description")?>">
               </td>			
      		</tr>	

      		<tr>
      			<td><?php EchoTranslation("Lower IP")?></td>
      			<td>
         		   <input type="text" name="LowerIPAddress" value="<?php echo PreprocessOutput($LowerIPAddress)?>" size="20">
               </td>			
      		</tr>	

      		<tr>
      			<td><?php EchoTranslation("Upper IP")?></td>
      			<td>
         		   <input type="text" name="UpperIPAddress" value="<?php echo PreprocessOutput($UpperIPAddress)?>" size="20">
               </td>			
      		</tr>	
      		
      		<tr>
      			<td><?php EchoTranslation("E-mail address")?></td>
      			<td>
         		   <input type="text" name="EmailAddress" value="<?php echo PreprocessOutput($EmailAddress)?>" size="50">
               </td>			
      		</tr>					
      			
      	</table>
      </div>
   </div>

   <?php 
      PrintSaveButton();
   ?>   
</form>
