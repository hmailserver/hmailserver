<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // Users are not allowed to show this page.

?>
<h1><?php EchoTranslation("Incoming relays")?></h1>
<table border="0" width="100%" cellpadding="5">
<?php

$bgcolor = "#EEEEEE";

$obSettings			= $obBaseApp->Settings();
$obIncomingRelays	= $obSettings->IncomingRelays();

$Count = $obIncomingRelays->Count();

$str_delete = $obLanguage->String("Remove");

for ($i = 0; $i < $Count; $i++)
{
	$obIncomingRelay     = $obIncomingRelays->Item($i);
	$relayname   = $obIncomingRelay->Name;
	$relayid     = $obIncomingRelay->ID;
	
   $relayname = PreprocessOutput($relayname);
   
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td width=\"80%\"><a href=\"?page=incomingrelay&action=edit&relayid=$relayid&\">$relayname</a></td>";
	echo "<td width=\"20%\"><a href=\"?page=background_incomingrelay_save&csrftoken=$csrftoken&action=delete&relayid=$relayid\">$str_delete</a></td>";
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
      <a href="?page=incomingrelay&action=add"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>

</table>