<?php
if (!defined('IN_WEBADMIN'))
   exit();

$domainid  = hmailGetVar("domainid",null);
$distributionlistid	= hmailGetVar("distributionlistid",0);

if (hmailGetAdminLevel() == 0)
   hmailHackingAttemp();

if (hmailGetAdminLevel() == 1 && $domainid != hmailGetDomainID())
	hmailHackingAttemp(); // Domain admin but not for this domain.

?>
<h1><?php EchoTranslation("Members")?></h1>
<table border="0" width="100%" cellpadding="5">
<?php
$bgcolor = "#EEEEEE";

$obDomain	= $obBaseApp->Domains->ItemByDBID($domainid);
$obList = $obDomain->DistributionLists->ItemByDBID($distributionlistid);
$obRecipients = $obList->Recipients;

$Count = $obRecipients->Count();

$str_delete = $obLanguage->String("Remove");

for ($i = 0; $i < $Count; $i++)
{
	$obRecipient         = $obRecipients->Item($i);
	
	$recipientaddress    = $obRecipient->RecipientAddress;
	$recipientid         = $obRecipient->ID;
	
   $recipientaddress = PreprocessOutput($recipientaddress);
   
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td width=\"80%\"><a href=\"?page=distributionlist_recipient&action=edit&domainid=$domainid&distributionlistid=$distributionlistid&recipientid=$recipientid\">$recipientaddress</a></td>";
	echo "<td width=\"20%\"><a href=\"?page=background_distributionlist_recipient_save&csrftoken=$csrftoken&action=delete&domainid=$domainid&distributionlistid=$distributionlistid&recipientid=$recipientid\">$str_delete</a></td>";
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
      <a href="?page=distributionlist_recipient&action=add&domainid=<?php echo $domainid?>&distributionlistid=<?php echo $distributionlistid?>"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>

</table>

