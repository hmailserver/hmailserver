<?php
if (!defined('IN_WEBADMIN'))
    exit();


// Build tree menu
$dtitem=0;
$dtree = "d.add(" . $dtitem++ .",-1,'" . GetStringForJavaScript("Welcome") . "','index.php','','','','');\r\n";
$username = $_SESSION['session_username'];

if (hmailGetAdminLevel() == 0)
{
    // User
    $domainname = hmailGetUserDomainName($username);
    
    $obDomain = $obBaseApp->Domains->ItemByName($domainname);
    $obAccounts = $obDomain->Accounts;
    
    $obAccount	= $obAccounts->ItemByAddress($username);
    
    $accountaddress = $obAccount->Address;
    $accountaddress = PreprocessOutput($accountaddress);
    $accountaddress = EscapeStringForJs($accountaddress);
    
    $url = htmlentities("index.php?page=account&action=edit&accountid=" . $obAccount->ID . "&domainid=" . $obDomain->ID);
    $di = $dtitem++;
    
    $dtree .= "d.add($di,0,'" . $accountaddress . "','$url','','','" . "images/user.png','" . "images/user.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$di,'" . GetStringForJavaScript("External accounts") . "','index.php?page=account_externalaccounts&accountid=" . $obAccount->ID . "&domainid=" . $obDomain->ID. "');\r\n";
}


if (hmailGetAdminLevel() == 1)
{
    // Domain
    $dtree .= "d.add(" . $dtitem++ .",0,'" . GetStringForJavaScript("Domains") . "','','','','" . "images/server.png','" . "images/server.png');\r\n";
    
    $domainname = hmailGetUserDomainName($username);
    
    $obDomain = $obBaseApp->Domains->ItemByName($domainname);
    
    $domain_root = $dtitem++;
    
    GetStringForDomain($obDomain,1);
}

if (hmailGetAdminLevel() == 2)
{
    $obSettings	= $obBaseApp->Settings();
    
    // Server
    $dtree .= "d.add(" . $dtitem++ . ",0,'" . GetStringForJavaScript("Status") . "','index.php?page=status','','','" . "images/information.png');\r\n";
    
    // List all domains
    $obDomains	= $obBaseApp->Domains();
    $DomainCount = $obDomains->Count();
    
    $dtree .= "d.add(" . $dtitem++ .",0,'" . GetStringForJavaScript("Domains") . " ($DomainCount)','index.php?page=domains','','','" . "images/world.png','" . "images/world.png');\r\n";
    
    for ($i = 1; $i <= $DomainCount; $i++)
    {
        $obDomain = $obBaseApp->Domains[$i-1];
        $domain_root = $dtitem++;
        GetStringForDomain($obDomain,2);
    }
    
    
    $dtree .= "d.add(" . $dtitem++ . ",0,'" . GetStringForJavaScript("Rules") . "','index.php?page=rules','','','" . "images/folder.png');\r\n";
    
    $settings_root = $dtitem++;
    $dtree .= "d.add(" . $settings_root . ",0,'" . GetStringForJavaScript("Settings") . "','','','','" . "images/setting_tools.png','" . "images/setting_tools.png');\r\n";
    
    $settings_protocols_root = $dtitem++;
    
    $dtree .= "d.add(" . $settings_protocols_root . ",$settings_root,'" . GetStringForJavaScript("Protocols") . "','','','','" . "images/server_connect.png','" . "images/server_connect.png');\r\n";
    $settings_smtp_root = $dtitem++;
    $dtree .= "d.add(" . $settings_smtp_root . ",$settings_protocols_root,'" . GetStringForJavaScript("SMTP") . "','index.php?page=smtp','','','" . "images/connect.png','" . "images/connect.png');\r\n";
    
    $settings_spamprotection_root = $dtitem++;
    $dtree .= "d.add(" . $settings_spamprotection_root . ",$settings_root,'" . GetStringForJavaScript("Anti-spam") . "','index.php?page=smtp_antispam','','','" . "images/spam_filter.png','" . "images/spam_filter.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$settings_spamprotection_root,'" . GetStringForJavaScript("DNS blacklists") . "','index.php?page=dnsblacklists','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$settings_spamprotection_root,'" . GetStringForJavaScript("SURBL servers") . "','index.php?page=surblservers','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$settings_spamprotection_root,'" . GetStringForJavaScript("Greylisting") . "','index.php?page=greylisting','','','" . "images/time_delete.png','" . "images/time_delete.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$settings_spamprotection_root,'" . GetStringForJavaScript("White listing") . "','index.php?page=whitelistaddresses','','','" . "images/email_link.png','" . "images/email_link.png');\r\n";
    
    $dtree .= "d.add(" . $dtitem++ . ",$settings_root,'" . GetStringForJavaScript("Anti-virus") . "','index.php?page=smtp_antivirus','','','" . "images/bug_delete.png','" . "images/bug_delete.png');\r\n";
    
    $settings_smtp_route	= $dtitem++;
    
    $obRoutes	= $obSettings->Routes();
    $Count = $obRoutes->Count();
    $dtree .= "d.add(" . $settings_smtp_route . ",$settings_smtp_root,'" . GetStringForJavaScript("Routes") . " (" . $Count . ")','index.php?page=routes','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    
    for ($i = 0; $i < $Count; $i++)
    {
        $obRoute = $obRoutes->Item($i);
        $di = $dtitem++;
        $dsub = $dtitem++;
        
        $routename = PreprocessOutput($obRoute->DomainName);
        $routename = EscapeStringForJs($routename);
        
        $dtree .= "d.add($di,$settings_smtp_route,'" . $routename . "','index.php?page=route&action=edit&routeid=" . $obRoute->ID . "','','','" . "images/routing_intersection_right.png','" . "images/routing_intersection_right.png');\r\n";
        $dtree .= "d.add($dsub,$di,'" . GetStringForJavaScript("Addresses") . "','index.php?page=route_addresses&routeid=" . $obRoute->ID . "');\r\n";
    }
    
    $dtree .= "d.add(" . $dtitem++ . ",$settings_protocols_root,'" . GetStringForJavaScript("POP3") . "','index.php?page=pop3','','','" . "images/connect.png','" . "images/connect.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$settings_protocols_root,'" . GetStringForJavaScript("IMAP") . "','index.php?page=imap','','','" . "images/connect.png','" . "images/connect.png');\r\n";
    
    
    $dtree .= "d.add(" . $dtitem++ . ",$settings_root,'" . GetStringForJavaScript("Logging") . "','index.php?page=logging','','','" . "images/directory_listing.png','" . "images/directory_listing.png');\r\n";
    
    
    $obSecurityRanges	= $obSettings->SecurityRanges();
    $Count = $obSecurityRanges->Count();
    
    $advanced_root = $dtitem++;
    $dtree .= "d.add(" . $advanced_root . ",$settings_root,'" . GetStringForJavaScript("Advanced") . "','','','','" . "images/understanding.png','" . "images/understanding.png');\r\n";
    
    
    $dtree .= "d.add(" . $dtitem++ . ",$advanced_root,'" . GetStringForJavaScript("SSL certificates") . "','index.php?page=sslcertificates','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$advanced_root,'" . GetStringForJavaScript("Auto-ban") . "','index.php?page=autoban','','','" . "images/stop.png','" . "images/stop.png');\r\n";
    
    $settings_ipranges_root = $dtitem++;
    
    $dtree .= "d.add(" . $settings_ipranges_root . ",$advanced_root,'" . GetStringForJavaScript("IP Ranges") . " ($Count)','index.php?page=securityranges','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    
    for ($j = 0; $j < $Count; $j++)
    {
        $obSecurityRange = $obSecurityRanges->Item($j);
        
        $rangename = PreprocessOutput($obSecurityRange->Name);
        $rangename = EscapeStringForJs($rangename);
        
        $di = $dtitem++;
        $dtree .= "d.add($di,$settings_ipranges_root,'" . $rangename . "','index.php?page=securityrange&action=edit&securityrangeid=" . $obSecurityRange->ID . "','','','" . "images/construction.png','" . "images/construction.png');\r\n";
    }
    
    // INCOMING RELAYS BEGIN
    
    
    $obIncomingRelays	= $obSettings->IncomingRelays();
    $Count = $obIncomingRelays->Count();
    
    $settings_incoming_relays_root = $dtitem++;
    $dtree .= "d.add(" . $settings_incoming_relays_root . ",$advanced_root,'" . GetStringForJavaScript("Incoming relays") . " ($Count)','index.php?page=incomingrelays','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    
    for ($j = 0; $j < $Count; $j++)
    {
        $obIncomingRelay = $obIncomingRelays->Item($j);
        $relayname = PreprocessOutput($obIncomingRelay->Name);
        $relayname = EscapeStringForJs($relayname);
        $di = $dtitem++;
        $dtree .= "d.add($di,$settings_incoming_relays_root,'" . $relayname . "','index.php?page=incomingrelay&action=edit&relayid=" . $obIncomingRelay->ID . "','','','" . "images/arrow_in.png','" . "images/arrow_in.png');\r\n";
    }
    
    // INCOMING RELAYS END
    
    
    $dtree .= "d.add(" . $dtitem++ . ",$advanced_root,'" . GetStringForJavaScript("Mirror") . "','index.php?page=mirror','','','" . "images/arrow_branch.png','" . "images/arrow_branch.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$advanced_root,'" . GetStringForJavaScript("Performance") . "','index.php?page=performance','','','" . "images/speedometer.png','" . "speedometer/flash.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$advanced_root,'" . GetStringForJavaScript("Server messages") . "','index.php?page=servermessages','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$advanced_root,'" . GetStringForJavaScript("SSL/TLS") . "','index.php?page=ssltls','','','" . "images/construction.png','" . "images/construction.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$advanced_root,'" . GetStringForJavaScript("Scripts") . "','index.php?page=scripts','','','" . "images/source_code.png','" . "images/source_code.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ",$advanced_root,'" . GetStringForJavaScript("TCP/IP ports") . "','index.php?page=tcpipports','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    
    /*
    Utilities node
    
    
    
    */
    $utilities_root = $dtitem++;
    $dtree .= "d.add(" . $utilities_root . ",0,'" . GetStringForJavaScript("Utilities") . "','','','','" . "images/multitool.png','" . "images/multitool.png');\r\n";
    
    $dtree .= "d.add(" . $dtitem++ . ", $utilities_root,'" . GetStringForJavaScript("Backup") . "','index.php?page=backup','','','" . "images/disk.png','" . "images/disk.png');\r\n";
    $dtree .= "d.add(" . $dtitem++ . ", $utilities_root,'" . GetStringForJavaScript("Diagnostics") . "','index.php?page=diagnostics','','','" . "images/heart.png','" . "images/heart.png');\r\n";
}

$dtree .= "d.add(" . $dtitem++ .",-1,'" . GetStringForJavaScript("Logout") . "','logout.php','" . "" ."');\r\n";

$dtree .= "document.write(d);";

function GetStringForDomain($obDomain, $parentid)
{
    global $dtree, $dtitem, $domain_root;
    
    $current_domainid = hmailGetVar("domainid",0);
    $current_accountid = hmailGetVar("accountid",0);
    
    $domainname = $obDomain->Name;
    $domainname = PreprocessOutput($domainname);
    $domainname = EscapeStringForJs($domainname);
    
    $dtree .= "d.add($domain_root,$parentid,'" . $domainname . "','index.php?page=domain&action=edit&domainid=" . $obDomain->ID . "','','','" . "images/server.png','" . "images/server.png');\r\n";
    
    if ($current_domainid != $obDomain->ID && hmailGetAdminLevel() == ADMIN_SERVER)
    {
        // If the user is logged on as a system administrator, only show accounts
        // for the currently selected domain.
        return;
    }
    
    $obAccounts 	= $obDomain->Accounts();
    $AccountsCount	= $obAccounts->Count();
    $accounts_root = $dtitem++;
    $dtree .= "d.add($accounts_root,$domain_root,'" . GetStringForJavaScript("Accounts") . " ($AccountsCount)','index.php?page=accounts&domainid=" . $obDomain->ID . "','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    for ($j = 0; $j < $AccountsCount; $j++)
    {
        $obAccount	= $obAccounts->Item($j);
        
        $accountaddress = $obAccount->Address;
        $accountaddress = PreprocessOutput($accountaddress);
        $accountaddress = EscapeStringForJs($accountaddress);
        
        $accountid = $obAccount->ID;
        
        $di = $dtitem++;
        $url = htmlentities("index.php?page=account&action=edit&accountid=" . $accountid . "&domainid=" . $obDomain->ID);
        $dtree .= "d.add($di,$accounts_root,'" . $accountaddress . "','$url','','','" . "images/user.png','" . "images/user.png');\r\n";
        
        // Only show sub-nodes for the currently selected account.
        if ($current_accountid == $accountid)
        {
            $dtree .= "d.add(" . $dtitem++ . ",$di,'" . GetStringForJavaScript("External accounts") . "','index.php?page=account_externalaccounts&accountid=" . $accountid . "&domainid=" . $obDomain->ID. "');\r\n";
        }
    }
    
    $obAliases		= $obDomain->Aliases();
    $AliasesCount	= $obAliases->Count();
    $aliases_root	= $dtitem++;
    $dtree .= "d.add($aliases_root,$domain_root,'" . GetStringForJavaScript("Aliases") . " ($AliasesCount)','index.php?page=aliases&domainid=" . $obDomain->ID . "','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    
    for ($j = 0; $j < $AliasesCount; $j++)
    {
        $obAlias	= $obAliases->Item($j);
        
        $aliasname = $obAlias->Name;
        $aliasname = PreprocessOutput($aliasname);
        $aliasname = EscapeStringForJs($aliasname);
        
        $di = $dtitem++;
        $dtree .= "d.add($di,$aliases_root,'" . $aliasname . "','index.php?page=alias&action=edit&aliasid=" . $obAlias->ID . "&domainid=" . $obDomain->ID  . "','','','" . "images/arrow_switch.png','" . "images/arrow_switch.png');\r\n";
    }
    
    $obDistributionLists	= $obDomain->DistributionLists();
    $DListCount				= $obDistributionLists->Count();
    $dlist_root				= $dtitem++;
    $dtree .= "d.add($dlist_root,$domain_root,'" . GetStringForJavaScript("Distribution lists") . " ($DListCount)','index.php?page=distributionlists&domainid=" . $obDomain->ID . "','','','" . "images/folder.png','" . "images/folder.png');\r\n";
    
    for ($j = 0; $j < $DListCount; $j++)
    {
        $obDistributionList	= $obDistributionLists->Item($j);
        $di = $dtitem++;
        
        $address = PreprocessOutput($obDistributionList->Address);
        $address = EscapeStringForJs($address);
        
        $dtree .= "d.add($di,$dlist_root,'" . $address .  "','index.php?page=distributionlist&action=edit&distributionlistid=" . $obDistributionList->ID . "&domainid=" . $obDomain->ID . "','','','" . "images/arrow_out.png','" . "images/arrow_out.png');\r\n";
        $dtree .= "d.add(" . $dtitem++ .",$di,'" . GetStringForJavaScript("Members") . " (" . $obDistributionList->Recipients->Count() . ")','index.php?page=distributionlist_recipients&distributionlistid=" . $obDistributionList->ID . "&domainid=" . $obDomain->ID. "');\r\n";
    }
    
    
}

unset($obDomain);
unset($obAccount);


?>