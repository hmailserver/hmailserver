<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();

$action	   = hmailGetVar("action","");

if($action == "save")
{
	$obSettings->SslCipherList = hmailGetVar("SslCipherList", "");
}

$SslCipherList 				= $obSettings->SslCipherList;
?>

<h1><?php EchoTranslation("Security")?></h1>

<form action="index.php" method="post" onSubmit="return formCheck(this);">
   <?php
      PrintHidden("page", "security");
      PrintHidden("action", "save");
   ?>   
   
   <div class="tabber">
      <div class="tabbertab">
         <h2><?php EchoTranslation("General")?></h2>            
   
      	<table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>            

		 <tr>
      		<td><?php EchoTranslation("SSL/TLS ciphers ")?></td>
      		<td>
				<textarea rows="6" cols="40" name="SslCipherList"><?php echo $SslCipherList?></textarea>
			</td>
      	</tr>		
      	</table>
      </div>
   </div>   
   <?php
      PrintSaveButton();
   ?>
     
</form>