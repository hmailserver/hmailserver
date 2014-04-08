<?php
session_start();
session_destroy();

define('IN_WEBADMIN', true);
require_once("config.php");

?>

<html>
	<head>
		<title></title>
		<meta http-equiv='refresh' content='0;URL=<?php echo $hmail_config['rooturl'] . "index.php"?>'>
	</head>
	<body>
	
	</body>
</html>