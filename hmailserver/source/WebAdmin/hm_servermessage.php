<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // The user is not server administrator

$messageid	            = hmailGetVar("messageid",0);
	
$obServerMessage = $obBaseApp->Settings->ServerMessages->ItemByDBID($messageid);

$messagename	= $obServerMessage->Name;
$messagetext	= $obServerMessage->Text;
?>

<h1><?php EchoTranslation("Server message")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_servermessage_save");
      PrintHidden("messageid", "$messageid");
   ?>
	
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>         
   
      	<table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td width="10%"><?php EchoTranslation("Name")?></td>
      			<td width="70%">
         			<?php echo PreprocessOutput($messagename)?>
               </td>			
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("Text")?></td>
               <td><textarea name="messagetext" rows="20" cols="100"><?php echo PreprocessOutput($messagetext)?></textarea>
      		</tr>
         </table>
       </div>
    </div>  
   <?php 
      PrintSaveButton();
   ?>
</form>
