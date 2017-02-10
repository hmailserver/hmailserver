<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();
$obTCPIPPorts  = $obSettings->TCPIPPorts;

$action	   = hmailGetVar("action","");

?>

<h1><?php EchoTranslation("TCP/IP ports")?></h1>

<table border="0" width="100%" cellpadding="5">
	<tr>
		<td width="30%"><?php EchoTranslation("Protocol")?></td>
		<td width="30%"><?php EchoTranslation("TCP/IP port")?></td>
		<td width="20%"></td>
	</tr>
	
<?php
$bgcolor = "#EEEEEE";

$Count = $obTCPIPPorts->Count();


$str_delete = $obLanguage->String("Remove");

for ($i = 0; $i < $Count; $i++)
{
	$obTCPIPPort         = $obTCPIPPorts->Item($i);
	
	$portprotcol             = $obTCPIPPort->Protocol;
	$portid            	     = $obTCPIPPort->ID;
	$portnumber              = $obTCPIPPort->PortNumber;
	
	$protocol_name = "";
	switch ($portprotcol)
	{
		case 1:
			
			$protocol_name = $obLanguage->String("SMTP");
			break;
		case 3:
			$protocol_name = $obLanguage->String("POP3");
			break;
		case 5:
			$protocol_name = $obLanguage->String("IMAP");
			break;
	}
	
	
	echo "<tr bgcolor=\"$bgcolor\">";
	echo "<td width=\"30%\"><a href=\"?page=tcpipport&action=edit&tcpipportid=$portid\">$protocol_name</a></td>";
	echo "<td width=\"30%\"><a href=\"?page=tcpipport&action=edit&tcpipportid=$portid\">$portnumber</a></td>";
	echo "<td width=\"20%\"><a href=\"?page=background_tcpipport_save&csrftoken=$csrftoken&action=delete&tcpipportid=$portid\">$str_delete</a></td>";
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
      <a href="?page=tcpipport&action=add"><i><?php EchoTranslation("Add")?></i></a>
   </td>
</tr>
</table>
