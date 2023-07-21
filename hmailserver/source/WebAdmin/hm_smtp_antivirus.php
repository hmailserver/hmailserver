<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();
$obAntivirus	= $obSettings->AntiVirus();

$action	   = hmailGetVar("action","");

$str_delete = $obLanguage->String("Remove");

if($action == "save")
{
	$obAntivirus->Action            = hmailGetVar("avaction",0);
	$obAntivirus->NotifySender      = hmailGetVar("avnotifysender",0);
	$obAntivirus->NotifyReceiver    = hmailGetVar("avnotifyreceiver",0);
	$obAntivirus->MaximumMessageSize = hmailGetVar("MaximumMessageSize",0);
   
	$obAntivirus->ClamWinEnabled    = hmailGetVar("clamwinenabled",0);
	$obAntivirus->ClamWinExecutable = hmailGetVar("clamwinexecutable",0);
	$obAntivirus->ClamWinDBFolder   = hmailGetVar("clamwindbfolder",0);
	
	$obAntivirus->ClamAVEnabled    = hmailGetVar("ClamAVEnabled",0);
	$obAntivirus->ClamAVHost = hmailGetVar("ClamAVHost","");
	$obAntivirus->ClamAVPort   = hmailGetVar("ClamAVPort","");
	
	$obAntivirus->CustomScannerEnabled    = hmailGetVar("customscannerenabled",0);
	$obAntivirus->CustomScannerExecutable = hmailGetVar("customscannerexecutable",0);
	$obAntivirus->CustomScannerReturnValue = hmailGetVar("customscannerreturnvalue",0);	
	
   $obAntivirus->EnableAttachmentBlocking = hmailGetVar("EnableAttachmentBlocking",0);	
}



$avaction = $obAntivirus->Action;      
$avnotifysender = $obAntivirus->NotifySender;     
$avnotifyreceiver = $obAntivirus->NotifyReceiver;     
$MaximumMessageSize = $obAntivirus->MaximumMessageSize;     
 
$EnableAttachmentBlocking = $obAntivirus->EnableAttachmentBlocking;

$clamwinenabled    = $obAntivirus->ClamWinEnabled;     
$clamwinexecutable = $obAntivirus->ClamWinExecutable;     
$clamwindbfolder    = $obAntivirus->ClamWinDBFolder;     

$ClamAVEnabled    = $obAntivirus->ClamAVEnabled;     
$ClamAVHost = $obAntivirus->ClamAVHost;     
$ClamAVPort    = $obAntivirus->ClamAVPort;     

   
$customscannerenabled    = $obAntivirus->CustomScannerEnabled;     
$customscannerexecutable = $obAntivirus->CustomScannerExecutable;     
$customscannerreturnvalue    = $obAntivirus->CustomScannerReturnValue;  

$avactiondeletemailchecked = hmailCheckedIf1($avaction == 0);
$avactiondeletattachmentschecked = hmailCheckedIf1($avaction == 1);

?>

<script language="javascript" type="text/javascript">
<!-- 
function testVirusScanner(scannerType)
{
   httpObject = getHTTPObject();
   if (httpObject != null) 
   {
	  switch (scannerType)
	  {
	      case "ClamAV":
			  document.getElementById('ClamAVTestResult').innerHTML = "";
			  var clamAVHost = document.getElementById('ClamAVHost').value;
			  var clamAPort = document.getElementById('ClamAVPort').value;
			  var url = "index.php?page=background_ajax_virustest&csrftoken=<?php echo $csrftoken?>&TestType=ClamAV&Hostname="+ clamAVHost + "&Port=" + clamAPort;
			  sendRequest(url, "ClamAVTestResult");
		  break;
	      case "ClamWin":
			  document.getElementById('ClamWinTestResult').innerHTML = "";
			  var executable = document.getElementById('clamwinexecutable').value;
			  var database = document.getElementById('clamwindbfolder').value;
			  var url = "index.php?page=background_ajax_virustest&csrftoken=<?php echo $csrftoken?>&TestType=ClamWin&Executable="+ executable + "&DatabaseFolder=" + database;
			  sendRequest(url, "ClamWinTestResult");
		  break;
		  case "External":
			  document.getElementById('ExternalTestResult').innerHTML = "";
			  var executable = document.getElementById('customscannerexecutable').value;
			  var returnValue = document.getElementById('customscannerreturnvalue').value;
			  var url = "index.php?page=background_ajax_virustest&csrftoken=<?php echo $csrftoken?>&TestType=External&Executable="+ encodeURIComponent(executable) + "&ReturnValue=" + returnValue;
			  sendRequest(url, "ExternalTestResult");
			  break;
		  default:
			alert(scannerType);
		  break;
     }
		  
	  
   }
}

function sendRequest(url, responseDiv)
{
   httpObject.open("GET", url, true);
   httpObject.send(null);
   httpObject.onreadystatechange = function()
	 {
		printResponse(httpObject, responseDiv);
	} ;
}

function printResponse(httpObject, elementName)
{
   if (httpObject.readyState == 4)
   {
      if (httpObject.responseText == "1")
		document.getElementById(elementName).innerHTML = "<font color=green>Test succeeded.</font>"
	  else 
	    document.getElementById(elementName).innerHTML = "<font color=red>Test failed.</font>"
   }
}


-->
</script>

<h1><?php EchoTranslation("Anti-virus")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "smtp_antivirus");
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
      	<tr>
      		<td><?php EchoTranslation("When a virus is found")?></td>
      		<td><input type="radio" name="avaction" value="0" <?php echo $avactiondeletemailchecked?>> <?php EchoTranslation("Delete e-mail")?></td>
      	</tr>  	
      	<tr>
      		<td></td>
      		<td><input type="radio" name="avaction" value="1" <?php echo $avactiondeletattachmentschecked?>> <?php EchoTranslation("Delete attachments")?></td>
      	</tr>	
         <?php
            PrintPropertyEditRow("MaximumMessageSize", "Maximum message size to virus scan (KB)", $MaximumMessageSize, 10);
         ?>
         </table>
      </div>
      
      <div class="tabbertab">
			<h2>ClamWin</h2>          
			 <table border="0" width="100%" cellpadding="5">
			 <tr>
				<th width="30%"></th>
				<th width="70%"></th>
			 </tr>	
			<?php
				PrintCheckboxRow("clamwinenabled", "Enabled", $clamwinenabled);
				PrintPropertyEditRow("clamwinexecutable", "ClamScan executable", $clamwinexecutable, 60);
				PrintPropertyEditRow("clamwindbfolder", "Path to ClamScan database", $clamwindbfolder, 60);
			?>	
			<tr>
				<td colspan="2">
					<input type="button" value="<?php EchoTranslation("Test")?>" onclick="testVirusScanner('ClamWin');">
					<br/>
					<br/>
					<div id="ClamWinTestResult"></div>
				</td>
			 </tr>		
         </table>
      </div>

	  <div class="tabbertab">
         <h2>ClamAV</h2>          
         <table border="0" width="100%" cellpadding="5">
			 <tr>
				<th width="30%"></th>
				<th width="70%"></th>
			 </tr>	
			<?php
				PrintCheckboxRow("ClamAVEnabled", "Use ClamAV", $ClamAVEnabled);
				PrintPropertyEditRow("ClamAVHost", "Host name", $ClamAVHost);
				PrintPropertyEditRow("ClamAVPort", "TCP/IP port", $ClamAVPort, 5, "number");
			?>
			<tr>
				<td colspan="2">
					<input type="button" value="<?php EchoTranslation("Test")?>" onclick="testVirusScanner('ClamAV');">
					<br/>
					<br/>
					<div id="ClamAVTestResult"></div>
				</td>
			 </tr>
         </table>
      </div>
	  
      <div class="tabbertab">
         <h2><?php EchoTranslation("External virus scanner")?></h2>        
         <table border="0" width="100%" cellpadding="5">
            <tr>
            <th width="30%"></th>
            <th width="70%"></th>
         </tr>	
		<?php
			PrintCheckboxRow("customscannerenabled", "Enabled", $customscannerenabled);
			PrintPropertyEditRow("customscannerexecutable", "Scanner executable", $customscannerexecutable, 60);
			PrintPropertyEditRow("customscannerreturnvalue", "Return value", $customscannerreturnvalue, 5, "number");
		?>
			<tr>
				<td colspan="2">
					<input type="button" value="<?php EchoTranslation("Test")?>" onclick="testVirusScanner('External');">
					<br/>
					<br/>
					<div id="ExternalTestResult"></div>
				</td>
			 </tr>		
      </table>
     </div>
     
      <div class="tabbertab">
         <h2><?php EchoTranslation("Block attachments")?></h2>        
         <table border="0" width="400" cellpadding="5">
            <?php               
               PrintCheckboxRow("EnableAttachmentBlocking", "Block attachments with the following extensions:", $EnableAttachmentBlocking);
            ?>
         </table>
         
         <table border="0" width="400" cellpadding="5">
            <tr>
               <td><i><?php EchoTranslation("Name")?></i></td>
               <td><i><?php EchoTranslation("Description")?></i></td>
               <td><!-- delete column --></td>
            </tr>
            
               <?php
                  $blockedAttachments = $obAntivirus->BlockedAttachments;
                  
                  for ($i = 0; $i < $blockedAttachments->Count; $i++)
                  {
                     $blockedAttachment = $blockedAttachments->Item($i);
                     $id = $blockedAttachment->ID;
                     $wildcard = $blockedAttachment->Wildcard;
                     $description= $blockedAttachment->Description;
                     
                     ?>
                     <tr>
                        <td><a href="?page=blocked_attachment&action=edit&id=<?php echo $id?>"><?php echo PreprocessOutput($wildcard);?></a></td>                     
                        <td><?php echo PreprocessOutput($description);?></td>                     
                        <td>
                        <?php
                           echo "<a href=\"?page=background_blocked_attachment_save&csrftoken=$csrftoken&action=delete&id=$id\">$str_delete</a>";
                        ?>
                        </td>                     
                     </tr>
                     <?php
                  }
               ?>            
         </table>
         <a href="?page=blocked_attachment&action=add"><i><?php EchoTranslation("Add")?></i></a>
      
     </div>     
    </div>
   <?php 
      PrintSaveButton();
   ?>

</form>