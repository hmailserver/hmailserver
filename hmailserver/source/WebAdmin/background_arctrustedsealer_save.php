<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   if (hmailGetAdminLevel() != ADMIN_SERVER)
   	hmailHackingAttemp(); // The user is not server administrator.

   $action      = hmailGetVar("action","");
   $id          = hmailGetVar("id",0);
   $Domain      = hmailGetVar("Domain","");
   $Description = hmailGetVar("Description","");

   $trustedSealers = $obBaseApp->Settings->AntiSpam->ARCTrustedSealers;

   if ($action == "edit")
      $trustedSealer = $trustedSealers->ItemByDBID($id);
   elseif ($action == "add")
      $trustedSealer = $trustedSealers->Add();
   elseif ($action == "delete")
   {
      $trustedSealers->DeleteByDBID($id);
      header("Location: index.php?page=arctrustedsealers");
      exit();
   }
   else
      exit();

   $trustedSealer->Domain = trim($Domain);
   $trustedSealer->Description = $Description;

   $trustedSealer->Save();

   header("Location: index.php?page=arctrustedsealers");
?>
