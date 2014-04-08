<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   if (hmailGetAdminLevel() != ADMIN_SERVER)
   	hmailHackingAttemp(); // The user is not server administrator.
   
   $action	   = hmailGetVar("action","");
   $id	      = hmailGetVar("id",0);
   
   $Name	      = hmailGetVar("Name",0);
   $CertificateFile	      = hmailGetVar("CertificateFile","");
   $PrivateKeyFile= hmailGetVar("PrivateKeyFile","");
   
   $sslCertificates = $obBaseApp->Settings->SSLCertificates;
   
   if ($action == "edit")
      $sslCertificate     = $sslCertificates->ItemByDBID($id);
   elseif ($action == "add")
      $sslCertificate     = $sslCertificates->Add();
   elseif ($action == "delete")
   {
      $sslCertificates->DeleteByDBID($id);
      header("Location: index.php?page=sslcertificates");
   }

   // Save the changes
   $sslCertificate->Name = $Name;
   $sslCertificate->CertificateFile = $CertificateFile;
   $sslCertificate->PrivateKeyFile = $PrivateKeyFile;
   $sslCertificate->Save();
   
   header("Location: index.php?page=sslcertificates");
?>

