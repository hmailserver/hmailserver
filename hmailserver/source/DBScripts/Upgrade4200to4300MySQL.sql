ALTER TABLE hm_accounts ADD COLUMN accountforwardenabled TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_accounts ADD COLUMN accountforwardaddress varchar (255) NOT NULL DEFAULT ''

ALTER TABLE hm_accounts ADD COLUMN accountforwardkeeporiginal TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD COLUMN domainuseplusaddressing TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD COLUMN domainplusaddressingchar varchar(1) NOT NULL DEFAULT '+'

ALTER TABLE hm_domains ADD COLUMN domainantispamoptions INTEGER NOT NULL DEFAULT 2047

ALTER TABLE hm_distributionlists ADD COLUMN distributionlistmode tinyint NOT NULL DEFAULT 0

UPDATE hm_distributionlists SET distributionlistmode = 0 WHERE distributionlistrequireaddress = ''

UPDATE hm_distributionlists SET distributionlistmode = 2 WHERE distributionlistrequireaddress <> ''

ALTER TABLE hm_fetchaccounts ADD COLUMN faprocessmimerecipients tinyint NOT NULL DEFAULT 0

ALTER TABLE hm_rule_actions ADD COLUMN actionscriptfunction varchar(255) NOT NULL DEFAULT 0

create table hm_surblservers
(
	surblid int auto_increment not null, primary key(surblid), unique(surblid),
	surblactive tinyint not null,
	surblhost varchar(255) not null,
	surblrejectmessage varchar(255) not null
)

insert into hm_surblservers (surblactive, surblhost, surblrejectmessage) values (0, 'multi.surbl.org', 'Rejected by SURBL')

create table hm_greylisting_triplets
(
	glid bigint auto_increment not null, primary key(glid), unique(glid),
	glcreatetime datetime not null,
	glblockendtime datetime not null,
	gldeletetime datetime not null,
	glipaddress bigint not null,
	glsenderaddress varchar(255) not null,
	glrecipientaddress varchar(255) not null,
	glblockedcount int not null,
	glpassedcount int not null
)

CREATE INDEX idx_greylisting_triplets ON hm_greylisting_triplets (glipaddress, glsenderaddress(40), glrecipientaddress(40))

create table hm_blocked_attachments
(
	baid bigint auto_increment not null, primary key(baid), unique(baid),
	bawildcard varchar(255) not null,
	badescription varchar(255) not null
)

create table hm_servermessages
(
	smid int auto_increment not null, primary key(smid), unique(smid), 
	smname varchar(255) not null,
	smtext text not null
)

insert into hm_servermessages (smname, smtext) values ('VIRUS_FOUND', 'Virus found')

insert into hm_servermessages (smname, smtext) values ('VIRUS_NOTIFICATION', 'The message below contained a virus and did not\r\nreach some or all of the intended recipients.\r\n\r\n   From: %MACRO_FROM%\r\n   To: %MACRO_TO%\r\n   Sent: %MACRO_SENT%\r\n   Subject: %MACRO_SUBJECT%\r\n\r\nhMailServer\r\n')

insert into hm_servermessages (smname, smtext) values ('SEND_FAILED_NOTIFICATION', 'Your message did not reach some or all of the intended recipients.\r\n\r\n   Sent: %MACRO_SENT%\r\n   Subject: %MACRO_SUBJECT%\r\n\r\nThe following recipient(s) could not be reached:\r\n\r\n%MACRO_RECIPIENTS%\r\n\r\nhMailServer\r\n')

insert into hm_servermessages (smname, smtext) values ('MESSAGE_UNDELIVERABLE', 'Message undeliverable')

insert into hm_servermessages (smname, smtext) values ('MESSAGE_FILE_MISSING', 'The mail server could not deliver the message to you since the file %MACRO_FILE% does not exist on the server.\r\n\r\nThe file may have been deleted by anti virus software running on the server.\r\n\r\nhMailServer')

insert into hm_servermessages (smname, smtext) values ('ATTACHMENT_REMOVED', 'The attachment %MACRO_FILE% was blocked for delivery by the e-mail server. Please contact your system administrator if you have any questions regarding this.\r\n\r\nhMailServer\r\n')

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

