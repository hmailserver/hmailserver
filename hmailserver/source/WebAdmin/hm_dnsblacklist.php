<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // The user is not server administrator

$id	            = hmailGetVar("id",0);
$action	         	= hmailGetVar("action","");
	
$Active = false;
$DNSHost = "";
$ExpectedResult = "";
$RejectMessage = "";
$Score = 5;

if ($action == "edit")
{
   $dnsBlackList = $obBaseApp->Settings->AntiSpam->DNSBlackLists->ItemByDBID($id);

   $Active		= $dnsBlackList->Active;
   $DNSHost    = $dnsBlackList->DNSHost;
   $ExpectedResult    = $dnsBlackList->ExpectedResult;
   $RejectMessage    = $dnsBlackList->RejectMessage;
   $Score    = $dnsBlackList->Score;
}
?>

<h1><?php EchoTranslation("DNS blacklist")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_dnsblacklist_save");
      PrintHidden("action", $action);
      PrintHidden("id", $id);
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
               PrintCheckboxRow("Active", "Enabled", $Active);
               PrintPropertyEditRow("DNSHost", "DNS Host", $DNSHost);
               PrintPropertyEditRow("ExpectedResult", "Expected result", $ExpectedResult);
               PrintPropertyEditRow("RejectMessage", "Rejection message", $RejectMessage);
               PrintPropertyEditRow("Score", "Score", $Score, 5, "number");
            ?>

         </table>
       </div>
    </div>  
   <?php 
      PrintSaveButton();
   ?>
</form>
