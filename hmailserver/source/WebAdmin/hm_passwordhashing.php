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

<script type="text/javascript">

function onAlgorithmChanged()
{
   // The cost parameters mean entirely different things to the two algorithms, so
   // whatever suited the previous one no longer applies to the one now selected.
   // Reset both to 0, which the server resolves to the recommended default for
   // whichever algorithm ends up configured.
   document.getElementById("PasswordHashMemoryCost").value = 0;
   document.getElementById("PasswordHashIterations").value = 0;

   updateMemoryCostState();
}

function updateMemoryCostState()
{
   // Read only rather than disabled - a disabled field is not submitted, and the
   // value would silently be lost.
   var algorithm = document.getElementById("PasswordHashAlgorithm").value;

   document.getElementById("PasswordHashMemoryCost").readOnly = (algorithm == "2");
}

</script>

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
                  <select name="PasswordHashAlgorithm" id="PasswordHashAlgorithm" onchange="onAlgorithmChanged();" style="font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif">
                     <option value="1" <?php if ($PasswordHashAlgorithm == "1") echo "selected";?> >Argon2id</option>
                     <option value="2" <?php if ($PasswordHashAlgorithm == "2") echo "selected";?> >PBKDF2-SHA256</option>
                  </select>
               </td>
            </tr>

			<?php
				PrintPropertyEditRow("PasswordHashMemoryCost", "Memory cost (kilobytes)", $PasswordHashMemoryCost, 10, "number");
				PrintPropertyEditRow("PasswordHashIterations", "Iterations", $PasswordHashIterations, 10, "number");
			?>

            <tr>
               <td></td>
               <td><small><?php EchoTranslation("0 selects the recommended value. Valid ranges: Argon2id memory 8192-1048576 KiB, iterations 2-20; PBKDF2-SHA256 iterations 100000-10000000")?></small></td>
            </tr>

			<?php
				PrintCheckboxRow("PasswordHashAutoUpgradeEnabled", "Upgrade stored passwords during logon", $PasswordHashAutoUpgradeEnabled);
			?>

      	</table>
      </div>
   </div>
   <?php
      PrintSaveButton();
   ?>

   <script type="text/javascript">
      updateMemoryCostState();
   </script>
</form>
