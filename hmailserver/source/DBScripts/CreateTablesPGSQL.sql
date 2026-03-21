CREATE or REPLACE function hm_drop_table (tablename varchar) returns void as '
DECLARE
	table_exists int4;
BEGIN
	SELECT into table_exists count(*) from pg_class where relname = tablename::name;
	if table_exists > 0 then
		execute ''DROP TABLE '' || tablename;
	end if;
END;'
language 'plpgsql' ;

select hm_drop_table('hm_imapfolders');

select hm_drop_table('hm_settings');

select hm_drop_table('hm_accounts');

select hm_drop_table('hm_aliases');

select hm_drop_table('hm_domains');

select hm_drop_table('hm_domain_aliases');

select hm_drop_table('hm_messages');

select hm_drop_table('hm_message_metadata');

select hm_drop_table('hm_dbversion');

select hm_drop_table('hm_distributionlists');

select hm_drop_table('hm_distributionlistsrecipients');

select hm_drop_table('hm_messagerecipients');

select hm_drop_table('hm_routes');

select hm_drop_table('hm_routeaddresses');

select hm_drop_table('hm_securityranges');

select hm_drop_table('hm_dnsbl');

select hm_drop_table('hm_fetchaccounts');

select hm_drop_table('hm_fetchaccounts_uids');

select hm_drop_table('hm_rules');

select hm_drop_table('hm_rule_criterias');

select hm_drop_table('hm_rule_actions');

select hm_drop_table('hm_surblservers');

select hm_drop_table('hm_greylisting_triplets');

select hm_drop_table('hm_blocked_attachments');

select hm_drop_table('hm_servermessages');

select hm_drop_table('hm_greylisting_whiteaddresses');

select hm_drop_table('hm_tcpipports');

select hm_drop_table('hm_whitelist');

select hm_drop_table('hm_sslcertificates');

select hm_drop_table('hm_groups');

select hm_drop_table('hm_group_members');

select hm_drop_table('hm_acl');

select hm_drop_table('hm_incoming_relays');

select hm_drop_table('hm_logon_failures');


create table hm_accounts 
(
	accountid bigserial not null primary key,
	accountdomainid int not null ,
	accountadminlevel smallint not null ,
	accountaddress varchar (255) not null unique,
	accountpassword varchar (255) not null ,
	accountactive smallint not null,
	accountisad smallint not null, 
	accountaddomain varchar (255) not null ,
	accountadusername varchar (255) not null,
	accountmaxsize int not null,
	accountvacationmessageon smallint  not null,
	accountvacationmessage text not null,
	accountvacationsubject varchar (200) not null,
	accountpwencryption smallint  not null,
	accountforwardenabled smallint  not null,
	accountforwardaddress varchar (255) not null,
	accountforwardkeeporiginal smallint  not null,
	accountenablesignature smallint  not null,
	accountsignatureplaintext text not null,
	accountsignaturehtml text not null,
	accountlastlogontime timestamp not null,
	accountvacationexpires smallint  not null,
	accountvacationexpiredate timestamp not null,
	accountpersonfirstname varchar(60) not null,
	accountpersonlastname varchar(60) not null	
);



CREATE INDEX idx_hm_accounts ON hm_accounts (accountaddress);

create table hm_aliases 
(
	aliasid bigserial not null primary key,
	aliasdomainid int not null ,
	aliasname varchar (255) not null unique,
	aliasvalue varchar (255) not null ,
	aliasactive smallint not null
);

CREATE INDEX idx_hm_aliases ON hm_aliases (aliasdomainid, aliasname);

create table hm_domains 
(
	domainid bigserial not null primary key,
	domainname varchar (80) not null unique,
	domainactive smallint not null,
	domainpostmaster varchar (80) not null,
	domainmaxsize integer not null,
	domainaddomain varchar(255) not null,
	domainmaxmessagesize integer not null,
	domainuseplusaddressing  smallint not null,
	domainplusaddressingchar varchar(1) not null,
	domainantispamoptions integer not null,
	domainenablesignature smallint not null,
	domainsignaturemethod smallint not null,
	domainsignatureplaintext text not null,
	domainsignaturehtml text not null,
	domainaddsignaturestoreplies smallint not null,
	domainaddsignaturestolocalemail smallint not null,
	domainmaxnoofaccounts int not null,
	domainmaxnoofaliases int not null,
	domainmaxnoofdistributionlists int not null,
	domainlimitationsenabled int not null,
	domainmaxaccountsize int not null,
   domaindkimselector varchar(255) not null,
   domaindkimprivatekeyfile varchar(255) not null   
);

CREATE INDEX idx_hm_domains ON hm_domains (domainname);

create table hm_domain_aliases
(
	daid bigserial not null primary key,
	dadomainid int not null ,
	daalias varchar(255) not null
);

create table hm_messages 
(
	messageid bigserial not null primary key,
	messageaccountid int not null ,
	messagefolderid integer not null DEFAULT 0,
	messagefilename varchar (255) not null ,
	messagetype smallint not null ,
	messagefrom varchar (255) not null ,
	messagesize bigint not null,
	messagecurnooftries int not null,
	messagenexttrytime timestamp not null,
	messageflags smallint not null,
	messagecreatetime timestamp not null,
	messagelocked smallint not null,
   messageuid bigint not null
);

CREATE INDEX idx_hm_messages ON hm_messages (messageaccountid, messagefolderid);

CREATE INDEX idx_hm_messages_type ON hm_messages (messagetype);

create table hm_message_metadata 
(
   metadata_id bigserial not null primary key,
   metadata_accountid int not null,
   metadata_folderid int not null,
   metadata_messageid bigint  not null,
   metadata_dateutc timestamp null,
   metadata_from varchar(255) not null,
   metadata_subject varchar(255) not null,
   metadata_to varchar(255) not null,
   metadata_cc varchar(255) not null
);

CREATE UNIQUE INDEX idx_message_metadata_unique ON hm_message_metadata (metadata_accountid, metadata_folderid, metadata_messageid);

CREATE INDEX idx_message_metadata_id ON hm_message_metadata (metadata_messageid);

create table hm_settings 
(
	settingid bigserial not null primary key,
	settingname varchar (30) not null unique,
	settingstring varchar (4000) not null ,
	settinginteger int not null
);

create table hm_dbversion 
(
	value int not null
);

create table hm_distributionlists 
(
	distributionlistid bigserial not null primary key,
	distributionlistdomainid int not null,
	distributionlistaddress varchar(255) not null unique,
	distributionlistenabled smallint not null,
   distributionlistrequireauth smallint not null,
	distributionlistrequireaddress varchar(255) not null,
	distributionlistmode smallint not null
);

CREATE INDEX idx_hm_distributionlists ON hm_distributionlists (distributionlistdomainid);

create table hm_distributionlistsrecipients
(
	distributionlistrecipientid bigserial not null primary key,
	distributionlistrecipientlistid int not null,
	distributionlistrecipientaddress varchar(255)	
);

CREATE INDEX idx_hm_distributionlistsrecipients ON hm_distributionlistsrecipients (distributionlistrecipientlistid);

create table hm_messagerecipients
(
	recipientid bigserial not null primary key,
 	recipientmessageid bigint not null,
	recipientaddress varchar(255) not null,
	recipientlocalaccountid int not null,
	recipientoriginaladdress varchar(255) not null
);

CREATE INDEX idx_hm_messagerecipients ON hm_messagerecipients (recipientmessageid);

create table hm_imapfolders 
(
  folderid bigserial not null primary key,
  folderaccountid int NOT NULL,
  folderparentid int NOT NULL,
  foldername varchar(255) NOT NULL,
  folderissubscribed smallint NOT NULL,
  foldercreationtime timestamp NOT NULL,
  foldercurrentuid bigint NOT NULL
);

CREATE INDEX idx_hm_imapfolders ON hm_imapfolders (folderaccountid);

CREATE UNIQUE INDEX idx_hm_imapfolders_unique ON hm_imapfolders (folderaccountid, folderparentid, foldername);

create table hm_routes
(
  routeid bigserial not null primary key,
  routedomainname varchar(255) NOT NULL,
  routedescription varchar(255) NOT NULL,
  routetargetsmthost varchar(255) NOT NULL,
  routetargetsmtport int NOT NULL,
  routenooftries int NOT NULL,
  routeminutesbetweentry int NOT NULL,
  routealladdresses smallint NOT NULL,
  routeuseauthentication smallint NOT NULL,
  routeauthenticationusername varchar(255) NOT NULL,
  routeauthenticationpassword varchar(255) NOT NULL,
  routetreatsecurityaslocal smallint NOT NULL,
  routeconnectionsecurity smallint not null,
  routetreatsenderaslocaldomain smallint NOT NULL
);

create table hm_routeaddresses
(
  routeaddressid bigserial not null primary key,
  routeaddressrouteid int NOT NULL,
  routeaddressaddress varchar(255) NOT NULL
);

create table hm_securityranges
(
	rangeid bigserial not null primary key,
   rangepriorityid int not null,
	rangelowerip1 bigint not null,
   rangelowerip2 bigint null,
	rangeupperip1 bigint not null,
   rangeupperip2 bigint null,
	rangeoptions int not null,
	rangename varchar(100) not null unique,
   rangeexpires smallint not null,
	rangeexpirestime timestamp not null
   
);

create table hm_dnsbl
(
	sblid bigserial not null primary key,
	sblactive smallint not null,
	sbldnshost varchar(255) not null,
	sblresult varchar(255) not null,
	sblrejectmessage varchar(255) not null,
	sblscore int not null
);

create table hm_fetchaccounts
(	
	faid bigserial not null primary key,
	faactive smallint not null,
	faaccountid int not null,
	faaccountname varchar (255) not null,
	faserveraddress varchar (255) not null,
	faserverport int not null,
	faservertype smallint not null,
	fausername varchar (255) not null,
	fapassword varchar (255) not null,
	faminutes int not null,
	fanexttry timestamp not null,
	fadaystokeep int not null,
	falocked smallint not null,
	faprocessmimerecipients smallint not null,
	faprocessmimedate smallint not null,
	faconnectionsecurity smallint not null,
    fauseantispam smallint not null,
    fauseantivirus smallint not null,
    faenablerouterecipients smallint not null,
	famimerecipientheaders varchar(255) not null DEFAULT 'To,CC,X-RCPT-TO,X-Envelope-To'
);

create table hm_fetchaccounts_uids
(
	uidid bigserial not null primary key,
	uidfaid int not null,
	uidvalue varchar(255) not null,
	uidtime timestamp not null
);

CREATE INDEX idx_hm_fetchaccounts_uids ON hm_fetchaccounts_uids (uidfaid);

create table hm_rules
(
	ruleid bigserial not null primary key,
	ruleaccountid int not null,
	rulename varchar(100) not null,
	ruleactive smallint not null,
	ruleuseand smallint not null,
	rulesortorder int not null
);

CREATE INDEX idx_rules ON hm_rules (ruleaccountid);

create table hm_rule_criterias
(
	criteriaid bigserial not null primary key,
	criteriaruleid int not null,
	criteriausepredefined smallint not null,
	criteriapredefinedfield smallint not null,
	criteriaheadername varchar(255) not null,
	criteriamatchtype smallint not null,
	criteriamatchvalue varchar(255) not null
	
);

CREATE INDEX idx_rules_criterias ON hm_rule_criterias (criteriaruleid);

create table hm_rule_actions
(
	actionid bigserial not null primary key,
	actionruleid int not null,
	actiontype smallint not null,
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
);

CREATE INDEX idx_rules_actions ON hm_rule_actions (actionruleid);

create table hm_surblservers
(
	surblid bigserial not null primary key,
	surblactive smallint not null,
	surblhost varchar(255) not null,
	surblrejectmessage varchar(255) not null,
	surblscore int not null
);

create table hm_greylisting_triplets
(
	glid bigserial not null primary key,
	glcreatetime timestamp not null,
	glblockendtime timestamp not null,
	gldeletetime timestamp not null,
	glipaddress1 bigint not null,
   glipaddress2 bigint null,
	glsenderaddress varchar(200) not null,
	glrecipientaddress varchar(200) not null,
	glblockedcount int not null,
	glpassedcount int not null
);

CREATE INDEX idx_greylisting_triplets ON hm_greylisting_triplets (glipaddress1, glipaddress2, glsenderaddress, glrecipientaddress);

create table hm_greylisting_whiteaddresses
(
	whiteid bigserial not null primary key,
	whiteipaddress varchar(255) not null,
	whiteipdescription varchar(255) not null
);

create table hm_blocked_attachments
(
	baid bigserial not null primary key,
	bawildcard varchar(255) not null,
	badescription varchar(255) not null
);

create table hm_servermessages
(
	smid bigserial not null primary key,
	smname varchar(255) not null,
	smtext text not null
);

create table hm_tcpipports
(
	portid bigserial not null primary key,
	portprotocol smallint not null,
	portnumber int not null,
	portaddress1 bigint not null,
    portaddress2 bigint null,
	portconnectionsecurity smallint not null,
	portsslcertificateid bigint not null
);

create table hm_whitelist
(
	whiteid bigserial not null primary key,
	whiteloweripaddress1 bigint not null,
   whiteloweripaddress2 bigint null,
	whiteupperipaddress1 bigint not null,
   whiteupperipaddress2 bigint null,
	whiteemailaddress varchar(255) not null,
	whitedescription varchar(255) not null
);

create table hm_sslcertificates
(
	sslcertificateid bigserial not null primary key,
	sslcertificatename varchar(255) not null,
	sslcertificatefile varchar(255) not null,
	sslprivatekeyfile varchar(255) not null
	
);

create table hm_groups
(
	groupid bigserial not null primary key,
    groupname varchar(255)
);

create table hm_group_members
(
	memberid bigserial not null primary key,
	membergroupid bigint not null,
	memberaccountid bigint not null
);

create table hm_acl
(
	aclid bigserial not null primary key,
   aclsharefolderid bigint not null,
	aclpermissiontype smallint not null,
	aclpermissiongroupid bigint not null,
	aclpermissionaccountid bigint not null,
	aclvalue bigint not null,
	unique(aclsharefolderid, aclpermissiontype, aclpermissiongroupid, aclpermissionaccountid)
);

create table hm_incoming_relays
(
   relayid serial not null primary key,
   relayname varchar(100) not null,
   relaylowerip1 bigint not null,
   relaylowerip2 bigint null,
   relayupperip1 bigint not null,
   relayupperip2 bigint null
);

create table hm_logon_failures
(
   ipaddress1 bigint not null,
   ipaddress2 bigint null,
   failuretime timestamp not null
);

CREATE INDEX idx_hm_logon_failures_ipaddress ON hm_logon_failures (ipaddress1, ipaddress2);

CREATE INDEX idx_hm_logon_failures_failuretime ON hm_logon_failures (failuretime);

insert into hm_securityranges (rangepriorityid, rangelowerip1, rangelowerip2, rangeupperip1, rangeupperip2, rangeoptions, rangename, rangeexpires, rangeexpirestime) values (10, 0, NULL, 4294967295, NULL, 96203, 'Internet', 0, '2001-01-01');

insert into hm_securityranges (rangepriorityid, rangelowerip1, rangelowerip2, rangeupperip1, rangeupperip2, rangeoptions, rangename, rangeexpires, rangeexpirestime) values (30, 2130706433, NULL, 2130706433, NULL, 71627, 'My computer', 0, '2001-01-01');

insert into hm_servermessages (smname, smtext) values ('VIRUS_FOUND', 'Virus found');

insert into hm_servermessages (smname, smtext) values ('VIRUS_ATTACHMENT_REMOVED', 'Virus found:\r\nThe attachment(s) of this message was removed since a virus was detected in at least one of them.\r\n\r\n');

insert into hm_servermessages (smname, smtext) values ('VIRUS_NOTIFICATION', E'The message below contained a virus and did not\r\nreach some or all of the intended recipients.\r\n\r\n   From: %MACRO_FROM%\r\n   To: %MACRO_TO%\r\n   Sent: %MACRO_SENT%\r\n   Subject: %MACRO_SUBJECT%\r\n\r\nhMailServer\r\n');

insert into hm_servermessages (smname, smtext) values ('SEND_FAILED_NOTIFICATION', E'Your message did not reach some or all of the intended recipients.\r\n\r\n   Sent: %MACRO_SENT%\r\n   Subject: %MACRO_SUBJECT%\r\n\r\nThe following recipient(s) could not be reached:\r\n\r\n%MACRO_RECIPIENTS%\r\n\r\nhMailServer\r\n');

insert into hm_servermessages (smname, smtext) values ('MESSAGE_UNDELIVERABLE', E'Message undeliverable');

insert into hm_servermessages (smname, smtext) values ('MESSAGE_FILE_MISSING', E'The mail server could not deliver the message to you since the file %MACRO_FILE% does not exist on the server.\r\n\r\nThe file may have been deleted by anti virus software running on the server.\r\n\r\nhMailServer');

insert into hm_servermessages (smname, smtext) values ('ATTACHMENT_REMOVED', E'The attachment %MACRO_FILE% was blocked for delivery by the e-mail server. Please contact your system administrator if you have any questions regarding this.\r\n\r\nhMailServer\r\n');

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