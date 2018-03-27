<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != ADMIN_SERVER)
	hmailHackingAttemp(); // Users are not allowed to show this page.

?>
<h1><?php EchoTranslation("Rules")?></h1>
<table border="0" width="100%" cellpadding="5">
<tr>
   <td width="50%">
      <i><?php EchoTranslation("Name");?></i>
   </td>
   <td width="25%">
      <i><?php EchoTranslation("Enabled");?></i>
   </td>
   
   <td>
   </td>
</tr>
<?php

$bgcolor = "#EEEEEE";

$rules = $obBaseApp->Rules();
$Count = $rules->Count();

$str_delete = $obLanguage->String("Remove");
$str_yes = $obLanguage->String("Yes");
$str_no = $obLanguage->String("No");

for ($i = 0; $i < $Count; $i++)
{
	$rule = $rules->Item($i);
	$rulename = $rule->Name;
	$ruleid = $rule->ID;
   $enabled = $rule->Active ? $str_yes : $str_no;
	
   $rulename = PreprocessOutput($rulename);
   
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td><a href=\"?page=rule&action=edit&domainid=0&accountid=0&ruleid=$ruleid&\">$rulename</a></td>";
   echo "<td><a href=\"?page=rule&action=edit&domainid=0&accountid=0&ruleid=$ruleid&\">$enabled</a></td>";
	echo "<td><a href=\"?page=background_rule_save&savetype=rule&csrftoken=$csrftoken&action=delete&domainid=0&accountid=0&action=delete&ruleid=$ruleid\">$str_delete</a></td>";
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
      <a href="?page=rule&domainid=0&accountid=0&action=add"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>

</table>