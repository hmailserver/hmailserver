ALTER TABLE hm_accounts ADD accountforwardenabled TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_accounts ADD accountforwardaddress varchar (255) NOT NULL DEFAULT ''

ALTER TABLE hm_accounts ADD accountforwardkeeporiginal TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD domainuseplusaddressing TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD domainplusaddressingchar varchar(1) NOT NULL DEFAULT '+'

ALTER TABLE hm_domains ADD domainantispamoptions INTEGER NOT NULL DEFAULT 2047

ALTER TABLE hm_distributionlists ADD distributionlistmode tinyint NOT NULL DEFAULT 0

UPDATE hm_distributionlists SET distributionlistmode = 0 WHERE distributionlistrequireaddress = ''

UPDATE hm_distributionlists SET distributionlistmode = 2 WHERE distributionlistrequireaddress <> ''

ALTER TABLE hm_fetchaccounts ADD faprocessmimerecipients tinyint NOT NULL DEFAULT 0

ALTER TABLE hm_rule_actions ADD actionscriptfunction varchar(255) NOT NULL DEFAULT 0

create table hm_surblservers
(
	surblid int identity(1,1) not null CONSTRAINT hm_surblid_pk PRIMARY KEY NONCLUSTERED,
	surblactive tinyint not null,
	surblhost varchar(255) not null,
	surblrejectmessage varchar(255) not null
)

insert into hm_surblservers (surblactive, surblhost, surblrejectmessage) values (0, 'multi.surbl.org', 'Rejected by SURBL')


create table hm_greylisting_triplets
(
	glid bigint identity(1,1) not null CONSTRAINT hm_glid_pk PRIMARY KEY NONCLUSTERED,
	glcreatetime datetime not null,
	glblockendtime datetime not null,
	gldeletetime datetime not null,
	glipaddress bigint not null,
	glsenderaddress varchar(255) not null,
	glrecipientaddress varchar(255) not null,
	glblockedcount int not null,
	glpassedcount int not null
	CONSTRAINT u_gltriplet UNIQUE NONCLUSTERED (glipaddress, glsenderaddress, glrecipientaddress)
)

create table hm_blocked_attachments
(
	baid bigint identity(1,1) not null CONSTRAINT hm_baid_pk PRIMARY KEY NONCLUSTERED,
	bawildcard varchar(255) not null,
	badescription varchar(255) not null
)

create table hm_servermessages
(
	smid int identity(1,1) not null CONSTRAINT hm_smid_pk PRIMARY KEY NONCLUSTERED, 
	smname varchar(255) not null,
	smtext text not null
)

insert into hm_servermessages (smname, smtext) values ('VIRUS_FOUND', 'Virus found') --- 

insert into hm_servermessages (smname, smtext) values ('VIRUS_NOTIFICATION', 'The message below contained a virus and did not' + CHAR(13) + CHAR(10) + 'reach some or all of the intended recipients.' + CHAR(13) + CHAR(10) + '' + CHAR(13) + CHAR(10) + '   From: %MACRO_FROM%' + CHAR(13) + CHAR(10) + '   To: %MACRO_TO%' + CHAR(13) + CHAR(10) + '   Sent: %MACRO_SENT%' + CHAR(13) + CHAR(10) + '   Subject: %MACRO_SUBJECT%' + CHAR(13) + CHAR(10) + '' + CHAR(13) + CHAR(10) + 'hMailServer' + CHAR(13) + CHAR(10) + '') --- 

insert into hm_servermessages (smname, smtext) values ('SEND_FAILED_NOTIFICATION', 'Your message did not reach some or all of the intended recipients.' + CHAR(13) + CHAR(10) + '' + CHAR(13) + CHAR(10) + '   Sent: %MACRO_SENT%' + CHAR(13) + CHAR(10) + '   Subject: %MACRO_SUBJECT%' + CHAR(13) + CHAR(10) + '' + CHAR(13) + CHAR(10) + 'The following recipient(s) could not be reached:' + CHAR(13) + CHAR(10) + '' + CHAR(13) + CHAR(10) + '%MACRO_RECIPIENTS%' + CHAR(13) + CHAR(10) + '' + CHAR(13) + CHAR(10) + 'hMailServer' + CHAR(13) + CHAR(10) + '') --- 

insert into hm_servermessages (smname, smtext) values ('MESSAGE_UNDELIVERABLE', 'Message undeliverable') --- 

insert into hm_servermessages (smname, smtext) values ('MESSAGE_FILE_MISSING', 'The mail server could not deliver the message to you since the file %MACRO_FILE% does not exist on the server.' + CHAR(13) + CHAR(10) + '' + CHAR(13) + CHAR(10) + 'The file may have been deleted by anti virus software running on the server.' + CHAR(13) + CHAR(10) + '' + CHAR(13) + CHAR(10) + 'hMailServer') --- 

insert into hm_servermessages (smname, smtext) values ('ATTACHMENT_REMOVED', 'The attachment %MACRO_FILE% was blocked for delivery by the e-mail server. Please contact your system administrator if you have any questions regarding this.' + CHAR(13) + CHAR(10) + '' + CHAR(13) + CHAR(10) + 'hMailServer' + CHAR(13) + CHAR(10) + '') --- 

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.bat', 'Batch processing file')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.cmd', 'Command file for Windows NT')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.com', 'Command')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.cpl', 'Windows Control Panel extension')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.csh', 'CSH script')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.exe', 'Executable file')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.inf', 'Setup file')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.lnk', 'Windows link file')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.msi', 'Windows Installer file')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.msp', 'Windows Installer patch')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.reg', 'Registration key')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.scf', 'Windows Explorer command')

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.scr', 'Windows Screen saver')

insert into hm_settings (settingname, settingstring, settinginteger) values ('defaultdomain', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avmaxmsgsize', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpdeliverybindtoip', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamaction', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapquota', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapidle', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('maximapconnections', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapsort', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('maskpasswordsinlog', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('workerthreadpriority', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ascheckhostinhelo', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('tcpipthreads', '', 15)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpallowincorrectlineendings', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('usegreylisting', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('greylistinginitialdelay', '', 30)

insert into hm_settings (settingname, settingstring, settinginteger) values ('greylistinginitialdelete', '', 24)

insert into hm_settings (settingname, settingstring, settinginteger) values ('greylistingfinaldelete', '', 864)

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamaddheaderspam', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamaddheaderreason', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamprependsubject', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamprependsubjecttext', '[SPAM]', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableattachmentblocking', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxsmtprecipientsinbatch', '', 100)

insert into hm_settings (settingname, settingstring, settinginteger) values ('disconnectinvalidclients', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('maximumincorrectcommands', '', 100)

update hm_dbversion set value = 4300



