drop table if exists hm_imapfolders;

drop table if exists hm_settings;

drop table if exists hm_accounts;

drop table if exists hm_aliases;

drop table if exists hm_domains;

drop table if exists hm_domain_aliases;

drop table if exists hm_messages;

drop table if exists hm_message_metadata;

drop table if exists hm_dbversion;

drop table if exists hm_distributionlists;

drop table if exists hm_distributionlistsrecipients;

drop table if exists hm_messagerecipients;

drop table if exists hm_routes;

drop table if exists hm_routeaddresses;

drop table if exists hm_securityranges;

drop table if exists hm_dnsbl;

drop table if exists hm_fetchaccounts;

drop table if exists hm_fetchaccounts_uids;

drop table if exists hm_rules;

drop table if exists hm_rule_criterias;

drop table if exists hm_rule_actions;

drop table if exists hm_surblservers;

drop table if exists hm_greylisting_triplets;

drop table if exists hm_blocked_attachments;

drop table if exists hm_servermessages;

drop table if exists hm_greylisting_whiteaddresses;

drop table if exists hm_tcpipports;

drop table if exists hm_whitelist;

drop table if exists hm_sslcertificates;

drop table if exists hm_groups;

drop table if exists hm_group_members;

drop table if exists hm_acl;

drop table if exists hm_incoming_relays;

drop table if exists hm_logon_failures;

create table hm_accounts 
(
	accountid int auto_increment not null, primary key(`accountid`), unique(`accountid`),
	accountdomainid int not null ,
	accountadminlevel tinyint not null ,
	accountaddress varchar (255) not null, unique(`accountaddress`),
	accountpassword varchar (255) not null ,
	accountactive tinyint not null,
	accountisad tinyint not null, 
	accountaddomain varchar (255) not null ,
	accountadusername varchar (255) not null,
	accountmaxsize int not null,
	accountvacationmessageon tinyint not null,
	accountvacationmessage text not null,
	accountvacationsubject varchar (200) not null,
	accountpwencryption tinyint not null,
	accountforwardenabled tinyint not null,
	accountforwardaddress varchar (255) not null,
	accountforwardkeeporiginal tinyint not null,
	accountenablesignature tinyint not null,
	accountsignatureplaintext text not null,
	accountsignaturehtml text not null,
	accountlastlogontime datetime not null,
	accountvacationexpires tinyint unsigned not null,
	accountvacationexpiredate datetime not null,
	accountpersonfirstname varchar(60) not null,
	accountpersonlastname varchar(60) not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_hm_accounts ON hm_accounts (accountaddress);

create table hm_aliases 
(
	aliasid int auto_increment not null, primary key(`aliasid`), unique(`aliasid`),
	aliasdomainid int not null ,
	aliasname varchar (255) not null, unique(`aliasname`),
	aliasvalue varchar (255) not null ,
	aliasactive tinyint not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_hm_aliases ON hm_aliases (aliasdomainid, aliasname);

create table hm_domains 
(
	domainid int auto_increment not null, primary key(`domainid`), unique(`domainid`),
	domainname varchar (80) not null, unique(`domainname`),
	domainactive tinyint not null,
	domainpostmaster varchar (80) not null,
	domainmaxsize integer not null,
	domainaddomain varchar(255) not null,
	domainmaxmessagesize integer not null,
	domainuseplusaddressing  tinyint not null,
	domainplusaddressingchar varchar(1) not null,
	domainantispamoptions integer not null,
	domainenablesignature tinyint not null,
	domainsignaturemethod tinyint not null,
	domainsignatureplaintext text not null,
	domainsignaturehtml text not null,
	domainaddsignaturestoreplies tinyint not null,
	domainaddsignaturestolocalemail tinyint not null,
	domainmaxnoofaccounts int not null,
	domainmaxnoofaliases int not null,
	domainmaxnoofdistributionlists int not null,
	domainlimitationsenabled int not null,
	domainmaxaccountsize int not null,
   domaindkimselector varchar(255) not null,
   domaindkimprivatekeyfile varchar(255) not null
)  DEFAULT CHARSET=utf8;

CREATE INDEX idx_hm_domains ON hm_domains (domainname);

create table hm_domain_aliases
(
	daid int auto_increment not null, primary key(`daid`), unique(`daid`),
	dadomainid int not null ,
	daalias varchar(255) not null
)  DEFAULT CHARSET=utf8;

create table hm_messages 
(
	messageid bigint auto_increment not null, primary key(`messageid`), unique(`messageid`),
	messageaccountid int not null ,
	messagefolderid integer not null DEFAULT 0,
	messagefilename varchar (255) not null ,
	messagetype tinyint not null ,
	messagefrom varchar (255) not null ,
	messagesize bigint not null,
	messagecurnooftries int not null,
	messagenexttrytime datetime not null,
	messageflags tinyint not null,
	messagecreatetime datetime not null,
	messagelocked tinyint not null,
   messageuid bigint not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_hm_messages ON hm_messages (messageaccountid, messagefolderid);

CREATE INDEX idx_hm_messages_type ON hm_messages (messagetype);

create table hm_message_metadata 
(
   metadata_id bigint auto_increment not null, primary key(`metadata_id`),
   metadata_accountid int not null,
   metadata_folderid int not null,
   metadata_messageid bigint  not null,
   metadata_dateutc datetime null,
   metadata_from varchar(255) not null,
   metadata_subject varchar(255) not null,
   metadata_to varchar(255) not null,
   metadata_cc varchar(255) not null
) DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX idx_message_metadata_unique ON hm_message_metadata (`metadata_accountid`,  `metadata_folderid`, `metadata_messageid`);

CREATE INDEX idx_message_metadata_id ON hm_message_metadata (`metadata_messageid`);

create table hm_settings 
(
	settingid int auto_increment not null, primary key(`settingid`), unique(`settingid`),
	settingname varchar (30) not null, unique(`settingname`),
	settingstring varchar (4000) not null ,
	settinginteger int not null
) DEFAULT CHARSET=utf8;

create table hm_dbversion 
(
	value int not null
) DEFAULT CHARSET=utf8;

create table hm_distributionlists 
(
	distributionlistid int auto_increment not null, primary key(distributionlistid), unique(distributionlistid),
	distributionlistdomainid int not null,
	distributionlistaddress varchar(255) not null,unique(`distributionlistaddress`),
	distributionlistenabled tinyint not null,
   distributionlistrequireauth tinyint not null,
	distributionlistrequireaddress varchar(255) not null,
	distributionlistmode tinyint not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_hm_distributionlists ON hm_distributionlists (distributionlistdomainid);

create table hm_distributionlistsrecipients
(
	distributionlistrecipientid int auto_increment not null, primary key(distributionlistrecipientid), unique(distributionlistrecipientid),
	distributionlistrecipientlistid int not null,
	distributionlistrecipientaddress varchar(255)	
);

CREATE INDEX idx_hm_distributionlistsrecipients ON hm_distributionlistsrecipients (distributionlistrecipientlistid);

create table hm_messagerecipients
(
	recipientid bigint auto_increment not null, primary key(recipientid), unique(recipientid),
 	recipientmessageid bigint not null,
	recipientaddress varchar(255) not null,
	recipientlocalaccountid int not null,
	recipientoriginaladdress varchar(255) not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_hm_messagerecipients ON hm_messagerecipients (recipientmessageid);

create table hm_imapfolders 
(
  folderid int NOT NULL auto_increment, primary key(folderid), unique(folderid),
  folderaccountid int unsigned NOT NULL,
  folderparentid int NOT NULL,
  foldername varchar(255) NOT NULL,
  folderissubscribed tinyint unsigned NOT NULL,
  foldercreationtime datetime NOT NULL,
  foldercurrentuid bigint NOT NULL
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_hm_imapfolders ON hm_imapfolders (folderaccountid);

CREATE UNIQUE INDEX idx_hm_imapfolders_unique ON hm_imapfolders (`folderaccountid`, `folderparentid`,  `foldername`);

create table hm_routes
(
  routeid int NOT NULL auto_increment, primary key(routeid), unique(routeid),
  routedomainname varchar(255) NOT NULL,
  routedescription varchar(255) NOT NULL,
  routetargetsmthost varchar(255) NOT NULL,
  routetargetsmtport int NOT NULL,
  routenooftries int NOT NULL,
  routeminutesbetweentry int NOT NULL,
  routealladdresses tinyint unsigned NOT NULL,
  routeuseauthentication tinyint NOT NULL,
  routeauthenticationusername varchar(255) NOT NULL,
  routeauthenticationpassword varchar(255) NOT NULL,
  routetreatsecurityaslocal tinyint NOT NULL,
  routeconnectionsecurity tinyint not null,
  routetreatsenderaslocaldomain tinyint NOT NULL
) DEFAULT CHARSET=utf8;

create table hm_routeaddresses
(
  routeaddressid mediumint(9) NOT NULL auto_increment, primary key(routeaddressid), unique(routeaddressid),
  routeaddressrouteid int NOT NULL,
  routeaddressaddress varchar(255) NOT NULL
) DEFAULT CHARSET=utf8;

create table hm_securityranges
(
	rangeid int auto_increment not null, primary key(rangeid), unique(rangeid),
   rangepriorityid int not null,
	rangelowerip1 bigint not null,
   rangelowerip2 bigint null,
	rangeupperip1 bigint not null,
   rangeupperip2 bigint null,
	rangeoptions int not null,
	rangename varchar (100) not null, unique(`rangename`),
   rangeexpires tinyint not null,
	rangeexpirestime datetime not null
) DEFAULT CHARSET=utf8;

create table hm_dnsbl
(
	sblid int auto_increment not null, primary key(sblid), unique(sblid),
	sblactive tinyint not null,
	sbldnshost varchar(255) not null,
	sblresult varchar(255) not null,
	sblrejectmessage varchar(255) not null,
	sblscore int not null
) DEFAULT CHARSET=utf8;

create table hm_fetchaccounts
(	
	faid int auto_increment not null, primary key(`faid`), unique(`faid`),
	faactive tinyint not null,
	faaccountid int not null,
	faaccountname varchar (255) not null,
	faserveraddress varchar (255) not null,
	faserverport int not null,
	faservertype tinyint not null,
	fausername varchar (255) not null,
	fapassword varchar (255) not null,
	faminutes int not null,
	fanexttry datetime not null,
	fadaystokeep int not null,
	falocked tinyint not null,
	faprocessmimerecipients tinyint not null,
	faprocessmimedate tinyint not null,
	faconnectionsecurity tinyint not null,
	fauseantispam tinyint not null,
	fauseantivirus tinyint not null,
	faenablerouterecipients tinyint not null,
	famimerecipientheaders varchar(255) not null DEFAULT 'To,CC,X-RCPT-TO,X-Envelope-To'
) DEFAULT CHARSET=utf8;

create table hm_fetchaccounts_uids
(
	uidid int auto_increment not null, primary key(`uidid`), unique(`uidid`),
	uidfaid int not null,
	uidvalue varchar(255) not null,
	uidtime datetime not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_hm_fetchaccounts_uids ON hm_fetchaccounts_uids (uidfaid);

create table hm_rules
(
	ruleid int auto_increment not null, primary key(`ruleid`), unique(`ruleid`),
	ruleaccountid int not null,
	rulename varchar(100) not null,
	ruleactive tinyint not null,
	ruleuseand tinyint not null,
	rulesortorder int not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_rules ON hm_rules (ruleaccountid);

create table hm_rule_criterias
(
	criteriaid int auto_increment not null, primary key(`criteriaid`), unique(`criteriaid`),
	criteriaruleid int not null,
	criteriausepredefined tinyint not null,
	criteriapredefinedfield tinyint not null,
	criteriaheadername varchar(255) not null,
	criteriamatchtype tinyint not null,
	criteriamatchvalue varchar(255) not null
	
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_rules_criterias ON hm_rule_criterias (criteriaruleid);

create table hm_rule_actions
(
	actionid int auto_increment not null, primary key(`actionid`), unique(`actionid`),
	actionruleid int not null,
	actiontype tinyint not null,
	actionimapfolder varchar(255) not null,
	actionsubject varchar(255) not null,
	actionfromname varchar(255) not null,
	actionfromaddress varchar(255) not null,
	actionto varchar(255) not null,
	actionbody text not null,
	actionfilename varchar(255) not null,
	actionsortorder int not null,
	actionscriptfunction varchar(255) not null,
	actionheader varchar(80) not null,
	actionvalue varchar(255) not null,
    actionrouteid int not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_rules_actions ON hm_rule_actions (actionruleid);

create table hm_surblservers
(
	surblid int auto_increment not null, primary key(surblid), unique(surblid),
	surblactive tinyint not null,
	surblhost varchar(255) not null,
	surblrejectmessage varchar(255) not null,
	surblscore int not null
) DEFAULT CHARSET=utf8;

create table hm_greylisting_triplets
(
	glid bigint auto_increment not null, primary key(glid), unique(glid),
	glcreatetime datetime not null,
	glblockendtime datetime not null,
	gldeletetime datetime not null,
	glipaddress1 bigint not null,
   glipaddress2 bigint null,
	glsenderaddress varchar(255) not null,
	glrecipientaddress varchar(255) not null,
	glblockedcount int not null,
	glpassedcount int not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_greylisting_triplets ON hm_greylisting_triplets (glipaddress1, glipaddress2, glsenderaddress(40), glrecipientaddress(40));

create table hm_greylisting_whiteaddresses
(
	whiteid bigint auto_increment not null, primary key(whiteid), unique(whiteid),
	whiteipaddress varchar(255) not null,
	whiteipdescription varchar(255) not null
) DEFAULT CHARSET=utf8;

create table hm_blocked_attachments
(
	baid bigint auto_increment not null, primary key(baid), unique(baid),
	bawildcard varchar(255) not null,
	badescription varchar(255) not null
) DEFAULT CHARSET=utf8;

create table hm_servermessages
(
	smid int auto_increment not null, primary key(smid), unique(smid), 
	smname varchar(255) not null,
	smtext text not null
) DEFAULT CHARSET=utf8;

create table hm_tcpipports
(
	portid bigint auto_increment not null, primary key(portid), unique(portid),
	portprotocol tinyint not null,
	portnumber int not null,
	portaddress1 bigint not null,
   portaddress2 bigint null,
	portconnectionsecurity tinyint not null,
	portsslcertificateid bigint not null
) DEFAULT CHARSET=utf8;

create table hm_whitelist
(
	whiteid bigint auto_increment not null, primary key(whiteid), unique(whiteid),
	whiteloweripaddress1 bigint not null,
   whiteloweripaddress2 bigint null,
	whiteupperipaddress1 bigint not null,
   whiteupperipaddress2 bigint null,
	whiteemailaddress varchar(255) not null,
	whitedescription varchar(255) not null
) DEFAULT CHARSET=utf8;

create table hm_sslcertificates
(
	sslcertificateid bigint auto_increment not null, primary key(sslcertificateid), unique(sslcertificateid),
	sslcertificatename varchar(255) not null,
	sslcertificatefile varchar(255) not null,
	sslprivatekeyfile varchar(255) not null
	
) DEFAULT CHARSET=utf8;

create table hm_groups
(
	groupid bigint auto_increment not null, primary key(`groupid`), unique(`groupid`),
    groupname varchar(255)
) DEFAULT CHARSET=utf8;

create table hm_group_members
(
	memberid bigint auto_increment not null, primary key(`memberid`), unique(`memberid`),
	membergroupid bigint not null,
	memberaccountid bigint not null
) DEFAULT CHARSET=utf8;

create table hm_acl
(
	aclid bigint auto_increment not null, primary key(`aclid`), unique(`aclid`),
   aclsharefolderid bigint not null,
	aclpermissiontype tinyint not null,
	aclpermissiongroupid bigint not null,
	aclpermissionaccountid bigint not null,
	aclvalue bigint not null,
	unique(aclsharefolderid, aclpermissiontype, aclpermissiongroupid, aclpermissionaccountid)
) DEFAULT CHARSET=utf8;

create table hm_incoming_relays
(
   relayid int auto_increment not null, primary key(`relayid`), unique(`relayid`),
   relayname varchar(100) not null,
   relaylowerip1 bigint not null,
   relaylowerip2 bigint null,
   relayupperip1 bigint not null,
   relayupperip2 bigint null
) DEFAULT CHARSET=utf8; 

create table hm_logon_failures
(
   ipaddress1 bigint not null,
   ipaddress2 bigint null,
   failuretime datetime not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_hm_logon_failures_ipaddress ON hm_logon_failures (ipaddress1, ipaddress2);

CREATE INDEX idx_hm_logon_failures_failuretime ON hm_logon_failures (failuretime);

insert into hm_securityranges (rangepriorityid, rangelowerip1, rangelowerip2, rangeupperip1, rangeupperip2, rangeoptions, rangename, rangeexpires, rangeexpirestime) values (10, 0, NULL, 4294967295, NULL, 96203, 'Internet', 0, '2001-01-01');

insert into hm_securityranges (rangepriorityid, rangelowerip1, rangelowerip2, rangeupperip1, rangeupperip2, rangeoptions, rangename, rangeexpires, rangeexpirestime) values (30, 2130706433, NULL, 2130706433, NULL, 71627, 'My computer', 0, '2001-01-01');

insert into hm_servermessages (smname, smtext) values ('VIRUS_FOUND', 'Virus found');

insert into hm_servermessages (smname, smtext) values ('VIRUS_ATTACHMENT_REMOVED', 'Virus found:\r\nThe attachment(s) of this message was removed since a virus was detected in at least one of them.\r\n\r\n');

insert into hm_servermessages (smname, smtext) values ('VIRUS_NOTIFICATION', 'The message below contained a virus and did not\r\nreach some or all of the intended recipients.\r\n\r\n   From: %MACRO_FROM%\r\n   To: %MACRO_TO%\r\n   Sent: %MACRO_SENT%\r\n   Subject: %MACRO_SUBJECT%\r\n\r\nhMailServer\r\n');

insert into hm_servermessages (smname, smtext) values ('SEND_FAILED_NOTIFICATION', 'Your message did not reach some or all of the intended recipients.\r\n\r\n   Sent: %MACRO_SENT%\r\n   Subject: %MACRO_SUBJECT%\r\n\r\nThe following recipient(s) could not be reached:\r\n\r\n%MACRO_RECIPIENTS%\r\n\r\nhMailServer\r\n');

insert into hm_servermessages (smname, smtext) values ('MESSAGE_UNDELIVERABLE', 'Message undeliverable');

insert into hm_servermessages (smname, smtext) values ('MESSAGE_FILE_MISSING', 'The mail server could not deliver the message to you since the file %MACRO_FILE% does not exist on the server.\r\n\r\nThe file may have been deleted by anti virus software running on the server.\r\n\r\nhMailServer');

insert into hm_servermessages (smname, smtext) values ('ATTACHMENT_REMOVED', 'The attachment %MACRO_FILE% was blocked for delivery by the e-mail server. Please contact your system administrator if you have any questions regarding this.\r\n\r\nhMailServer\r\n');

insert into hm_dnsbl (sblactive, sbldnshost, sblresult, sblrejectmessage, sblscore) values (0, 'zen.spamhaus.org','127.0.0.2-8|127.0.0.10-11', 'Rejected by Spamhaus.', 3);

insert into hm_dnsbl (sblactive, sbldnshost, sblresult, sblrejectmessage, sblscore) values (0, 'bl.spamcop.net', '127.0.0.2','Rejected by SpamCop.', 3);

insert into hm_surblservers (surblactive, surblhost, surblrejectmessage, surblscore) values (0, 'multi.surbl.org', 'Rejected by SURBL.', 3);

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.bat', 'Batch processing file');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.cmd', 'Command file for Windows NT');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.com', 'Command');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.cpl', 'Windows Control Panel extension');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.csh', 'CSH script');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.exe', 'Executable file');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.inf', 'Setup file');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.lnk', 'Windows link file');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.msi', 'Windows Installer file');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.msp', 'Windows Installer patch');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.pif', 'Program Information file');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.reg', 'Registration key');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.scf', 'Windows Explorer command');

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.scr', 'Windows Screen saver');

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxpop3connections', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxsmtpconnections', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('mirroremailaddress', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('relaymode', '', 2);

insert into hm_settings (settingname, settingstring, settinginteger) values ('authallowplaintext', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('allowmailfromnull', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('logging', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('logdevice', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpnoofretries', '', 4);

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpminutesbetweenretries', '', 60);

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolimap', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolsmtp', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolpop3', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomeimap', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomepop3', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomesmtp', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayer', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxdelivertythreads', '', 10);

insert into hm_settings (settingname, settingstring, settinginteger) values ('logformat', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwinenable', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwinexec', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwindb', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('avnotifysender', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('avnotifyreceiver', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('avaction', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('sendstatistics', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('hostname', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerusername', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerpassword', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('usesmtprelayerauthentication', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerport', '', 25);

insert into hm_settings (settingname, settingstring, settinginteger) values ('usecustomvirusscanner', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('customvirusscannerexecutable', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('customviursscannerreturnvalue', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('usespf', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('usemxchecks', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('usescriptserver', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('scriptlanguage', 'VBScript', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxmessagesize', '', 20480);

insert into hm_settings (settingname, settingstring, settinginteger) values ('usecache', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('domaincachettl', '', 60);

insert into hm_settings (settingname, settingstring, settinginteger) values ('accountcachettl', '', 60);

insert into hm_settings (settingname, settingstring, settinginteger) values ('awstatsenabled', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('rulelooplimit', '', 5);

insert into hm_settings (settingname, settingstring, settinginteger) values ('backupoptions', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('backupdestination', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('defaultdomain', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('avmaxmsgsize', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpdeliverybindtoip', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapquota', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapidle', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapacl', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('maximapconnections', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapsort', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('workerthreadpriority', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ascheckhostinhelo', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('tcpipthreads', '', 15);

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpallowincorrectlineendings', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('usegreylisting', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('greylistinginitialdelay', '', 30);

insert into hm_settings (settingname, settingstring, settinginteger) values ('greylistinginitialdelete', '', 24);

insert into hm_settings (settingname, settingstring, settinginteger) values ('greylistingfinaldelete', '', 864);

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamaddheaderspam', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamaddheaderreason', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamprependsubject', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamprependsubjecttext', '[SPAM]', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableattachmentblocking', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxsmtprecipientsinbatch', '', 100);

insert into hm_settings (settingname, settingstring, settinginteger) values ('disconnectinvalidclients', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('maximumincorrectcommands', '', 100);

insert into hm_settings (settingname, settingstring, settinginteger) values ('aliascachettl', '', 60);

insert into hm_settings (settingname, settingstring, settinginteger) values ('distributionlistcachettl', '', 60);

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerconnectionsecurity', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('adddeliveredtoheader', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('groupcachettl', '', 60);

insert into hm_settings (settingname, settingstring, settinginteger) values ('imappublicfoldername', '#Public', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamenabled', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('usespfscore', '', 3);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ascheckhostinheloscore', '', 2);

insert into hm_settings (settingname, settingstring, settinginteger) values ('usemxchecksscore', '', 2);

insert into hm_settings (settingname, settingstring, settinginteger) values ('spammarkthreshold', '', 5);

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamdeletethreshold', '', 20);

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinenabled', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinscore', '', 5);

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinmergescore', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinhost', '127.0.0.1', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinport', '', 783);

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispammaxsize', '', 1024);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDKIMVerificationEnabled', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDKIMVerificationFailureScore', '', 5);

insert into hm_settings (settingname, settingstring, settinginteger) values ('AutoBanOnLogonFailureEnabled', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('MaxInvalidLogonAttempts', '', 3);

insert into hm_settings (settingname, settingstring, settinginteger) values ('LogonAttemptsWithinMinutes', '', 30);

insert into hm_settings (settingname, settingstring, settinginteger) values ('AutoBanMinutes', '', 60);

insert into hm_settings (settingname, settingstring, settinginteger) values ('IMAPHierarchyDelimiter', '.', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('MaxNumberOfAsynchronousTasks', '', 15);

insert into hm_settings (settingname, settingstring, settinginteger) values ('MessageIndexing', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnSPFSuccess', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnMailFromMX', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('MaxNumberOfMXHosts', '', 15);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ClamAVEnabled', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ClamAVHost', 'localhost', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ClamAVPort', '', 3310);

insert into hm_settings (settingname, settingstring, settinginteger) values ('SmtpDeliveryConnectionSecurity', '', 2);

insert into hm_settings (settingname, settingstring, settinginteger) values ('VerifyRemoteSslCertificate', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('SslCipherList', 'ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:ECDHE-RSA-RC4-SHA:ECDHE-ECDSA-RC4-SHA:AES128:AES256:RC4-SHA:HIGH:!aNULL:!eNULL:!EXPORT:!DES:!3DES:!MD5:!PSK;', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('SslVersions', '', 14);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ImapMasterUser', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ImapAuthAllowPlainText', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('EnableImapSASLPlain', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('EnableImapSASLInitialResponse', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ascheckptr', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ascheckptrscore', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('IPv6Preferred', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('TlsOptions', '', 0);

insert into hm_tcpipports (portprotocol, portnumber, portaddress1, portaddress2, portconnectionsecurity, portsslcertificateid) values (1, 25, 0, NULL, 0, 0);

insert into hm_tcpipports (portprotocol, portnumber, portaddress1, portaddress2, portconnectionsecurity, portsslcertificateid) values (1, 587, 0, NULL, 0, 0);

insert into hm_tcpipports (portprotocol, portnumber, portaddress1, portaddress2, portconnectionsecurity, portsslcertificateid) values (3, 110, 0, NULL, 0, 0);

insert into hm_tcpipports (portprotocol, portnumber, portaddress1, portaddress2, portconnectionsecurity, portsslcertificateid) values (5, 143, 0, NULL, 0, 0);

insert into hm_dbversion values (5705);