<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();

$action	   = hmailGetVar("action","");

if($action == "save")
{
	$obSettings->VerifyRemoteSslCertificate= hmailGetVar("VerifyRemoteSslCertificate",0);
	$obSettings->SslCipherList = hmailGetVar("SslCipherList", "");
	$obSettings->SslVersion30Enabled = hmailGetVar("SslVersion30Enabled", 0);
	$obSettings->TlsVersion10Enabled = hmailGetVar("TlsVersion10Enabled", 0);
	$obSettings->TlsVersion11Enabled = hmailGetVar("TlsVersion11Enabled", 0);
	$obSettings->TlsVersion12Enabled = hmailGetVar("TlsVersion12Enabled", 0);
	
}

$VerifyRemoteSslCertificate = $obSettings->VerifyRemoteSslCertificate;      
$SslCipherList 				= $obSettings->SslCipherList;
$SslVersion30Enabled 		= $obSettings->SslVersion30Enabled;
$TlsVersion10Enabled 		= $obSettings->TlsVersion10Enabled;
$TlsVersion11Enabled 		= $obSettings->TlsVersion11Enabled;
$TlsVersion12Enabled 		= $obSettings->TlsVersion12Enabled;
?>

<h1><?php EchoTranslation("Security")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">
   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "ssltls");
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
				PrintPropertyAreaRow("SslCipherList", "SSL/TLS ciphers", $SslCipherList, 12, 80);
				PrintCheckboxRow("VerifyRemoteSslCertificate", "Verify remote server SSL/TLS certificates", $VerifyRemoteSslCertificate);
				PrintCheckboxRow("SslVersion30Enabled", "SSL v3.0", $SslVersion30Enabled);
				PrintCheckboxRow("TlsVersion10Enabled", "TLS v1.0", $TlsVersion10Enabled);
				PrintCheckboxRow("TlsVersion11Enabled", "TLS v1.1", $TlsVersion11Enabled);
				PrintCheckboxRow("TlsVersion12Enabled", "TLS v1.2", $TlsVersion12Enabled);
			?>
      		
      	</table>
      </div>
   </div>   
   <?php
      PrintSaveButton();
   ?>
     
</form>