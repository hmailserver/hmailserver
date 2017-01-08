<?php
if (!defined('IN_WEBADMIN'))
   exit();

   
$distributionlistid 	= hmailGetVar("distributionlistid",0);
$recipientid	      = hmailGetVar("recipientid",0);
$domainid	      = hmailGetVar("domainid",0,true);
$action	   = hmailGetVar("action","");

if (hmailGetAdminLevel() == 0)
   hmailHackingAttemp();

if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
	hmailHackingAttemp(); // Domain admin but not for this domain.

$recipientaddress = "";

if ($action == "edit")
{
   $obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);
   $obList = $obDomain->DistributionLists->ItemByDBID($distributionlistid);
   $obRecipient = $obList->Recipients->ItemByDBID($recipientid);

   $recipientaddress = $obRecipient->RecipientAddress;
}

?>

<h1><?php EchoTranslation("Address")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_distributionlist_recipient_save");
      PrintHidden("action", $action);
      PrintHidden("distributionlistid", $distributionlistid);
      PrintHidden("domainid", $domainid);
      PrintHidden("recipientid", $recipientid);
   ?>      
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>          
   
      	<table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td width="30%"><?php EchoTranslation("Address")?></td>
      			<td width="70%">
         			<input type="text" name="recipientaddress" value="<?php echo PreprocessOutput($recipientaddress)?>" size="30"  checkallownull="false" checktype="email" checkmessage="<?php EchoTranslation("Address")?>">
               </td>			
      		</tr>

        </table>
      </div>
   </div>
   <?php 
      PrintSaveButton();
   ?>      
      
</form>
