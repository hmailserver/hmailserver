<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // Users are not allowed to show this page.

?>
<h1><?php EchoTranslation("IP Ranges")?></h1>
<table border="0" width="100%" cellpadding="5">
<?php

$bgcolor = "#EEEEEE";

$obSettings			= $obBaseApp->Settings();
$obSecurityRanges	= $obSettings->SecurityRanges();

$Count = $obSecurityRanges->Count();

$str_delete = $obLanguage->String("Remove");

for ($i = 0; $i < $Count; $i++)
{
	$obSecurityRange     = $obSecurityRanges->Item($i);
	$securityrangename   = $obSecurityRange->Name;
	$securityrangeid     = $obSecurityRange->ID;
	
   $securityrangename = PreprocessOutput($securityrangename);
   
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td width=\"80%\"><a href=\"?page=securityrange&action=edit&securityrangeid=$securityrangeid&\">$securityrangename</a></td>";
	echo "<td width=\"20%\"><a href=\"?page=background_securityrange_save&csrftoken=$csrftoken&action=delete&securityrangeid=$securityrangeid\">$str_delete</a></td>";
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
      <a href="?page=securityrange&action=add"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>

</table>