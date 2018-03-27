<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // Users are not allowed to show this page.

?>
<h1><?php EchoTranslation("SSL certificates")?></h1>
<table border="0" width="100%" cellpadding="5">
<tr>
   <td><i><?php EchoTranslation("Name")?></i></td>
   <td>&nbsp;</td>
</tr>
<?php

$bgcolor = "#EEEEEE";

$obSettings	   = $obBaseApp->Settings();
$SSLCertificates = $obSettings->SSLCertificates;

$Count = $SSLCertificates->Count();

$str_delete = $obLanguage->String("Remove");



for ($i = 0; $i < $Count; $i++)
{
	$sslCertificate = $SSLCertificates->Item($i);
   $id          = $sslCertificate->ID;
	$name        = $sslCertificate->Name;
	
   $name = PreprocessOutput($name);
      
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td width=\"60%\"><a href=\"?page=sslcertificate&action=edit&id=$id&\">$name</a></td>";
	echo "<td width=\"20%\"><a href=\"?page=background_sslcertificate_save&csrftoken=$csrftoken&action=delete&id=$id\">$str_delete</a></td>";
	echo "</tr>";
	
	if ($bgcolor == "#EEEEEE")
	   $bgcolor = "#DDDDDD";
	else
	   $bgcolor = "#EEEEEE";
}

?>
<tr>
   <td>
      <br>
      <a href="?page=sslcertificate&action=add"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>

</table>