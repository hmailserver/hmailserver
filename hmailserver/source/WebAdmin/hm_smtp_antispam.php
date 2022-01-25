<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();
$obAntiSpam	= $obSettings->AntiSpam;

$action	   = hmailGetVar("action","");

$antiSpamSettings = $obSettings->AntiSpam;

if($action == "save")
{
   $antiSpamSettings->SpamMarkThreshold = hmailGetVar("SpamMarkThreshold",0);
   $antiSpamSettings->SpamDeleteThreshold = hmailGetVar("SpamDeleteThreshold",0);

   $antiSpamSettings->SpamAssassinEnabled = hmailGetVar("SpamAssassinEnabled", 0);
   $antiSpamSettings->SpamAssassinHost = hmailGetVar("SpamAssassinHost", 0);
   $antiSpamSettings->SpamAssassinPort = hmailGetVar("SpamAssassinPort", 0);
   $antiSpamSettings->SpamAssassinMergeScore = hmailGetVar("SpamAssassinMergeScore", 0);
   $antiSpamSettings->SpamAssassinScore = hmailGetVar("SpamAssassinScore", 0);

   $antiSpamSettings->UseSPF= hmailGetVar("usespf",0);
   $antiSpamSettings->UseSPFScore = hmailGetVar("usespfscore",0);
   $antiSpamSettings->UseMXChecks= hmailGetVar("usemxchecks",0);
   $antiSpamSettings->UseMXChecksScore = hmailGetVar("usemxchecksscore",0);
   $antiSpamSettings->CheckHostInHelo = hmailGetVar("checkhostinhelo", 0);
   $antiSpamSettings->CheckHostInHeloScore = hmailGetVar("checkhostinheloscore", 0);
   $antiSpamSettings->CheckPTR = hmailGetVar("checkptr", 0);
   $antiSpamSettings->CheckPTRScore = hmailGetVar("checkptrscore", 0);

   $antiSpamSettings->AddHeaderSpam = hmailGetVar("AddHeaderSpam", 0);
   $antiSpamSettings->AddHeaderReason = hmailGetVar("AddHeaderReason", 0);
   $antiSpamSettings->PrependSubject = hmailGetVar("PrependSubject", 0);
   $antiSpamSettings->PrependSubjectText = hmailGetVar("PrependSubjectText", "");
   $antiSpamSettings->MaximumMessageSize = hmailGetVar("MaximumMessageSize", 0);

   $antiSpamSettings->DKIMVerificationEnabled = hmailGetVar("DKIMVerificationEnabled", 0);
   $antiSpamSettings->DKIMVerificationFailureScore = hmailGetVar("DKIMVerificationFailureScore", 0);
}

$SpamMarkThreshold = $antiSpamSettings->SpamMarkThreshold;
$SpamDeleteThreshold = $antiSpamSettings->SpamDeleteThreshold;
$MaximumMessageSize =   $antiSpamSettings->MaximumMessageSize;

$SpamAssassinEnabled = $antiSpamSettings->SpamAssassinEnabled;
$SpamAssassinHost = $antiSpamSettings->SpamAssassinHost;
$SpamAssassinPort = $antiSpamSettings->SpamAssassinPort;
$SpamAssassinMergeScore = $antiSpamSettings->SpamAssassinMergeScore;
$SpamAssassinScore = $antiSpamSettings->SpamAssassinScore;

$usespf = $antiSpamSettings->UseSPF;     
$usespfscore = $antiSpamSettings->UseSPFScore;     
$usemxchecks = $antiSpamSettings->UseMXChecks;     
$usemxchecksscore = $antiSpamSettings->UseMXChecksScore;     
$checkhostinhelo =   $antiSpamSettings->CheckHostInHelo;
$checkhostinheloscore =   $antiSpamSettings->CheckHostInHeloScore;
$checkptr = $antiSpamSettings->CheckPTR;
$checkptrscore = $antiSpamSettings->CheckPTRScore;

$DKIMVerificationEnabled = $antiSpamSettings->DKIMVerificationEnabled;
$DKIMVerificationFailureScore = $antiSpamSettings->DKIMVerificationFailureScore;

$AddHeaderSpam =   $antiSpamSettings->AddHeaderSpam;
$AddHeaderReason =   $antiSpamSettings->AddHeaderReason;
$PrependSubject =   $antiSpamSettings->PrependSubject;
$PrependSubjectText =   $antiSpamSettings->PrependSubjectText;

$AddHeaderSpamChecked = hmailCheckedIf1($AddHeaderSpam);
$AddHeaderReasonChecked = hmailCheckedIf1($AddHeaderReason);
$PrependSubjectChecked = hmailCheckedIf1($PrependSubject);
?>

<script language="javascript" type="text/javascript">
<!-- 
function HandleSpamAssassinTestResult()
{
   if (httpObject.readyState == 4)
   {
      if (httpObject.responseText == "1")
         document.getElementById('SpamAssassinTestResult').innerHTML = "<font color=green>Connected successfully.</font>";
      else
         document.getElementById('SpamAssassinTestResult').innerHTML = "<font color=red>Connection failed.</font>";
   }
}

function TestSpamAssassinConnection()
{
   httpObject = getHTTPObject();
   if (httpObject != null) 
   {
      document.getElementById('SpamAssassinTestResult').innerHTML = "";
      
      var url = "index.php?page=background_ajax_spamassassintest&csrftoken=<?php echo $csrftoken?>&Hostname="+ document.getElementById('SpamAssassinHost').value + "&Port=" + document.getElementById('SpamAssassinPort').value;
      
      httpObject.open("GET", url, true);
      httpObject.send(null);
      httpObject.onreadystatechange = HandleSpamAssassinTestResult;
      
   }
}
-->
</script>

<h1><?php EchoTranslation("Anti-spam")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "smtp_antispam");
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
                  PrintPropertyEditRow("SpamMarkThreshold", "Spam mark threshold", $SpamMarkThreshold);
                  ?>
            	<tr>
            		<td>&nbsp;&nbsp;&nbsp;<?php EchoTranslation("Add X-hMailServer-Spam")?></td>
            		<td><input type="checkbox" name="AddHeaderSpam" value="1" <?php echo $AddHeaderSpamChecked?>></td>
            	</tr> 		
            	<tr>
            		<td>&nbsp;&nbsp;&nbsp;<?php EchoTranslation("Add X-hMailServer-Reason")?></td>
            		<td><input type="checkbox" name="AddHeaderReason" value="1" <?php echo $AddHeaderReasonChecked?>></td>
            	</tr> 		  
            	<tr>
            		<td>&nbsp;&nbsp;&nbsp;<?php EchoTranslation("Add to message subject")?><br/>
            		</td>
            		<td>
            		   <input type="checkbox" name="PrependSubject" value="1" <?php echo $PrependSubjectChecked?>>
            		   <input type="text" name="PrependSubjectText" value="<?php echo PreprocessOutput($PrependSubjectText)?>">
            		
            		</td>
            	</tr> 	         
                  
                  
                  <?php
                  PrintPropertyEditRow("SpamDeleteThreshold", "Spam delete threshold", $SpamDeleteThreshold);
                  PrintPropertyEditRow("MaximumMessageSize", "Maximum message size to scan (KB)", $MaximumMessageSize, 6, "number");
               ?>
         	  

         </table>
      </div>

     <div class="tabbertab">
         <h2><?php EchoTranslation("Spam tests")?></h2>    
         
      	<table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>
            <?php
               PrintCheckboxRow("usespf", "Use SPF", $usespf);
               PrintPropertyEditRow("usespfscore", "Score", $usespfscore, 4, "number");
               PrintCheckboxRow("checkhostinhelo", "Check host in the HELO command", $checkhostinhelo);
               PrintPropertyEditRow("checkhostinheloscore", "Score", $checkhostinheloscore, 4, "number");
               PrintCheckboxRow("usemxchecks", "Check that sender has DNS-MX records", $usemxchecks);
               PrintPropertyEditRow("usemxchecksscore", "Score", $usemxchecksscore, 4, "number");
               PrintCheckboxRow("checkptr", "Check rDNS/PTR", $checkptr);
               PrintPropertyEditRow("checkptrscore", "Score", $checkptrscore, 4, "number", "small");
               PrintCheckboxRow("DKIMVerificationEnabled", "Verify DKIM-Signature header", $DKIMVerificationEnabled);
               PrintPropertyEditRow("DKIMVerificationFailureScore", "Score", $DKIMVerificationFailureScore, 4, "number");
            ?>
         </table>
     </div>
      
      <div class="tabbertab">
         <h2>SpamAssassin</h2>                  
      	<table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>
            <?php
               PrintCheckboxRow("SpamAssassinEnabled", "Use SpamAssassin", $SpamAssassinEnabled);
               PrintPropertyEditRow("SpamAssassinHost", "Host name", $SpamAssassinHost);
               PrintPropertyEditRow("SpamAssassinPort", "TCP/IP port", $SpamAssassinPort, 10, "number");
               PrintCheckboxRow("SpamAssassinMergeScore", "Use score from SpamAssassin", $SpamAssassinMergeScore);
               PrintPropertyEditRow("SpamAssassinScore", "Score", $SpamAssassinScore, 4, "number");
            ?>
            <tr>
               <td colspan="2">
                  <input type="button" value="<?php EchoTranslation("Test")?>" onclick="TestSpamAssassinConnection();">
                  <br/>
                  <br/>
                  <div id="SpamAssassinTestResult"></div>
               </td>
            </tr>
         </table>
     </div>

   </div>
   <?php 
      PrintSaveButton();
   ?>

   
</form>