<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

$obSettings	= $obBaseApp->Settings();
$obScripting    = $obSettings->Scripting();

$action	   = hmailGetVar("action","");

if($action == "save")
{
	$obScripting->Enabled = hmailGetVar("scriptingenabled",0);
	
	$langauge = hmailGetVar("scriptinglanguage",0);
	
	if ($langauge != "VBScript" && $langauge != "JScript")
	{
		echo "Unsupported language";
		die;
	}
	
	$obScripting->Language = hmailGetVar("scriptinglanguage",0);

}
elseif ($action == "checksyntax")
{
   $syntax_result = $obScripting->CheckSyntax();
}
elseif ($action == "reloadscripts")
{
   $obScripting->Reload();
}


$scriptingenabled = $obScripting->Enabled;
$scriptinglanguage = $obScripting->Language;

$scriptingenabledchecked = hmailCheckedIf1($scriptingenabled);

?>

<h1><?php EchoTranslation("Scripts")?></h1>

   <div class="tabber">
      <div class="tabbertab">
          <h2><?php EchoTranslation("General")?></h2>            
   
         <form action="index.php" method="post" onSubmit="return formCheck(this);">
         
            <?php
			   PrintHiddenCsrfToken();
               PrintHidden("page", "scripts");
               PrintHidden("action", "save");
            ?>   

         	<table border="0" width="100%" cellpadding="5">
         	<tr>
         		<td width="30%"><?php EchoTranslation("Enabled")?></td>
         		<td width="70%" colspan="2"><input type="checkbox" name="scriptingenabled" value="1" <?php echo $scriptingenabledchecked?>></td>
         	</tr>   
         	<tr>
         		<td></td>
         		<td>
         		   <select name="scriptinglanguage"  style="font-family: Trebuchet MS, Verdana, Arial, Helvetica, sans-serif">
         		      <option value="VBScript" <?php if ($scriptinglanguage == "VBScript") echo "selected"; ?>>VBScript</option>
         		      <option value="JScript" <?php if ($scriptinglanguage == "JScript") echo "selected"; ?>>JScript</option>
         		   </select>
         		</td>
         	</tr>   
         	</table>
         <?php 
            PrintSaveButton();
         ?>            
        </form>
      </div>
      <div class="tabbertab">
          <h2><?php EchoTranslation("Actions")?></h2>            

         <form action="index.php" method="post" onSubmit="return formCheck(this);">
            <?php
			   PrintHiddenCsrfToken();
               PrintHidden("page", "scripts");
               PrintHidden("action", "checksyntax");
            ?>   
            
            
            <table border="0" width="100%" cellpadding="5">
         	<tr>
         		<td colspan="1">  
         		   <?php
         		   if ($action == "checksyntax")
         		      echo $syntax_result;
                     echo "<br/>";
         		   ?>               
         		   <input type="submit" value="<?php EchoTranslation("Check syntax")?>">
         		</td>
         	</tr>		
         	</table>
         </form>

         <form action="index.php" method="post" onSubmit="return formCheck(this);">
         
            <?php
			   PrintHiddenCsrfToken();
               PrintHidden("page", "scripts");
               PrintHidden("action", "reloadscripts");
            ?>            

            <table border="0" width="100%" cellpadding="5">
         	<tr>
         		<td colspan="1">  
         		   <br/>
         		   <input type="submit" value="<?php EchoTranslation("Reload scripts")?>">
         		</td>
         	</tr>		
         	</table>
       </form>   
    </div>
   </div>
   

   
