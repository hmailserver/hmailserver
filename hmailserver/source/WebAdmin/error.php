   <?php
      if (!isset($errstr) || !isset($errfile))
         die;
         
      if (!defined('IN_WEBADMIN'))
         die;     
   ?>

   <table width="900" border="0" cellspacing="0" cellpadding="0" align="center">
      <tr> 
        <td>
         <font face="verdana" size="2">
         
         <h1>Operation failed</h1>
         The operation failed. Please make sure that you have logged on with the appropriate permissions to perform this task.
         <br/>
         <br/>
         
         The following description exists:<br/><br/>
         
         <?php echo $errstr?>
         </br><br/>
         Error location: <?php echo $errfile?><br/>
         </font>
         <br/><br/>
         <input type="submit" onClick="history.back();" value="Go back">
         <input type="submit" onClick="document.location.href='logout.php';" value="Log out">
         
         
      </tr>
  </table>
