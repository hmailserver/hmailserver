<?php
if (!defined('IN_WEBADMIN'))
   exit();

$domainid  = hmailGetVar("domainid",null, true);

if (hmailGetAdminLevel() == 0)
	hmailHackingAttemp(); // Users are not allowed to show this page.

if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
	hmailHackingAttemp(); // Domain admin but not for this domain.
	
?>

<h1><?php EchoTranslation("Aliases")?></h1>
<table border="0" width="100%" cellpadding="5">
<?php

$bgcolor = "#EEEEEE";

$obDomain			= $obBaseApp->Domains->ItemByDBID($domainid);
$obAliases 			= $obDomain->Aliases();

$Count = $obAliases->Count();

$str_delete = $obLanguage->String("Remove");

$obAliases = $obDomain->Aliases;

for ($i = 0; $i < $Count; $i++)
{
	$obAlias             = $obAliases->Item($i);
	$aliasname           = $obAlias->Name;
	$aliasid             = $obAlias->ID;
	
   $aliasname = PreprocessOutput($aliasname);
   $aliasname_escaped = GetStringForJavaScript($aliasname);
   
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td width=\"80%\"><a href=\"?page=alias&action=edit&domainid=$domainid&aliasid=$aliasid\">$aliasname</a></td>";
   echo "<td><a href=\"javascript:ConfirmDelete('$aliasname_escaped', '?page=background_alias_save&csrftoken=$csrftoken&action=delete&domainid=$domainid&aliasid=$aliasid')\">$str_delete</a></td>";
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
      <a href="?page=alias&action=add&domainid=<?php echo $domainid?>"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>
</table>