<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // Users are not allowed to show this page.

?>
<h1><?php EchoTranslation("SURBL servers")?></h1>
<table border="0" width="100%" cellpadding="5">
<tr>
   <td><i><?php EchoTranslation("Name")?></i></td>
   <td><i><?php EchoTranslation("Enabled")?></i></td>
   <td>&nbsp;</td>
</tr>
<?php

$bgcolor = "#EEEEEE";

$obSettings	   = $obBaseApp->Settings();
$dnsBlacklists = $obSettings->AntiSpam->SURBLServers;

$Count = $dnsBlacklists->Count();

$str_delete = $obLanguage->String("Remove");



for ($i = 0; $i < $Count; $i++)
{
	$dnsBlackList = $dnsBlacklists->Item($i);
    $id          = $dnsBlackList->ID;
	$name        = PreprocessOutput($dnsBlackList->DNSHost);
	$enabled     = $dnsBlackList->Active ? $obLanguage->String("Yes") : $obLanguage->String("No");
	
     
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td width=\"60%\"><a href=\"?page=surblserver&action=edit&id=$id&\">$name</a></td>";
    echo "<td width=\"20%\">$enabled</td>";
	echo "<td width=\"20%\"><a href=\"?page=background_surblserver_save&csrftoken=$csrftoken&action=delete&id=$id\">$str_delete</a></td>";
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
      <a href="?page=surblserver&action=add"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>

</table>