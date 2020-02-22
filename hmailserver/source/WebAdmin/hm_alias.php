<?php

if (!defined('IN_WEBADMIN'))
   exit();

$domainid	= hmailGetVar("domainid",0,true);
$aliasid	   = hmailGetVar("aliasid",0, true);
$action	   = hmailGetVar("action","");

$error_message	   = hmailGetVar("error_message","");

if (hmailGetAdminLevel() == 0)
   hmailHackingAttemp();

if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
	hmailHackingAttemp(); // Domain admin but not for this domain.
				     
$obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);

$aliasname="";
$aliasvalue="";
$aliasactive=0;
				     
if ($action == "edit")
{
   $obAlias = $obDomain->Aliases->ItemByDBID($aliasid);  
   $aliasname = $obAlias->Name;
   $aliasvalue = $obAlias->Value;
   $aliasactive  = $obAlias->Active;
   $aliasname = substr($aliasname, 0, strpos($aliasname, "@"));
}

$domainname = $obDomain->Name;

$aliasactivechecked = hmailCheckedIf1($aliasactive);

?>

<h1><?php EchoTranslation("Alias")?></h1>

<?php
   if (strlen($error_message) > 0)
   {
      $error_message = $obLanguage->String($error_message);
      echo "<font color=\"red\"><b>$error_message</b></font><br><br>";
   }
?>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

	
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_alias_save");
      PrintHidden("action", $action);
      PrintHidden("domainid", $domainid);
      PrintHidden("aliasid", $aliasid);
   ?>
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>    
   
      	<table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td width="30%"><?php EchoTranslation("Redirect from")?></td>
      			<td width="70%"><input type="text" name="aliasname" value="<?php echo PreprocessOutput($aliasname)?>" checkallownull="false" checkmessage="<?php EchoTranslation("Redirect from")?>">@<?php echo $domainname?></td>
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("To")?></td>
      			<td><input type="text" name="aliasvalue" value="<?php echo PreprocessOutput($aliasvalue)?>"  checkallownull="false" checkmessage="<?php EchoTranslation("To")?>"></td>
      		</tr>

      		<tr>
      			<td><?php EchoTranslation("Enabled")?></td>
      			<td><input type="checkbox" name="aliasactive" value="1" <?php echo $aliasactivechecked?>></td>
      			</td>
      		</tr>
      	</table>
      </div>
   </div>
   <?php 
      PrintSaveButton();
   ?>   
</form>
