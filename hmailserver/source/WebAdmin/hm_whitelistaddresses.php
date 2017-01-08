<?php
   if (!defined('IN_WEBADMIN'))
      exit();
   
   if (hmailGetAdminLevel() != 2)
   	hmailHackingAttemp(); // Only server admins can change this.
   
   $obWhiteListAddresses	= $obBaseApp->Settings()->AntiSpam()->WhiteListAddresses;

?>

<h1><?php EchoTranslation("White listing")?></h1>

<table border="0" width="100%" cellpadding="5">
	<tr>
		<td width="30%"><?php EchoTranslation("Description")?></td>
		<td width="20%"><?php EchoTranslation("Lower IP")?></td>
		<td width="20%"><?php EchoTranslation("Upper IP")?></td>
		<td width="20%"><?php EchoTranslation("E-mail address")?></td>
	</tr>
	
<?php
$bgcolor = "#EEEEEE";

$Count = $obWhiteListAddresses->Count();



$str_delete     = $obLanguage->String("Remove");

for ($i = 0; $i < $Count; $i++)
{
	$obAddress   = $obWhiteListAddresses->Item($i);
	
	$ID   	               = $obAddress->ID;
	$LowerIPAddress         = $obAddress->LowerIPAddress;
	$UpperIPAddress         = $obAddress->UpperIPAddress;
	$EmailAddress		      = $obAddress->EmailAddress;
	$Description	      	= $obAddress->Description;
	
   $EmailAddress = PreprocessOutput($EmailAddress);
   $Description = PreprocessOutput($Description);
   
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td><a href=\"?page=whitelistaddress&action=edit&ID=$ID\">$Description</a></td>";
	echo "<td><a href=\"?page=whitelistaddress&action=edit&ID=$ID\">$LowerIPAddress</a></td>";
	echo "<td><a href=\"?page=whitelistaddress&action=edit&ID=$ID\">$UpperIPAddress</a></td>";
	echo "<td><a href=\"?page=whitelistaddress&action=edit&ID=$ID\">$EmailAddress</a></td>";
	echo "<td><a href=\"?page=background_whitelistaddress_save&csrftoken=$csrftoken&action=delete&ID=$ID\">$str_delete</a></td>";
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
      <a href="?page=whitelistaddress&action=add"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>
</table>


