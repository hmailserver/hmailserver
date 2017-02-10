<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();

$action	   = hmailGetVar("action","");

if($action == "save")
{
	$obSettings->MaxPOP3Connections= hmailGetVar("maxpop3connections",0);
	$obSettings->WelcomePOP3= hmailGetVar("welcomepop3",0);
}

$maxpop3connections = $obSettings->MaxPOP3Connections;     
$welcomepop3 = $obSettings->WelcomePOP3;     

?>

<h1><?php EchoTranslation("POP3")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "pop3");
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
            PrintPropertyEditRow("maxpop3connections", "Maximum number of simultaneous connections (0 for unlimited)", $maxpop3connections);
            PrintPropertyEditRow("welcomepop3", "Welcome message", $welcomepop3);
         ?>
      	</table>
      </div>
   </div>
   <?php
      PrintSaveButton();
   ?>
</form>