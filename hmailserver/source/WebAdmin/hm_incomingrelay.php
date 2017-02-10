<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // The user is not server administrator

$relayid	            = hmailGetVar("relayid",0);
$action	         	= hmailGetVar("action","");
	
$relayname ="";
$relaylowerip = "0.0.0.0";
$relayupperip = "255.255.255.255";

if ($action == "edit")
{
   $obIncomingRelay = $obBaseApp->Settings->IncomingRelays->ItemByDBID($relayid);

   $relayname		= $obIncomingRelay->Name;
   $relaylowerip	= $obIncomingRelay->LowerIP;
   $relayupperip	= $obIncomingRelay->UpperIP;
}
?>

<h1><?php EchoTranslation("Incoming relay")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_incomingrelay_save");
      PrintHidden("action", $action);
      PrintHidden("relayid", $relayid);
   ?>      
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>         
   
      	<table border="0" width="100%" cellpadding="5">
         
      		<tr>
      			<td width="30%"><?php EchoTranslation("Name")?></td>
      			<td width="70%">
         			<input type="text" name="relayname" value="<?php echo PreprocessOutput($relayname)?>" size="30"  checkallownull="false" checkmessage="<?php EchoTranslation("Name")?>">
               </td>			
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("Lower IP")?></td>
               <td><input type="text" name="relaylowerip" value="<?php echo PreprocessOutput($relaylowerip)?>" size="25" checkallownull="false" checktype="ipaddress" checkmessage="<?php EchoTranslation("Lower IP")?>"></td>
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("Upper IP")?></td>
      			<td><input type="text" name="relayupperip" value="<?php echo PreprocessOutput($relayupperip)?>" size="25" checkallownull="false" checktype="ipaddress" checkmessage="<?php EchoTranslation("Upper IP")?>"></td>
      		</tr>		
         </table>
       </div>
    </div>  
   <?php 
      PrintSaveButton();
   ?>
</form>
