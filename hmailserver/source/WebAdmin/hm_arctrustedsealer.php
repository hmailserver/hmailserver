<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // The user is not server administrator

$id          = hmailGetVar("id",0);
$action      = hmailGetVar("action","");

$Domain = "";
$Description = "";

if ($action == "edit")
{
   $trustedSealer = $obBaseApp->Settings->AntiSpam->ARCTrustedSealers->ItemByDBID($id);

   $Domain      = $trustedSealer->Domain;
   $Description = $trustedSealer->Description;
}
?>

<h1><?php EchoTranslation("ARC trusted sealer")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_arctrustedsealer_save");
      PrintHidden("action", "$action");
      PrintHidden("id", "$id");
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
               PrintPropertyEditRow("Domain", "Domain", $Domain, 40);
               PrintPropertyEditRow("Description", "Description", $Description, 40);
            ?>

         </table>
       </div>
    </div>
   <?php
      PrintSaveButton();
   ?>
</form>
