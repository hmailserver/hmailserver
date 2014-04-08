<?php

   error_reporting(E_ALL);

   if (!file_exists("config.php"))
   {
   	echo "Please rename config-dist.php to config.php. The file is found in the PHPWebAdmin root folder.";
   	die;
   }

   define('IN_WEBADMIN', true);
   
   require_once("config.php");
   require_once("include/initialization_test.php");
   require_once("initialize.php");
 
   set_exception_handler("ExceptionHandler");
   set_error_handler("ErrorHandler");
   
   $page = hmailGetVar("page");
   
   if ($page == "")
      $page = "frontpage";

   $isbackground = (substr($page, 0,10) == "background");
   
   if ($isbackground)
      $page = "$page.php";
   else
      $page = "hm_$page.php";
      

      
   // Check that the page really exists.
   $page = stripslashes($page);
   $page = basename($page, ".php");
   
   if (!file_exists('./' . $page . '.php'))
      hmailHackingAttemp();

   // If it's a background page, run here.
   if ($isbackground)
   {
      include './' . $page . '.php';

      // Page is run, die now.
      die;
   }

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
	<head>
		<META HTTP-EQUIV="Content-Type" CONTENT="text/html; CHARSET=iso-8859-1">
		<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
		<META HTTP-EQUIV="Expires" CONTENT="0">
		
		<TITLE>WebAdmin</TITLE>

		<link href="style.css" type="text/css" rel="stylesheet">
      <link href="include/tabber/tabber.css" type="text/css" rel="stylesheet">

      <script type="text/javascript">
         document.write('<style type="text/css">.tabber{display:none;}<\/style>');
      </script>      
		
		<script type="text/javascript" src="include/formcheck.js"></script>
      <script type="text/javascript" src="include/dtree.js"></script>
      <script type="text/javascript" src="include/tabber/tabber.js"></script>
      <script type="text/javascript" src="include/ajax.js"></script>

      <script type="text/javascript">
      function ConfirmDelete(name, url)
      {
         confirm_delete = "<?php echo GetConfirmDelete();?>"
         confirm_delete = confirm_delete.replace("%s", name);
         
         if (confirm(confirm_delete))
            document.location = url;
      }
      </script>       
      
</head>
<body>
  <br/>
  
  <table width="90%" border="0" cellpadding="0" cellspacing="0" align="center">
  <tr>
     <td>
         <font face="verdana, arial" size="1">
         WebAdmin
         </font>
     </td>
  </tr>
  </table>
  <br/>
   

   
  <table height="500" width="90%" class="mainborder" id="maintable" cellpadding="8" border="0" align="center">
      <tr> 
         <?php
         
         if (hmail_isloggedin())
         {
         ?>
            <td valign="top" width="240">
            <?php
               include "include_treemenu.php";

               echo "<div class=\"dtree\" id=\"dtree\">
                  <script type=\"text/javascript\">
                  //<!--
                  d = new dTree('d','images/');
                  $dtree
                  //-->
                  </script>
               ";
            ?>
               <div align="right">

               </div>
            </td>
            
            <td valign="top">
               <?php

                  include './' . $page . '.php';
               ?>
               <br/><br/>
            </td>
         <?php
         }
         else
         {
            ?>
            <td valign="top" align="left">
               <?php
               
                  include "hm_login.php";
               ?>
            </td>                     
            <?php
         }
         ?>
      </tr>

</table>

</body>
</html>
