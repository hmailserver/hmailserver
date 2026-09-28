<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // Users are not allowed to show this page.

?>
<h1><?php EchoTranslation("ARC trusted sealers")?></h1>
<table border="0" width="100%" cellpadding="5">
<tr>
   <td><i><?php EchoTranslation("Domain")?></i></td>
   <td><i><?php EchoTranslation("Description")?></i></td>
   <td>&nbsp;</td>
</tr>
<?php

$bgcolor = "#EEEEEE";

$trustedSealers = $obBaseApp->Settings->AntiSpam->ARCTrustedSealers;

$Count = $trustedSealers->Count();

$str_delete = $obLanguage->String("Remove");

for ($i = 0; $i < $Count; $i++)
{
   $trustedSealer = $trustedSealers->Item($i);
   $id            = $trustedSealer->ID;
   $domain        = PreprocessOutput($trustedSealer->Domain);
   $description   = PreprocessOutput($trustedSealer->Description);

   echo "<tr bgcolor=\"$bgcolor\">";
   echo "<td width=\"40%\"><a href=\"?page=arctrustedsealer&action=edit&id=$id&\">$domain</a></td>";
   echo "<td width=\"40%\">$description</td>";
   echo "<td width=\"20%\"><a href=\"?page=background_arctrustedsealer_save&csrftoken=$csrftoken&action=delete&id=$id\">$str_delete</a></td>";
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
      <a href="?page=arctrustedsealer&action=add"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>

</table>
