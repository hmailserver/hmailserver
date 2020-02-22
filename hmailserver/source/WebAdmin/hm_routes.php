<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp(); // Users are not allowed to show this page.

?>
<h1><?php EchoTranslation("Routes")?></h1>
<table border="0" width="100%" cellpadding="5">
<?php
$bgcolor = "#EEEEEE";

$obRoutes	= $obSettings->Routes();

$Count = $obRoutes->Count();

$str_delete = $obLanguage->String("Remove");

for ($i = 0; $i < $Count; $i++)
{
	$obRoute             = $obRoutes->Item($i);
	$routename           = $obRoute->DomainName;
	$routeid             = $obRoute->ID;
	
   $routename = PreprocessOutput($routename);
   
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td width=80><a href=\"?page=route&action=edit&routeid=$routeid&\">$routename</a></td>";
	echo "<td width=20><a href=\"?page=background_route_save&action=delete&csrftoken=$csrftoken&routeid=$routeid&\">$str_delete</a></td>";
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
      <a href="?page=route&action=add"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>

</table>

