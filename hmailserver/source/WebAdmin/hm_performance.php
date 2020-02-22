<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();
$obCache    = $obSettings->Cache();

$action	   = hmailGetVar("action","");

if($action == "save")
{
	$obCache->Enabled		            = hmailGetVar("cacheenabled",0);
	$obCache->DomainCacheTTL         = hmailGetVar("cachedomainttl",0); 
	$obCache->AccountCacheTTL		   = hmailGetVar("cacheaccountttl",0);
	
   $obCache->AliasCacheTTL            = hmailGetVar("cachealiasttl",0);
   $obCache->DistributionListCacheTTL = hmailGetVar("cachedistributionlistttl",0);
	
	$obSettings->TCPIPThreads        = hmailGetVar("tcpipthreads", 0);
	$obSettings->MaxDeliveryThreads  = hmailGetVar("maxdeliverythreads", 0);
	$obSettings->MaxAsynchronousThreads = hmailGetVar("MaxAsynchronousThreads", 0);
   $obSettings->WorkerThreadPriority = hmailGetVar("workerthreadpriority", 0);
   
   
   $obSettings->MessageIndexing->Enabled = hmailGetVar("MessageIndexingEnabled", 0);
}
else if ($action == "ClearMessageIndexingCache")
{
   $obSettings->MessageIndexing->Clear();
}

$cacheenabledchecked = hmailCheckedIf1($obCache->Enabled);
    
$cachedomainttl = $obCache->DomainCacheTTL;
$cacheaccountttl = $obCache->AccountCacheTTL;
$cachedomainhitrate = $obCache->DomainHitRate;
$cacheaccounthitrate = $obCache->AccountHitRate;
$cachealiashitrate = $obCache->AliasHitRate;
$cachealiasttl     = $obCache->AliasCacheTTL;
$cachedistributionlisthitrate = $obCache->DistributionListHitRate;
$cachedistributionlistttl = $obCache->DistributionListCacheTTL;

$tcpipthreads           = $obSettings->TCPIPThreads;
$maxdeliverythreads     = $obSettings->MaxDeliveryThreads;
$MaxAsynchronousThreads     = $obSettings->MaxAsynchronousThreads;
$workerthreadpriority   = $obSettings->WorkerThreadPriority;

$obMessageIndexingSettings  = $obSettings->MessageIndexing;

$MessageIndexingEnabled = $obMessageIndexingSettings->Enabled;

$TotalMessageCount = $obMessageIndexingSettings->TotalMessageCount;
$TotalIndexedCount = $obMessageIndexingSettings->TotalIndexedCount;

?>

<script type="text/javascript">
   function ClearMessageIndexingCache()
   {
      document.forms["mainform"].elements["action"].value = "ClearMessageIndexingCache";
      document.forms["mainform"].submit();
   }
</script>

<h1><?php EchoTranslation("Performance")?></h1>

<form action="index.php" method="post" name="mainform" onSubmit="return formCheck(this);">
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "performance");
      PrintHidden("action", "save");
   ?>   
   
   <div class="tabber">
      <div class="tabbertab">
          <h2><?php EchoTranslation("Cache")?></h2>            
   
      	<table border="0" width="100%" cellpadding="5">
         	<tr>
         		<td width="30%"><?php EchoTranslation("Enabled")?></td>
         		<td width="70%" colspan="2"><input type="checkbox" name="cacheenabled" value="1" <?php echo $cacheenabledchecked?>></td>
               <td></td>
         	</tr>   
         	<tr>
         		<td><i><?php EchoTranslation("Type")?></i></td>
         		<td><i><?php EchoTranslation("Time to live (seconds)")?></i></td>
         		<td><i><?php EchoTranslation("Hit rate")?></i></td>
         	</tr>   
         	<tr>
         		<td valign="top"><?php EchoTranslation("Domain")?></td>
         		<td><input type="text" name="cachedomainttl" value="<?php echo PreprocessOutput($cachedomainttl)?>" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Domain")?>"></td>
         		<td><?php echo $cachedomainhitrate?></td>
         	</tr>  	
         	<tr>
         		<td valign="top"><?php EchoTranslation("Account")?></td>
         		<td><input type="text" name="cacheaccountttl" value="<?php echo PreprocessOutput($cacheaccountttl)?>" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Account")?>">
         		<td><?php echo $cacheaccounthitrate?></td>
         		</td>
         	</tr>  	
         	<tr>
         		<td valign="top"><?php EchoTranslation("Alias")?></td>
         		<td><input type="text" name="cachealiasttl" value="<?php echo PreprocessOutput($cachealiasttl)?>" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Alias")?>"></td>
         		<td><?php echo $cachealiashitrate?></td>
         	</tr>  	
         	<tr>
         		<td valign="top"><?php EchoTranslation("Distribution list")?></td>
         		<td><input type="text" name="cachedistributionlistttl" value="<?php echo PreprocessOutput($cachedistributionlistttl)?>" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Distribution list")?>">
         		<td><?php echo $cachedistributionlisthitrate?></td>
         		</td>
         	</tr>    	
         </table>
      </div>
     <div class="tabbertab">
         <h2><?php EchoTranslation("Threading")?></h2>            
  
      	<table border="0" width="100%" cellpadding="5">
         	<tr>
         		<td size="30%" valign="top"><?php EchoTranslation("Max number of command threads")?></td>
         		<td size="70%"><input type="text" name="tcpipthreads" value="<?php echo PreprocessOutput($tcpipthreads)?>" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Max number of command threads")?>"></td>
         	</tr>  	
         	<tr>
         		<td valign="top"><?php EchoTranslation("Delivery threads")?></td>
         		<td><input type="text" name="maxdeliverythreads" value="<?php echo PreprocessOutput($maxdeliverythreads)?>" checkallownull="false" checktype="number" checkmessage="<?php EchoTranslation("Delivery threads")?>"></td>
         	</tr> 
            <?php
               PrintPropertyEditRow("MaxAsynchronousThreads", "Max number of asynchronous task threads", $MaxAsynchronousThreads, 4, "number");
            ?>
         	<tr>
         		<td valign="top"><?php EchoTranslation("Worker thread priority")?></td>
         		<td>  
         		   
         		   <select name="workerthreadpriority" style="font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif">
         		      <option value="2" <?php if ($workerthreadpriority == "2") echo "selected";?> >Highest</option>
         		      <option value="1" <?php if ($workerthreadpriority == "1") echo "selected";?> >Above normal</option>
         		      <option value="0" <?php if ($workerthreadpriority == "0") echo "selected";?> >Normal</option>
         		      <option value="-1" <?php if ($workerthreadpriority == "-1") echo "selected";?> >Below normal</option>
         		      <option value="-2" <?php if ($workerthreadpriority == "-2") echo "selected";?> >Lowest</option>
         		      <option value="-15" <?php if ($workerthreadpriority == "-15") echo "selected";?> >Idle</option>
         		   </select>
         		</td>
         	</tr>
      	</table>
      </div>
      <div class="tabbertab">
          <h2><?php EchoTranslation("Message indexing")?></h2>            
         	<table border="0" width="100%" cellpadding="5">
         
            <?php
               PrintCheckboxRow("MessageIndexingEnabled", "Enabled", $MessageIndexingEnabled);
            ?>
         	<tr>
         		<td size="30%"><b><?php EchoTranslation("Status")?></b></td>
         		<td size="70%"><?php echo $TotalIndexedCount . " / ". $TotalMessageCount;?></td>
         	</tr>   

         	<tr>
         		<td size="30%"><input type="button" name="MessageIndexingClear" value="<?php EchoTranslation("Clear")?>" onclick="ClearMessageIndexingCache();" > </b></td>
         		<td size="70%"></td>
         	</tr> 
            
            </table>
      </div>
   </div>
   <?php 
      PrintSaveButton();
   ?>   
</form>