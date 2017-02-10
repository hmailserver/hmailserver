<?php
   if (!defined('IN_WEBADMIN'))
      exit();

   $version = explode('.', phpversion());
   if ((int) $version[0] < 5)
   {
      echo "hMailServer WebAdmin requires PHP5 or later.";
      die;   
   }
   

   // Make sure that settings in config.php are specified.
   if ($hmail_config['rooturl'] === "CHANGE-ME")
   {
      echo "Please update config.php to match your system.";
      die;
   }

   if (!isset($hmail_config['rule_editing_level']))
   {
      echo "The config.php file which is in use is not compatible with this version of WebAdmin.<br/> To resolve this, please use the config.php which comes with this version of WebAdmin.";
      die;   
   }
   
?>