<?php
if (!defined('IN_WEBADMIN'))
   exit();

if (hmailGetAdminLevel() != 2)
	hmailHackingAttemp();

define("STSMTP", 1);
define("STPOP3", 3);
define("STIMAP", 5);
	
$obStatus      = $obBaseApp->Status();
$serverstate	= $obBaseApp->ServerState();
$action  		= hmailGetVar("action","");

$statusstarttime = $obStatus->StartTime();
$statusprocessedmessages = $obStatus->ProcessedMessages();
$statusmessageswithvirus = $obStatus->RemovedViruses();
$statusmessageswithspam = $obStatus->RemovedSpamMessages();

$sessions_smtp = $obStatus->SessionCount(STSMTP);
$sessions_pop3 = $obStatus->SessionCount(STPOP3);
$sessions_imap = $obStatus->SessionCount(STIMAP);

if ($action == "control")
{
   $controlaction = hmailGetVar("controlaction","");
   
   if ($controlaction == "1")
      $obBaseApp->Start();
   else if ($controlaction == "0")
      $obBaseApp->Stop();
   
}

switch($serverstate)
{
	case 1:
		$state = $obLanguage->String("Stopped");
		break;
	case 2:
	   $state = $obLanguage->String("Starting");
		break;
	case 3:
	   $state = $obLanguage->String("Running");
		break;
	case 4:
	   $state = $obLanguage->String("Stopping");
		break;
	default:
	   $state = "Unknown";;
		break;
}

switch($serverstate)
{
	case 1:
	case 4:
		$controlaction = 1;
		$controlbutton = $obLanguage->String("Start");
		break;
	case 2:
	case 3:
	   $controlaction = 0;
	   $controlbutton = $obLanguage->String("Stop");
		break;
	default:
	   $state = "Unknown";
		break;
}
?>

<div class="tabber">
  <div class="tabbertab">
      <h2><?php EchoTranslation("Server")?></h2>
      
      <table border="0" width="100%" cellpadding="5">
         <tr>
            <td width="30%"><?php EchoTranslation("Status")?></td>
            <td><?php echo $state?></td>
         </tr>
         <form action="index.php" method="post" onSubmit="return formCheck(this);">
         
         <?php
		PrintHiddenCsrfToken();
            PrintHidden("page", "status");
            PrintHidden("action", "control");
         ?>
         

         <tr>
            <td><?php EchoTranslation("Action")?></td>
            <td>
               <?php PrintHidden("controlaction", $controlaction);?>
               <input type="submit" value="<?php echo $controlbutton?>">
            </td>
         </tr> 
         </form> 
         <tr>
            <td>
              <i><?php EchoTranslation("Server information")?> </i>
            </td>
            <td>
               <i><?php EchoTranslation("Database information")?> </i>
            </td>
        </tr>
        <tr>
           <td valign="top">
              <?php echo $obBaseApp->Version?> 
           </td>
           <td valign="top">
              <?php 
                $obDatabase = $obBaseApp->Database;
                
                $servername = $obDatabase->ServerName;
                $databasename = $obDatabase->DatabaseName;
                $databaseversion = $obDatabase->CurrentVersion;
                
                if ($servername != "")
                {
                   echo $obLanguage->String("Server address") . ": " . $servername; 
                   echo "<br/>";
                }
                
                if ($databasename != "")
                {
                   echo $obLanguage->String("Database name") . ": " . $databasename;         
                   echo "<br/>";
                }
                   
                if ($databaseversion != "")
                {
                   echo $obLanguage->String("Database version") . ": " . $databaseversion;         
                   echo "<br/>";                   
                }
              ?>
           </td>           
        </tr>
       </table>
   </div>
   <div class="tabbertab">
         <h2><?php EchoTranslation("Status")?></h2>
         
         <table border="0" width="100%" cellpadding="5">
            <tr>
               <th width="30%"></th>
               <th width="70%"></th>
            </tr>   
         <tr>
            <td><?php EchoTranslation("Server up since")?></td>
            <td><?php echo $statusstarttime?></td>
         </tr>   
         <tr>
            <td colspan="2"><i><?php EchoTranslation("Number of")?></i></td>
         </tr>          
         <tr>
            <td><?php EchoTranslation("Processed messages")?></td>
            <td><?php echo $statusprocessedmessages?></td>
         </tr>    
         <tr>
            <td><?php EchoTranslation("Viruses detected")?></td>
            <td><?php echo $statusmessageswithvirus?></td>
         </tr>    
         <tr>
            <td><?php EchoTranslation("Spam messages")?></td>
            <td><?php echo $statusmessageswithspam?></td>
         </tr>   
         <tr>
            <td colspan="2"><i><?php EchoTranslation("Number of open sessions")?></i></td>
         </tr>   

         <?php      
         
            PrintPropertyRow("SMTP", $sessions_smtp);
            PrintPropertyRow("POP3", $sessions_pop3);
            PrintPropertyRow("IMAP", $sessions_imap);
         ?>
      </table>
   </div>
   <div class="tabbertab">

         <h2><?php EchoTranslation("Delivery queue")?></h2>
         

         
         <table border="0" width="100%" cellpadding="5">
               <tr>
                  <td><i><?php EchoTranslation("ID")?></i></td>
                  <td><i><?php EchoTranslation("Created")?></i></td>
                  <td><i><?php EchoTranslation("From")?></i></td>
                  <td><i><?php EchoTranslation("To")?></i></td>
                  <td><i><?php EchoTranslation("Next try")?></i></td>
                  <td><i><?php EchoTranslation("Number of retries")?></i></td>
               </tr>         
         <?php      
            $undeliveredMessages = $obStatus->UndeliveredMessages;
            
            $count = 0;
            if (strlen($undeliveredMessages) > 0)
            {
               $list = explode("\r\n", $undeliveredMessages);
               $count = count($list);
               $as_soon_as_possible = $obLanguage->String("As soon as possible");
            
               foreach ($list as $line)
               {
                  $columns = explode("\t", $line);
                  
                  if (count($columns) > 4)
                  {
                     if ($columns[4] == "1901-01-01 00:00:00")
                        $columns[4] = $as_soon_as_possible;
                  
                     echo "<tr>";
                     echo "<td>" . $columns[0] . "</td>";
                     echo "<td>" . $columns[1] . "</td>";
                     echo "<td>" . PreprocessOutput($columns[2]) . "</td>";
                     echo "<td>" . PreprocessOutput($columns[3]) . "</td>";
                     echo "<td>" . $columns[4] . "</td>";
                     echo "<td>" . $columns[6] . "</td>";
                     echo "</tr>";
                  }
               }
            }
            
         ?>
         </table>
         <br/>
         <?php
            echo $count . "&nbsp;" .  $obLanguage->String("messages in queue");
         ?>         
         
   </div>   
</div>
       