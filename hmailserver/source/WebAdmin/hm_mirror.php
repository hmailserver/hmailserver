<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();

$action	   = hmailGetVar("action","");

if($action == "save")
	$obSettings->MirrorEMailAddress= hmailGetVar("mirroremailaddress",0);

$mirroremailaddress = $obSettings->MirrorEMailAddress;      
?>

<h1><?php EchoTranslation("Mirror")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "mirror");
      PrintHidden("action", "save");
   ?>   
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>            
   
      	<table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>            
         <?php
            PrintPropertyEditRow("mirroremailaddress", "Mirror e-mail address", $mirroremailaddress, 40, "email");
         ?>
      	</table>
      </div>
   </div>   
   <?php
      PrintSaveButton();
   ?>
     
</form>