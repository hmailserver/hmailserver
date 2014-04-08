<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // Users are not allowed to show this page.

?>
<h1><?php EchoTranslation("Server messages")?></h1>
<table border="0" width="100%" cellpadding="5">
<?php

$bgcolor = "#EEEEEE";

$obSettings			= $obBaseApp->Settings();
$obServerMessages	= $obSettings->ServerMessages();

$Count = $obServerMessages->Count();

for ($i = 0; $i < $Count; $i++)
{
   $obServerMessage = $obServerMessages->Item($i);
   
	$messagename   = $obServerMessage->Name;
	$messageid     = $obServerMessage->ID;
	
   $messagename = PreprocessOutput($messagename);
   
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td width=\"80%\"><a href=\"?page=servermessage&messageid=$messageid&\">$messagename</a></td>";
	
	echo "</tr>";
	
	if ($bgcolor == "#EEEEEE")
	   $bgcolor = "#DDDDDD";
	else
	   $bgcolor = "#EEEEEE";
}
?>

</table>