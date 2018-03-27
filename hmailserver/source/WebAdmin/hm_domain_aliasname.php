<?php

if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
   	hmailHackingAttemp(); // Only server can change these settings.   
   
$domainid	= hmailGetVar("domainid",0,true);
$action	   = hmailGetVar("action","");

?>

<h1><?php EchoTranslation("Alias")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_domain_name_save");
      PrintHidden("action", $action);
      PrintHidden("domainid", $domainid);
   ?>      
   
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>    
   
      	<table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td width="30%"><?php EchoTranslation("Name")?></td>
      			<td width="70%"><input type="text" name="aliasname" checkallownull="false"></td>
      		</tr>
      	</table>
      </div>
   </div>
   <?php 
      PrintSaveButton();
   ?>   
</form>
