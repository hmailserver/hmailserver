<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();
$obLogging	= $obSettings->Logging();

$action	   = hmailGetVar("action","");

if($action == "save")
{
	$obLogging->Enabled		   = hmailGetVar("logenabled",0);
	$obLogging->LogApplication = hmailGetVar("logapplication",0); 
	$obLogging->LogSMTP		   = hmailGetVar("logsmtp",0);
	$obLogging->LogPOP3	   	= hmailGetVar("logpop3",0);
	$obLogging->LogIMAP	   	= hmailGetVar("logimap",0);
	$obLogging->LogTCPIP	      = hmailGetVar("logtcpip",0);
	$obLogging->LogDebug	      = hmailGetVar("logdebug",0);
	$obLogging->AwstatsEnabled  	= hmailGetVar("logawstats",0);
    $obLogging->KeepFilesOpen = hmailGetVar("KeepFilesOpen",0);
}

$logenabled= $obLogging->Enabled;
$logapplication= $obLogging->LogApplication;
$logsmtp= $obLogging->LogSMTP;
$logpop3= $obLogging->LogPOP3;
$logimap= $obLogging->LogIMAP;
$logtcpip= $obLogging->LogTCPIP;
$logdebug= $obLogging->LogDebug;     
$logawstats= $obLogging->AwstatsEnabled;
$KeepFilesOpen = $obLogging->KeepFilesOpen;

?>

<h1><?php EchoTranslation("Logging")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "logging");
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
            PrintCheckboxRow("logenabled", "Enabled", $logenabled);
            PrintCheckboxRow("logapplication", "Application", $logapplication);
            PrintCheckboxRow("logsmtp", "SMTP", $logsmtp);
            PrintCheckboxRow("logpop3", "POP3", $logpop3);
            PrintCheckboxRow("logimap", "IMAP", $logimap);
            PrintCheckboxRow("logdebug", "Debug", $logdebug);
            PrintCheckboxRow("logtcpip", "TCP/IP", $logtcpip);
            PrintCheckboxRow("logawstats", "AWStats", $logawstats);
            PrintCheckboxRow("KeepFilesOpen", "Keep files open", $KeepFilesOpen);
            
            
         ?>
         
      	</table>
      </div>
   </div>
   
   <?php
      PrintSaveButton();
   ?>   
</form>
