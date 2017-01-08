<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();

$action	   = hmailGetVar("action","");

if($action == "save")
{
	$obSettings->WelcomeIMAP= hmailGetVar("welcomeimap",0);
	$obSettings->MaxIMAPConnections = hmailGetVar("MaxIMAPConnections",0);
	
	$obSettings->IMAPSortEnabled  = hmailGetVar("IMAPSortEnabled",0);
	$obSettings->IMAPQuotaEnabled = hmailGetVar("IMAPQuotaEnabled",0);
	$obSettings->IMAPIdleEnabled  = hmailGetVar("IMAPIdleEnabled",0);
	$obSettings->IMAPACLEnabled  = hmailGetVar("IMAPACLEnabled",0);
   
   $obSettings->IMAPHierarchyDelimiter = hmailGetVar("IMAPHierarchyDelimiter","");
}

$welcomeimap = $obSettings->WelcomeIMAP;     
$MaxIMAPConnections = $obSettings->MaxIMAPConnections;

$IMAPSortEnabled  = $obSettings->IMAPSortEnabled;
$IMAPQuotaEnabled = $obSettings->IMAPQuotaEnabled;
$IMAPIdleEnabled  = $obSettings->IMAPIdleEnabled;
$IMAPACLEnabled  = $obSettings->IMAPACLEnabled;
$IMAPHierarchyDelimiter = $obSettings->IMAPHierarchyDelimiter;

?>

<h1><?php EchoTranslation("IMAP")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "imap");
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
               PrintPropertyEditRow("MaxIMAPConnections", "Maximum number of simultaneous connections (0 for unlimited)", $MaxIMAPConnections, 50);
               PrintPropertyEditRow("welcomeimap", "Welcome message", $welcomeimap, 50);
            ?>
       	</table>
      </div>
      <div class="tabbertab">
         <h2><?php EchoTranslation("Advanced")?></h2>      
	
       	<table border="0" width="100%" cellpadding="5">
            <tr>
               <td width="30%">
               </td>
               <td  width="70%">
               </td>
            </tr>
            <?php
               PrintCheckboxRow("IMAPSortEnabled", "IMAP Sort", $IMAPSortEnabled);
               PrintCheckboxRow("IMAPQuotaEnabled", "IMAP Quota", $IMAPQuotaEnabled);
               PrintCheckboxRow("IMAPIdleEnabled", "IMAP Idle", $IMAPIdleEnabled);
               PrintCheckboxRow("IMAPACLEnabled", "IMAP ACL", $IMAPACLEnabled);
            ?>
            
      		<tr>
      			<td><?php EchoTranslation("Hierarchy delimiter")?></td>
      			<td>
      				<select name="IMAPHierarchyDelimiter" style="font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif">
      					<option value="." <?php if ($IMAPHierarchyDelimiter == ".") echo "selected";?> >.</option>
      					<option value="\" <?php if ($IMAPHierarchyDelimiter == "\\") echo "selected";?> >\</option>
      					<option value="/" <?php if ($IMAPHierarchyDelimiter == "/") echo "selected";?> >/</option>
      				</select>
      		
      			</td>
      		</tr>              
      	</table>
      </div>
   </div>
   <?php
      PrintSaveButton();
   ?>
</form>