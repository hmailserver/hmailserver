<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   if (hmailGetAdminLevel() != 2)
   	hmailHackingAttemp(); // Only server admins can change this.

   $iphomeid 	= hmailGetVar("iphomeid",0);
   $iphomeaddress	= hmailGetVar("iphomeaddress",0);
   $action	   = hmailGetVar("action","");
   
   $obSettings	= $obBaseApp->Settings();
   $obIPHomes  = $obSettings->IPHomes;

   if ($action == "edit")
      $obIPHome = $obIPHomes->ItemByDBID($iphomeid);
   elseif ($action == "add")
      $obIPHome = $obIPHomes->Add();
   elseif ($action == "delete")
   {
      $obIPHomes->DeleteByDBID($iphomeid);
      header("Location: index.php?page=multihoming");
      exit();
   }

   $obIPHome->IPAddress = $iphomeaddress;
   $obIPHome->Save();
   
   $iphomeid = $obIPHome->ID;
   
   header("Location: index.php?page=iphome&action=edit&iphomeid=$iphomeid");

?>

