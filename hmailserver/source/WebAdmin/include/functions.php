<?php
if (!defined('IN_WEBADMIN'))
   exit();

define("ADMIN_USER", 0);
define("ADMIN_DOMAIN", 1);
define("ADMIN_SERVER", 2);
   

define("CONNECTION_SECURITY_NONE", 0);
define("CONNECTION_SECURITY_TLS", 1);
define("CONNECTION_SECURITY_STARTTLSOPTIONAL", 2);
define("CONNECTION_SECURITY_STARTTLSREQUIRED", 3);

define("CSRF_TOKEN_PARAMNAME", "csrftoken");

function hmailGetVar($p_varname, $p_defaultvalue = null, $p_isnumeric = false)
{
	$retval = $p_defaultvalue;
	if(isset($_GET[$p_varname]))
	{
		$retval = $_GET[$p_varname];
	}
	else if (isset($_POST[$p_varname]))
	{
		$retval = $_POST[$p_varname];
	}
	else if (isset($_REQUEST[$p_varname]))
	{
		$retval	= $_REQUEST[$p_varname];
	}
 
	if ($p_isnumeric) 
	{
		$retval = intval($retval);
	}
	
	return $retval;
}


function hmailGetUserDomainName($username)
{
	$atpos = strpos($username, "@");
	$domain = substr($username, $atpos + 1);
	return $domain;
}

function hmailGetAdminLevel()
{
	if (isset($_SESSION["session_adminlevel"]))
		return $_SESSION["session_adminlevel"];
	else
		return -1;
}

function hmailGetDomainID()
{
	if (isset($_SESSION["session_domainid"]))
		return $_SESSION["session_domainid"];
	else
		return -1;
}

function hmailGetAccountID()
{
	if (isset($_SESSION["session_accountid"]))
		return $_SESSION["session_accountid"];
	else
		return -1;
}

function hmail_isloggedin()
{
   if (isset($_SESSION['session_loggedin']) && 
             $_SESSION['session_loggedin'] == "1")
      return true;
   else
      return false;
}

function hmailHackingAttemp()
{
	include "hm_permission_denied.php";
	
	exit();
}

function hmailHasDomainAccess($domainid)
{
	if (hmailGetAdminLevel() == 2)
		return true;

	if (hmailGetAdminLevel() == 1 && hmailGetDomainID() == $domainid)
		return true;
		
	return false;
}

function hmailCheckedIf1($value)
{
   if ($value == "1")
      return "checked";  
   else
      return "";
   
}

function GetStringForJavaScript($StringID)
{
   global $obLanguage;
   
   $value = $obLanguage->String($StringID);
   $value = str_replace('\'', '\\\'', $value);
   
   return $value;
}

function EchoTranslation($string)
{
   global $obLanguage;
   
   echo $obLanguage->String($string);
}

function EscapeStringForJs($string)
{
   return str_replace("\n", '\n', str_replace('"', '\"', addcslashes(str_replace("\r", '', (string)$string), "\0..\37'\\")));
}

function ExceptionHandler($exception)
{
   $errno = $exception->getCode();
   $errfile = basename($exception->getFile());
   $errline = $exception->getLine();
   $errstr = $exception->getMessage();
   
   include "error.php";
   
   die;
}


function ErrorHandler( $errno, $errstr, $errfile )
{
   $errfile = basename($errfile);
   
   include "error.php";
   
   die;
}

function  PrintPropertyRow($caption, $value)
{
   global $obLanguage;
   $caption = $obLanguage->String($caption);
   
   echo 
   "
		<tr>
			<td>$caption</td>
			<td>
   			$value
   	   </td>
		</tr>		
    ";
}

function  PrintPropertyEditRow($name, $caption, $value, $length = 20, $checktype = "")
{
   global $obLanguage;
   $caption = $obLanguage->String($caption);
   
   
   $value = PreprocessOutput($value);
   
   echo 
   "
		<tr>
			<td>$caption</td>
			<td>
            <input type=\"text\" name=\"$name\" id=\"$name\" value=\"$value\" checktype=\"$checktype\" size=\"$length\">
   	   </td>
		</tr>		
    ";
}

function  PrintPropertyAreaRow($name, $caption, $value, $rows = 5, $cols = 20)
{
   global $obLanguage;
   $caption = $obLanguage->String($caption);
   
   
   $value = PreprocessOutput($value);
   
   echo 
   "
		<tr>
			<td>$caption</td>
			<td>
           <textarea name=\"$name\" rows=\"$rows\" cols=\"$cols\">$value</textarea>
   	   </td>
		</tr>		
    ";
}


function  PrintPasswordEntry($name, $caption, $length = 20)
{
   global $obLanguage;
   $caption = $obLanguage->String($caption);
      
   echo 
   "
		<tr>
			<td>$caption</td>
            <td>
				<input type=\"password\" name=\"$name\" size=\"$length\" autocomplete=\"off\">
			</td>
		</tr>		
    ";
}


function  PrintCheckboxRow($name, $caption, $checked)
{
   global $obLanguage;
   $caption = $obLanguage->String($caption);
   
   $checked_text = hmailCheckedIf1($checked);
   
   echo 
   "
   	<tr>
   		<td>$caption</td>
   		<td><input type=\"checkbox\" name=\"$name\" value=\"1\" $checked_text></td>
   	</tr>	
    ";
}

function PrintLargeTableHeader($caption)
{
   global $obLanguage;
   $caption = $obLanguage->String($caption);
   
   echo "
     	<tr>
	      <td colspan=\"2\">
	         <h2>$caption</h3>
	      </td>
   	</tr>
     ";
}

function PrintSaveButton()
{
   global $obLanguage;
   
   $save_label = $obLanguage->String("Save");
   
   echo "
   <div align=\"right\">
      <br/>
      <input type=\"submit\" value=\"$save_label\">
   </div>
   ";
   
}

function PrintHidden($name, $value)
{
   $name = PreprocessOutput($name);
   $value = PreprocessOutput($value);
   
   echo "<input type=\"hidden\" name=\"$name\" value=\"$value\">";
   
}

function PrintHiddenCsrfToken()
{
   PrintHidden("csrftoken", get_csrf_session_token());
   
}

function GetConfirmDelete()
{
   global $obLanguage;
   return GetStringForJavaScript($obLanguage->String("Are you sure you want to delete %s?"));
}

function PreprocessOutput($outputString)
{
   return htmlspecialchars($outputString, ENT_COMPAT, 'utf-8', true);
}

function GetHasRuleAccess($domainid, $accountid)
{

   global $hmail_config;
 
   
   if (hmailGetAdminLevel() == ADMIN_SERVER)
   {
      // server admin always have access.
      return true;
   }
   else if (hmailGetAdminLevel() == ADMIN_DOMAIN)
   {
      // Domain admin has access if domain access is enabled.
      if ($hmail_config['rule_editing_level'] == ADMIN_DOMAIN &&
          hmailGetDomainID() == $domainid &&
          $accountid != 0)
      {
          return true;
      }
      
      // Domain admin has access if user-level is permitted and the account
      // is under the domain admins control.
      if ($hmail_config['rule_editing_level'] == ADMIN_USER &&
          hmailGetDomainID() == $domainid)
      {
         return true;
      }   
   }
   else if (hmailGetAdminLevel() == ADMIN_USER)
   {
      // user has access if enabled and the rule is connected to his account.
      if ($hmail_config['rule_editing_level'] == ADMIN_USER &&
          hmailGetDomainID() == $domainid &&
          hmailGetAccountID() == $accountid)
      {
         return true;
      }         
   }
   
   return false;
   
}

function generate_random_string()
{
	if (function_exists("openssl_random_pseudo_bytes"))	
	{
		$bytes = openssl_random_pseudo_bytes(10);
		return bin2hex($bytes);
	} 
	else 
	{
		$value = mt_rand();
		return sha1(strval($value));
	}
}

function ensure_csrf_session_token_exists()
{
	 if (isset($_SESSION["session_csrf_token"]) && !empty($_SESSION["session_csrf_token"]))
		 return;
	
	$token = generate_random_string();
	
	$_SESSION["session_csrf_token"] = $token;
}

function get_csrf_session_token()
{
	return $_SESSION['session_csrf_token'];
}

function validate_csrf_token_supplied()
{
	if (!defined('CSRF_ENABLED') || CSRF_ENABLED !== true) 
	{
		// CSRF validaton has been disabled.
		return;
	}

	$expected_token = get_csrf_session_token();
	
	$actual_token = hmailGetVar("csrftoken");
	
	if (strcmp($expected_token, $actual_token) !== 0)
	{
		echo "Invalid CSRF token.";
		die;
	}
}

?>
