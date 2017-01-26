<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // The user is not server administrator

$id	            = hmailGetVar("id",0);
$action	         	= hmailGetVar("action","");
	
$Name = "";
$CertificateFile = "";
$PrivateKeyFile = "";

if ($action == "edit")
{
   $sslCertificate = $obBaseApp->Settings->SSLCertificates->ItemByDBID($id);

   $Name		= $sslCertificate->Name;
   $CertificateFile    = $sslCertificate->CertificateFile;
   $PrivateKeyFile    = $sslCertificate->PrivateKeyFile;
}
?>

<h1><?php EchoTranslation("SSL certificate")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_sslcertificate_save");
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
               PrintPropertyEditRow("Name", "Name", $Name);
               PrintPropertyEditRow("CertificateFile", "Certificate file", $CertificateFile, 50);
               PrintPropertyEditRow("PrivateKeyFile", "Private key file", $PrivateKeyFile, 50);
            ?>

         </table>
       </div>
    </div>  
   <?php 
      PrintSaveButton();
   ?>
</form>
