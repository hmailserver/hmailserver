<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();
$obBackup    = $obSettings->Backup();

$action	   = hmailGetVar("action","");

if($action == "save")
{
	$obBackup->Destination = hmailGetVar("backupdestination",0);
	$obBackup->BackupSettings = hmailGetVar("backupsettings",0);
	$obBackup->BackupDomains = hmailGetVar("backupdomains",0);
	$obBackup->BackupMessages = hmailGetVar("backupmessages",0);
	$obBackup->CompressDestinationFiles = hmailGetVar("backupcompress",0);
}
elseif ($action == "startbackup")
{
   $obBaseApp->BackupManager->StartBackup();  
}

$backupdestination = $obBackup->Destination;
$backupsettings = $obBackup->BackupSettings;
$backupdomains = $obBackup->BackupDomains;
$backupmessages = $obBackup->BackupMessages;
$backupcompress = $obBackup->CompressDestinationFiles;

$backupsettingschecked = hmailCheckedIf1($backupsettings);
$backupdomainschecked = hmailCheckedIf1($backupdomains);
$backupmessageschecked = hmailCheckedIf1($backupmessages);
$backupcompresschecked = hmailCheckedIf1($backupcompress);

?>

<h1><?php EchoTranslation("Backup")?></h1>

   <div class="tabber">
      <div class="tabbertab">
          <h2><?php EchoTranslation("General")?></h2>            
   
         <form action="index.php" method="post" onSubmit="return formCheck(this);">
            <?php
			   PrintHiddenCsrfToken();
               PrintHidden("page", "backup");
               PrintHidden("action", "save");
            ?>   
   
         	<table border="0" width="100%" cellpadding="5">
         	<tr>
         		<td width="20%"><?php EchoTranslation("Destination")?></td>
         		<td width="80%"><input type="text" name="backupdestination" value="<?php echo PreprocessOutput($backupdestination)?>" size="50"></td>
         	</tr>   
         	<tr>
         	      <td colspan="2">
         	         <hr noshade style="height: 1px; border: 1px solid #eeeeee;">
         	         <br/>
         	         <b><?php EchoTranslation("Backup")?></b>
         	      </td>
         	</tr>	
         	<tr>
         		<td><?php EchoTranslation("Settings")?></td>
         		<td><input type="checkbox" name="backupsettings" value="1" <?php echo $backupsettingschecked?>></td>
         	</tr>   
         	<tr>
         		<td><?php EchoTranslation("Domains")?></td>
         		<td><input type="checkbox" name="backupdomains" value="1" <?php echo $backupdomainschecked?>></td>
         	</tr>   
         	<tr>
         		<td><?php EchoTranslation("Messages")?></td>
         		<td><input type="checkbox" name="backupmessages" value="1" <?php echo $backupmessageschecked?>></td>
         	</tr>   
         	<tr>
         		<td><?php EchoTranslation("Compress files")?></td>
         		<td><input type="checkbox" name="backupcompress" value="1" <?php echo $backupcompresschecked?>></td>
         	</tr>   		
         	</table>
            
            <?php 
               PrintSaveButton();
            ?>             
         </form>
      </div>
      
      <div class="tabbertab">
          <h2><?php EchoTranslation("Actions")?></h2>      
          
         <form action="index.php" method="post" onSubmit="return formCheck(this);">
           
            <?php
			   PrintHiddenCsrfToken();
               PrintHidden("page", "backup");
               PrintHidden("action", "startbackup");
            ?>   
            
            <table border="0" width="100%" cellpadding="5">
         	<tr>
         		<td colspan="1">  
         		   <br/>
         		   <input type="submit" value="Start">
         		</td>
         	</tr>		
         	</table>
         </form>
      </div>
   </div>
        