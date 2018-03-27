<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp();

$action	   = hmailGetVar("action","");   
   
$obSettings	= $obBaseApp->Settings();
$antiSpamSettings = $obSettings->AntiSpam;

if($action == "save")
{
	$antiSpamSettings->GreyListingEnabled = hmailGetVar("greylistingenabled", 0);
	$antiSpamSettings->GreyListingInitialDelay = hmailGetVar("greylistinginitialdelay", 0);
	$antiSpamSettings->GreyListingInitialDelete = hmailGetVar("greylistinginitialdelete", 0) * 24;
	$antiSpamSettings->GreyListingFinalDelete = hmailGetVar("greylistingfinaldelete", 0) * 24;

   $antiSpamSettings->BypassGreylistingOnSPFSuccess = hmailGetVar("BypassGreylistingOnSPFSuccess", 0); 
   $antiSpamSettings->BypassGreylistingOnMailFromMX = hmailGetVar("BypassGreylistingOnMailFromMX", 0); 
	
}

$greylistingenabled =   $antiSpamSettings->GreyListingEnabled;
$greylistinginitialdelay = $antiSpamSettings->GreyListingInitialDelay;
$greylistinginitialdelete = $antiSpamSettings->GreyListingInitialDelete / 24;
$greylistingfinaldelete = $antiSpamSettings->GreyListingFinalDelete / 24;
$greylistingenabledchecked = hmailCheckedIf1($greylistingenabled);
$BypassGreylistingOnSPFSuccess = $antiSpamSettings->BypassGreylistingOnSPFSuccess;
$BypassGreylistingOnMailFromMX = $antiSpamSettings->BypassGreylistingOnMailFromMX;

?>

<h1><?php EchoTranslation("Greylisting")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "greylisting");
      PrintHidden("action", "save");
   ?>   
   
   <div class="tabber">
     <div class="tabbertab">
         <h2><?php EchoTranslation("Greylisting")?></h2>    
               
      	<table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>
         	<tr>
         		<td><?php EchoTranslation("Enabled")?></td>
         		<td><input type="checkbox" name="greylistingenabled" value="1" <?php echo $greylistingenabledchecked?>></td>
         	</tr> 	
         	<tr>
         		<td><?php EchoTranslation("Minutes to defer delivery attempts")?></td>
         		<td><input type="text" name="greylistinginitialdelay" value="<?php echo PreprocessOutput($greylistinginitialdelay)?>" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Minutes to defer delivery attempts")?>"></td>
         	</tr>  	
         	<tr>
         		<td><?php EchoTranslation("Days before removing unused records")?></td>
         		<td><input type="text" name="greylistinginitialdelete" value="<?php echo PreprocessOutput($greylistinginitialdelete)?>" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Days before removing unused records")?>"></td>
         	</tr>  	
         	<tr>
         		<td><?php EchoTranslation("Days before removing used records")?></td>
         		<td><input type="text" name="greylistingfinaldelete" value="<?php echo PreprocessOutput($greylistingfinaldelete)?>" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Days before removing used records")?>"></td>
         	</tr>  
            <?php
               PrintCheckboxRow("BypassGreylistingOnSPFSuccess", "Bypass Greylisting on SPF Pass", $BypassGreylistingOnSPFSuccess);
               PrintCheckboxRow("BypassGreylistingOnMailFromMX", "Bypass Greylisting when message arrives from A or MX record.", $BypassGreylistingOnMailFromMX);
            ?>            
         </table>
     </div>
   </div>
   <?php
      PrintSaveButton();
   ?>
</form>