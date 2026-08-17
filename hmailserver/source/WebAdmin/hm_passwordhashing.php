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

// Must match PasswordHasher::Constants in the server.
var argon2idMemoryCost = 19456;
var argon2idIterations = 2;
var pbkdf2Iterations = 600000;

function onAlgorithmChanged()
{
   // The cost parameters mean entirely different things to the two algorithms, so
   // whatever suited the previous one is replaced by the defaults for the one now
   // selected. PBKDF2-SHA256 has no memory cost at all.
   var algorithm = document.getElementById("PasswordHashAlgorithm").value;

   if (algorithm == "2")
   {
      document.getElementById("PasswordHashMemoryCost").value = 0;
      document.getElementById("PasswordHashIterations").value = pbkdf2Iterations;
   }
   else
   {
      document.getElementById("PasswordHashMemoryCost").value = argon2idMemoryCost;
      document.getElementById("PasswordHashIterations").value = argon2idIterations;
   }

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
