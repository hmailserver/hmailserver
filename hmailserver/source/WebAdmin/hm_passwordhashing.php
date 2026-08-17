<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();

$action	   = hmailGetVar("action","");

if($action == "save")
{
	$obSettings->PasswordHashAlgorithm = hmailGetVar("PasswordHashAlgorithm", 1);
	$obSettings->PasswordHashMemoryCost = hmailGetVar("PasswordHashMemoryCost", 0);
	$obSettings->PasswordHashIterations = hmailGetVar("PasswordHashIterations", 0);
	$obSettings->PasswordHashAutoUpgradeEnabled = hmailGetVar("PasswordHashAutoUpgradeEnabled", 0);
}

$PasswordHashAlgorithm  = $obSettings->PasswordHashAlgorithm;
$PasswordHashMemoryCost = $obSettings->PasswordHashMemoryCost;
$PasswordHashIterations = $obSettings->PasswordHashIterations;
$PasswordHashAutoUpgradeEnabled = $obSettings->PasswordHashAutoUpgradeEnabled;
?>

<h1><?php EchoTranslation("Password hashing")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "passwordhashing");
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
               <td><?php EchoTranslation("Algorithm")?></td>
               <td>
                  <select name="PasswordHashAlgorithm" style="font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif">
                     <option value="1" <?php if ($PasswordHashAlgorithm == "1") echo "selected";?> >Argon2id</option>
                     <option value="2" <?php if ($PasswordHashAlgorithm == "2") echo "selected";?> >PBKDF2-SHA256</option>
                  </select>
               </td>
            </tr>

			<?php
				PrintPropertyEditRow("PasswordHashMemoryCost", "Memory cost (kilobytes)", $PasswordHashMemoryCost, 10, "number");
				PrintPropertyEditRow("PasswordHashIterations", "Iterations", $PasswordHashIterations, 10, "number");
				PrintCheckboxRow("PasswordHashAutoUpgradeEnabled", "Upgrade stored passwords during logon", $PasswordHashAutoUpgradeEnabled);
			?>

            <tr>
               <td colspan="2">
                  <?php EchoTranslation("Zero means the recommended default for the selected algorithm: 19456 kilobytes and 2 iterations for Argon2id, 600000 iterations for PBKDF2-SHA256. The memory cost is not used by PBKDF2-SHA256.")?>
                  <br/><br/>
                  <?php EchoTranslation("When upgrading during logon is enabled, passwords stored using an older algorithm or a lower cost are re-hashed the next time the user logs on. Each account is upgraded once, but the first logon after a change costs one hash and one database write. Existing installations start with this switched off.")?>
               </td>
            </tr>

      	</table>
      </div>
   </div>
   <?php
      PrintSaveButton();
   ?>

</form>
