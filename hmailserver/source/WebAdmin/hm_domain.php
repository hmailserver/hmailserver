<?php

if (!defined('IN_WEBADMIN'))
   exit();
   

$domainid	= hmailGetVar("domainid",0,true);
$action	   = hmailGetVar("action","");

if (hmailGetAdminLevel() == 1 && ($domainid != hmailGetDomainID() || $action != "edit"))
	hmailHackingAttemp(); 

$admin_rights = (hmailGetAdminLevel()  === ADMIN_SERVER);

$domainname = "";
$domainactive = 1;
$domainpostmaster = "";
$domainmaxsize = 0;
$domainplusaddressingenabled = 0;
$domainplusaddressingcharacter = "+";
$domainantispamenablegreylisting = 1;
$domainmaxmessagesize = 0;

$AllocatedSize = 0;

$SignatureEnabled = 0;
$SignatureHTML = "";
$SignaturePlainText = "";
$SignatureMethod = 1;

$AddSignaturesToLocalMail = 1;
$AddSignaturesToReplies = 0;

$MaxNumberOfAccounts = 0;
$MaxNumberOfAliases  = 0;
$MaxNumberOfDistributionLists = 0;
$MaxAccountSize      = 0;

$MaxNumberOfAccountsEnabled = 0;
$MaxNumberOfAliasesEnabled  = 0;
$MaxNumberOfDistributionListsEnabled = 0;

$DKIMSignEnabled = 0;
$DKIMSignAliasesEnabled = 0;
$DKIMPrivateKeyFile = "";
$DKIMSelector = "";

$DKIMHeaderCanonicalizationMethod = 2;
$DKIMBodyCanonicalizationMethod = 2;
$DKIMSigningAlgorithm = 2;

$DomainID = 0;

if ($action == "edit")
{
   $obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);
   
   $domainname       = $obDomain->Name;
   $domainactive     = $obDomain->Active;
   $domainpostmaster = $obDomain->Postmaster;
   $domainmaxsize    = $obDomain->MaxSize;
   $AllocatedSize = $obDomain->AllocatedSize;
   $domainmaxmessagesize    = $obDomain->MaxMessageSize;
   
   $domainplusaddressingenabled = $obDomain->PlusAddressingEnabled;
   $domainplusaddressingcharacter = $obDomain->PlusAddressingCharacter;
   $domainantispamenablegreylisting = $obDomain->AntiSpamEnableGreylisting;
   
   $SignatureEnabled   = $obDomain->SignatureEnabled;
   $SignatureHTML  	   = $obDomain->SignatureHTML;
   $SignaturePlainText = $obDomain->SignaturePlainText;
   $SignatureMethod    = $obDomain->SignatureMethod;
   
   $AddSignaturesToLocalMail = $obDomain->AddSignaturesToLocalMail;
   $AddSignaturesToReplies  = $obDomain->AddSignaturesToReplies;
   
   $MaxAccountSize      = $obDomain->MaxAccountSize;
   
   $MaxNumberOfAccounts = $obDomain->MaxNumberOfAccounts;
   $MaxNumberOfAliases  = $obDomain->MaxNumberOfAliases;
   $MaxNumberOfDistributionLists = $obDomain->MaxNumberOfDistributionLists;
   $MaxNumberOfAccountsEnabled = $obDomain->MaxNumberOfAccountsEnabled;
   $MaxNumberOfAliasesEnabled  = $obDomain->MaxNumberOfAliasesEnabled;
   $MaxNumberOfDistributionListsEnabled = $obDomain->MaxNumberOfDistributionListsEnabled;
   
   $DKIMSignEnabled = $obDomain->DKIMSignEnabled;
   $DKIMSignAliasesEnabled = $obDomain->DomainAliases->Count > 0 ? $obDomain->DKIMSignAliasesEnabled : 0;
   $DKIMPrivateKeyFile = $obDomain->DKIMPrivateKeyFile;
   $DKIMSelector = $obDomain->DKIMSelector;

   $DKIMHeaderCanonicalizationMethod = $obDomain->DKIMHeaderCanonicalizationMethod;
   $DKIMBodyCanonicalizationMethod = $obDomain->DKIMBodyCanonicalizationMethod;
   $DKIMSigningAlgorithm = $obDomain->DKIMSigningAlgorithm;
   
   $DomainID = $obDomain->ID;
   
}

$domainactivechecked = hmailCheckedIf1($domainactive);
$domainplusaddressingenabledchecked = hmailCheckedIf1($domainplusaddressingenabled);
$domainantispamenablegreylistingchecked = hmailCheckedIf1($domainantispamenablegreylisting);

$SignatureEnabledChecked = hmailCheckedIf1($SignatureEnabled);
$AddSignaturesToLocalMailChecked = hmailCheckedIf1($AddSignaturesToLocalMail);
$AddSignaturesToRepliesChecked   = hmailCheckedIf1($AddSignaturesToReplies);

$MaxNumberOfAccountsEnabledChecked          = hmailCheckedIf1($MaxNumberOfAccountsEnabled);
$MaxNumberOfAliasesEnabledChecked           = hmailCheckedIf1($MaxNumberOfAliasesEnabled);
$MaxNumberOfDistributionListsEnabledChecked = hmailCheckedIf1($MaxNumberOfDistributionListsEnabled);
?>

     
<h1><?php EchoTranslation("Domain")?></h1>
<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
	  PrintHiddenCsrfToken();
      PrintHidden("page", "background_domain_save");
      PrintHidden("action", $action);
      PrintHidden("domainid", $DomainID);
   ?>
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>
   
      	<table border="0" width="100%" cellpadding="5">
            <tr>
      			<td width="30%"><?php EchoTranslation("Name")?></td>
      			<td width="70%">
         			<?php
         			$str_name = $obLanguage->String("Name");
                  
                  $domainname = PreprocessOutput($domainname);
                  
         			if ($admin_rights)
         			   echo "<input type=\"text\" name=\"domainname\" value=\"$domainname\" size=\"40\" checkallownull=\"false\" checkmessage=\"$str_name\">";
         			else
         				echo $domainname;
         			?>
               </td>			
      		</tr>
            
      		<tr>
      			<td><?php EchoTranslation("Enabled")?></td>
      			<td>
      			<?php
      			if ($admin_rights)
      			   echo "<input type=\"checkbox\" name=\"domainactive\" value=\"1\" $domainactivechecked>";
      			else
      			{
      				if ($domainactive == 1)
      				   echo $obLanguage->String("Yes");
      				else
      				   echo $obLanguage->String("No");				
      			}
      			?>			
      			</td>
      		</tr>

         </table>
      </div>
      
      <?php
         if (isset($obDomain) && hmailGetAdminLevel() == ADMIN_SERVER)
         {
         
         $str_delete = $obLanguage->String("Remove");
      ?>
      <div class="tabbertab">
         <h2><?php EchoTranslation("Names")?></h2>
   
         <table width="100%">
            <tr>
               <td width="70%">
                  <?php EchoTranslation("Name");?>
               </td>
               <td width="30%">
                  <!-- delete-column -->
               </td>
               <?php
                  $obDomainAliases = $obDomain->DomainAliases;
                  
                  for ($i = 0; $i < $obDomainAliases->Count; $i++)
                  {
                     $obDomainAlias = $obDomainAliases->Item($i);
                     $aliasid = $obDomainAlias->ID;
                     $name = $obDomainAlias->AliasName;
                     
                     ?>
                     <tr>
                     <td>
                        <?php echo PreprocessOutput($name);?>
                     </td>                     
                     <td>
                     <?php
                        echo "<a href=\"?page=background_domain_name_save&csrftoken=$csrftoken&action=delete&domainid=$domainid&aliasid=$aliasid\">$str_delete</a>";
                     ?>
                     </td>                     
                     </tr>
                     <?php
                  }
               ?>
            </tr>
         </table>
         <br/>
         <a href="?page=domain_aliasname&action=add&domainid=<?php echo $DomainID?>"><i><?php EchoTranslation("Add")?></i></a>
         
      </div>      
      <?php
         }
      ?>

      
      <div class="tabbertab">
         <h2><?php EchoTranslation("Signature")?></h2>   
         
         <table border="0" width="100%" cellpadding="5">
            <tr>
      			<td><?php EchoTranslation("Enabled")?></td>
      			<td>
      			<?php
      		   echo "<input type=\"checkbox\" name=\"SignatureEnabled\" value=\"1\" $SignatureEnabledChecked>";
      			?>			
      			</td>
            </tr>
         	<tr>
         		<td><?php EchoTranslation("Add signatures to local email")?></td>
         		<td><input type="checkbox" name="AddSignaturesToLocalMail" value="1" <?php echo $AddSignaturesToLocalMailChecked?>></td>
         	</tr>   	
         	<tr>
         		<td><?php EchoTranslation("Add signatures to replies")?></td>
         		<td><input type="checkbox" name="AddSignaturesToReplies" value="1" <?php echo $AddSignaturesToRepliesChecked?>></td>
         	</tr>   	   	
         	
      		<tr>
      			<td>&nbsp;</td>
      			<td>
         		   <select name="SignatureMethod" style="font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif">
         		      <option value="1" <?php if ($SignatureMethod == "1") echo "selected";?> ><?php EchoTranslation("Use signature if none has been specified in sender's account")?></option>
         		      <option value="2" <?php if ($SignatureMethod == "2") echo "selected";?> ><?php EchoTranslation("Overwrite account signature")?></option>
         		      <option value="3" <?php if ($SignatureMethod == "3") echo "selected";?> ><?php EchoTranslation("Append signature to account signature")?></option>
         		   </select>
      			</td>
      		</tr>   	
         	
      		<tr>
      			<td valign="top"><?php EchoTranslation("Plain text signature")?></td>
      			<td>
      				<textarea cols="50" rows="4" name="SignaturePlainText"><?php echo PreprocessOutput($SignaturePlainText)?></textarea>
      			</td>
      		</tr>
      		<tr>
      			<td valign="top"><?php EchoTranslation("HTML signature")?></td>
      			<td>
      				<textarea cols="50" rows="4" name="SignatureHTML"><?php echo PreprocessOutput($SignatureHTML)?></textarea>
      			</td>
      		</tr>				
      	</table>   	
   	</div>
      
      <div class="tabbertab">
         <h2><?php EchoTranslation("Limits")?></h2>   
      
         <table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td width="40%"><?php EchoTranslation("Maximum size (MB)")?></td>
      			<td width="60%">
      			<?php
      			$str_maxsizemb = $obLanguage->String("Maximum size (MB)");
               $domainmaxsize = PreprocessOutput($domainmaxsize);
               
      			if ($admin_rights)
      			   echo "<input type=\"text\" name=\"domainmaxsize\" value=\"$domainmaxsize\" size=\"8\" checkallownull=\"false\" checkmessage=\"$str_maxsizemb\">";
      			else
      				echo $domainmaxsize;
      			?>			
               
               <?php 
                  echo $obLanguage->String("Allocated size (MB)");
                  echo "&nbsp;";
                  echo "<input type=\"text\" name=\"dummy\" value=\"$AllocatedSize\" size=\"8\" readonly disabled>";
               
               ?>
      			</td>
      		</tr>
      		<tr>
      			<td><?php EchoTranslation("Max message size (KB)")?></td>
      			<td>
      			<?php
      			$str_warning = $obLanguage->String("Max message size (KB)");
      			
               $domainmaxmessagesize = PreprocessOutput($domainmaxmessagesize);
               
      			if ($admin_rights)
      			   echo "<input type=\"text\" name=\"domainmaxmessagesize\" value=\"$domainmaxmessagesize\" size=\"8\" checkallownull=\"false\" checkmessage=\"$str_warning\">";
      			else
      				echo $domainmaxmessagesize;
      			?>			
      			</td>
      		</tr>		
      		</tr>	   
      		<tr>
      			<td><?php EchoTranslation("Max size of accounts (MB)")?></td>
      			<td>
      			
      			<?php
      			$str_warning = $obLanguage->String("Max size of accounts (MB)");
      			
               $MaxAccountSize = PreprocessOutput($MaxAccountSize);
               
      			if ($admin_rights)
      			   echo "<input type=\"text\" name=\"MaxAccountSize\" value=\"$MaxAccountSize\" size=\"8\" checkallownull=\"false\" checkmessage=\"$str_warning\">";
      			else
      				echo $MaxAccountSize;
      			?>			
      			</td>
      		</tr>	     		
      		<tr>
      			<td><?php EchoTranslation("Max number of accounts")?></td>
      			<td>
      			<?php

               
      			if ($admin_rights)
      			   echo "<input type=\"checkbox\" name=\"MaxNumberOfAccountsEnabled\" value=\"1\" $MaxNumberOfAccountsEnabledChecked>";
      			else
      			   echo "<input type=\"checkbox\" name=\"MaxNumberOfAccountsEnabled\" value=\"1\" readonly disabled $MaxNumberOfAccountsEnabledChecked>";
      			
      			$str_warning = $obLanguage->String("Max number of accounts");
      			
               $MaxNumberOfAccounts = PreprocessOutput($MaxNumberOfAccounts);
               
      			if ($admin_rights)
      			   echo "<input type=\"text\" name=\"MaxNumberOfAccounts\" value=\"$MaxNumberOfAccounts\" size=\"8\" checkallownull=\"false\" checkmessage=\"$str_warning\">";
      			else
      				echo $MaxNumberOfAccounts;
      			?>			
      			</td>
      		</tr>	  
      		<tr>
      			<td><?php EchoTranslation("Max number of aliases")?></td>
      			<td>
      			<?php
      			
      			if ($admin_rights)
      			   echo "<input type=\"checkbox\" name=\"MaxNumberOfAliasesEnabled\" value=\"1\" $MaxNumberOfAliasesEnabledChecked>";
      			else
      			   echo "<input type=\"checkbox\" name=\"MaxNumberOfAliasesEnabled\" value=\"1\" readonly disabled $MaxNumberOfAliasesEnabledChecked>";

      			   
      			$str_warning = $obLanguage->String("Max number of aliases");
      			
               $MaxNumberOfAliases = PreprocessOutput($MaxNumberOfAliases);
               
      			if ($admin_rights)
      			   echo "<input type=\"text\" name=\"MaxNumberOfAliases\" value=\"$MaxNumberOfAliases\" size=\"8\" checkallownull=\"false\" checkmessage=\"$str_warning\">";
      			else
      				echo $MaxNumberOfAliases;
      			?>			
      			</td>
      		</tr>	   
      		<tr>
      			<td><?php EchoTranslation("Max number of distribution lists")?></td>
      			<td>
      			<?php
               
               
      			if ($admin_rights)
      			   echo "<input type=\"checkbox\" name=\"MaxNumberOfDistributionListsEnabled\" value=\"1\" $MaxNumberOfDistributionListsEnabledChecked>";
      			else
      			   echo "<input type=\"checkbox\" name=\"MaxNumberOfDistributionListsEnabled\" value=\"1\" readonly disabled $MaxNumberOfDistributionListsEnabledChecked>";

      			
      			$str_warning = $obLanguage->String("Max number of distribution lists");
      			
               $MaxNumberOfDistributionLists = PreprocessOutput($MaxNumberOfDistributionLists);
               
      			if ($admin_rights)
      			   echo "<input type=\"text\" name=\"MaxNumberOfDistributionLists\" value=\"$MaxNumberOfDistributionLists\" size=\"8\" checkallownull=\"false\" checkmessage=\"$str_warning\">";
      			else
      				echo $MaxNumberOfDistributionLists;
      			?>			
      			</td>
           </tr>
   	</table>
   </div>
   
    <div class="tabbertab">
         <h2><?php EchoTranslation("DKIM Signing")?></h2>   
      
         <table border="0" width="100%" cellpadding="5">
      		<?php
               PrintCheckboxRow("DKIMSignEnabled", $obLanguage->String("Enabled"), $DKIMSignEnabled);
               PrintCheckboxRow("DKIMSignAliasesEnabled", $obLanguage->String("Sign aliases"), $DKIMSignAliasesEnabled, $obDomain->DomainAliases->Count == 0);
               PrintPropertyEditRow("DKIMPrivateKeyFile", $obLanguage->String("Private key file"), $DKIMPrivateKeyFile, 50);
               PrintPropertyEditRow("DKIMSelector", $obLanguage->String("Selector"), $DKIMSelector, 20);
            ?>
            
            <tr>
               <td colspan="2">
                  Header method
               </td>
            </tr>
            <tr>
               <td>
                    <input type="radio" name="DKIMHeaderCanonicalizationMethod" value="1" <?php if ($DKIMHeaderCanonicalizationMethod == 1) echo "checked"?>> Simple
                    <input type="radio" name="DKIMHeaderCanonicalizationMethod" value="2" <?php if ($DKIMHeaderCanonicalizationMethod == 2) echo "checked"?>> Relaxed 
               </td>
               
            </tr> 
            
            <tr>
               <td colspan="2">
                  Body method
               </td>
            </tr>
            <tr>
               <td>
                    <input type="radio" name="DKIMBodyCanonicalizationMethod" value="1" <?php if ($DKIMBodyCanonicalizationMethod == 1) echo "checked"?>> Simple
                    <input type="radio" name="DKIMBodyCanonicalizationMethod" value="2" <?php if ($DKIMBodyCanonicalizationMethod == 2) echo "checked"?>> Relaxed 
               </td>
               
            </tr>             
            
            
            <tr>
               <td colspan="2">
                  Signing algorithm
               </td>
            </tr>
            <tr>
               <td>
                    <input type="radio" name="DKIMSigningAlgorithm" value="1" <?php if ($DKIMSigningAlgorithm == 1) echo "checked"?>> SHA1
                    <input type="radio" name="DKIMSigningAlgorithm" value="2" <?php if ($DKIMSigningAlgorithm == 2) echo "checked"?>> SHA256
               </td>
               
            </tr>             
            
   	 </table>
   </div>   
   
      <div class="tabbertab">
         <h2><?php EchoTranslation("Advanced")?></h2>   
         
         <table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>
            <?php PrintPropertyEditRow("domainpostmaster", $obLanguage->String("Catch-all address"), $domainpostmaster, 40, "email"); ?>
           	<tr>
      	      <td colspan="2">
      	         <h3><?php EchoTranslation("Plus addressing")?></h3>
      	      </td>
         	</tr>
         	   	
      		<tr>
      			<td><?php EchoTranslation("Enabled")?></td>
      			<td>
      			<?php
      		   echo "<input type=\"checkbox\" name=\"domainplusaddressingenabled\" value=\"1\" $domainplusaddressingenabledchecked>";
      			?>			
      			</td>
      		</tr>
         	
      		<tr>
      			<td><?php EchoTranslation("Character")?></td>
      			<td>
         		   <select name="domainplusaddressingcharacter" style="font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif">
         		      <option value="+" <?php if ($domainplusaddressingcharacter == "+") echo "selected";?> >+</option>
         		      <option value="-" <?php if ($domainplusaddressingcharacter == "-") echo "selected";?> >-</option>
         		      <option value="_" <?php if ($domainplusaddressingcharacter == "_") echo "selected";?> >_</option>
         		      <option value="%" <?php if ($domainplusaddressingcharacter == "%") echo "selected";?> >%</option>
         		   </select>
      			</td>
      		</tr>   	
         	
           	<tr>
      	      <td colspan="2">
      	         <h3><?php EchoTranslation("Greylisting")?></h3>
      	      </td>
         	</tr>
         	   	
      		<tr>
      			<td><?php EchoTranslation("Enabled")?></td>
      			<td>
      			   <input type="checkbox" name="domainantispamenablegreylisting" value="1" <?php echo $domainantispamenablegreylistingchecked?>>
      			</td>
      		</tr>
         </table>
   	</div>   
 </div>
 
<?php 
   PrintSaveButton();
?> 
</form>
