<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp(); 
	
$routeid 	= hmailGetVar("routeid",0);
$routeaddressid	= hmailGetVar("routeaddressid",0);
$action	   = hmailGetVar("action","");

$routeaddress = "";

if ($action == "edit")
{
   $obRoute = $obSettings->Routes->ItemByDBID($routeid);
   $obRouteAddresses = $obRoute->Addresses;
   $obRouteAddress = $obRouteAddresses->ItemByDBID($routeaddressid);
   $routeaddress   = $obRouteAddress->Address;
}

?>

<h1><?php EchoTranslation("Address")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">

   <?php
      PrintHiddenCsrfToken();
      PrintHidden("page", "background_route_address_save");
      PrintHidden("action", $action);
      PrintHidden("routeid", $routeid);
      PrintHidden("routeaddressid", $routeaddressid);
   ?>      
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>          
   
      	<table border="0" width="100%" cellpadding="5">
      		<tr>
      			<td width="30%"><?php EchoTranslation("Address")?></td>
      			<td width="70%">
         			<input type="text" name="routeaddress" value="<?php echo PreprocessOutput($routeaddress)?>" size="35" checkallownull="false" checktype="email" checkmessage="<?php EchoTranslation("Address")?>">
               </td>			
      		</tr>
      	</table>
      </div>
   </div>
   <?php 
      PrintSaveButton();
   ?>   
</form>
