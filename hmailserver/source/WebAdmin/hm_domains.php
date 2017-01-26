<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();


?>
<h1><?php EchoTranslation("Domains")?></h1>
<table border="0" width="100%" cellpadding="5">

<?php

$bgcolor = "#EEEEEE";

$DomainCount = $obBaseApp->Domains->Count();
$str_delete = $obLanguage->String("Remove");

$str_name      = $obLanguage->String("Domain name");
$str_maxsizemb = $obLanguage->String("Maximum size (MB)");

echo "<tr bgcolor=\"#CCCCCC\">";
echo "<td width=\"60%\">$str_name</td>";
echo "<td width=\"20%\">$str_maxsizemb</td>";
echo "<td width=\"20%\"></td>";
echo "</tr>";

for ($i = 1; $i <= $DomainCount; $i++)
{
	$obDomain            = $obBaseApp->Domains->Item($i-1);
	$domainname          = $obDomain->Name;
	$domainid            = $obDomain->ID;
	$domainmaxsize       = $obDomain->MaxSize;
   
   $domainname = PreprocessOutput($domainname);
   $domainname_escaped = GetStringForJavaScript($domainname);
	
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td width=\"60%\"><a href=\"?page=domain&action=edit&domainid=$domainid\">$domainname</a></td>";
	echo "<td width=\"20%\">$domainmaxsize</td>";
	echo "<td width=\"20%\"><a href=\"javascript:ConfirmDelete('$domainname_escaped', '?page=background_domain_save&csrftoken=$csrftoken&action=delete&domainid=$domainid')\">$str_delete</a></td>";
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
      <a href="?page=domain&action=add"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>
</table>