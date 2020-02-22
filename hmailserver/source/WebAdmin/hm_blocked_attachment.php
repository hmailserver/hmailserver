<?php

if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
   	hmailHackingAttemp(); // Only server can change these settings.   
   
$action	    = hmailGetVar("action","");
$id          = hmailGetVar("id","");

$wildcard = "";
$description = "";

if ($action == "edit")
{
   $obSettings	= $obBaseApp->Settings();
   $obAntivirus	= $obSettings->AntiVirus();
   $blockedAttachment = $obAntivirus->BlockedAttachments->ItemByDBID($id);
   
   $wildcard = $blockedAttachment->Wildcard;
   $description = $blockedAttachment->Description;
} 

?>

<h1><?php EchoTranslation("Blocked attachment")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_blocked_attachment_save");
      PrintHidden("action", $action);
      PrintHidden("id", $id);
   ?>
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>    
   
      	<table border="0" width="100%" cellpadding="5">
      		<?php
               PrintPropertyEditRow("wildcard", "Wildcard", $wildcard, 10);
               PrintPropertyAreaRow("description", "Description", $description);
               
               
            ?>           
            
      	</table>
      </div>
   </div>
   <?php 
      PrintSaveButton();
   ?>   
</form>
