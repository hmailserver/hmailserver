if exists (select * from sysobjects where id = object_id('hm_accounts') and objectproperty(id, 'isusertable') = 1) drop table hm_accounts 

if exists (select * from sysobjects where id = object_id('hm_imapfolders') and objectproperty(id, 'isusertable') = 1) drop table hm_imapfolders 

if exists (select * from sysobjects where id = object_id('hm_securityranges') and objectproperty(id, 'isusertable') = 1) drop table hm_securityranges 

if exists (select * from sysobjects where id = object_id('hm_aliases') and objectproperty(id, 'isusertable') = 1) drop table hm_aliases 

if exists (select * from sysobjects where id = object_id('hm_domains') and objectproperty(id, 'isusertable') = 1) drop table hm_domains 

if exists (select * from sysobjects where id = object_id('hm_domain_aliases') and objectproperty(id, 'isusertable') = 1) drop table hm_domain_aliases 

if exists (select * from sysobjects where id = object_id('hm_messages') and objectproperty(id, 'isusertable') = 1) drop table hm_messages 

if exists (select * from sysobjects where id = object_id('hm_message_metadata') and objectproperty(id, 'isusertable') = 1) drop table hm_message_metadata 

if exists (select * from sysobjects where id = object_id('hm_settings') and objectproperty(id, 'isusertable') = 1) drop table hm_settings 

if exists (select * from sysobjects where id = object_id('hm_dbversion') and objectproperty(id, 'isusertable') = 1) drop table hm_dbversion 

if exists (select * from sysobjects where id = object_id('hm_routes') and objectproperty(id, 'isusertable') = 1) drop table hm_routes 

if exists (select * from sysobjects where id = object_id('hm_routeaddresses') and objectproperty(id, 'isusertable') = 1) drop table hm_routeaddresses 

if exists (select * from sysobjects where id = object_id('hm_serverlog') and objectproperty(id, 'isusertable') = 1) drop table hm_serverlog 

if exists (select * from sysobjects where id = object_id('hm_distributionlists') and objectproperty(id, 'isusertable') = 1) drop table hm_distributionlists 

if exists (select * from sysobjects where id = object_id('hm_distributionlistsrecipients') and objectproperty(id, 'isusertable') = 1) drop table hm_distributionlistsrecipients 

if exists (select * from sysobjects where id = object_id('hm_messagerecipients') and objectproperty(id, 'isusertable') = 1) drop table hm_messagerecipients 

if exists (select * from sysobjects where id = object_id('hm_dnsbl') and objectproperty(id, 'isusertable') = 1) drop table hm_dnsbl 

if exists (select * from sysobjects where id = object_id('hm_fetchaccounts') and objectproperty(id, 'isusertable') = 1) drop table hm_fetchaccounts 

if exists (select * from sysobjects where id = object_id('hm_fetchaccounts_uids') and objectproperty(id, 'isusertable') = 1) drop table hm_fetchaccounts_uids 

if exists (select * from sysobjects where id = object_id('hm_rules') and objectproperty(id, 'isusertable') = 1) drop table hm_rules 

if exists (select * from sysobjects where id = object_id('hm_rule_criterias') and objectproperty(id, 'isusertable') = 1) drop table hm_rule_criterias 

if exists (select * from sysobjects where id = object_id('hm_rule_actions') and objectproperty(id, 'isusertable') = 1) drop table hm_rule_actions 

if exists (select * from sysobjects where id = object_id('hm_surblservers') and objectproperty(id, 'isusertable') = 1) drop table hm_surblservers 

if exists (select * from sysobjects where id = object_id('hm_greylisting_triplets') and objectproperty(id, 'isusertable') = 1) drop table hm_greylisting_triplets 

if exists (select * from sysobjects where id = object_id('hm_blocked_attachments') and objectproperty(id, 'isusertable') = 1) drop table hm_blocked_attachments 

if exists (select * from sysobjects where id = object_id('hm_servermessages') and objectproperty(id, 'isusertable') = 1) drop table hm_servermessages 

if exists (select * from sysobjects where id = object_id('hm_greylisting_whiteaddresses') and objectproperty(id, 'isusertable') = 1) drop table hm_greylisting_whiteaddresses 

if exists (select * from sysobjects where id = object_id('hm_tcpipports') and objectproperty(id, 'isusertable') = 1) drop table hm_tcpipports 

if exists (select * from sysobjects where id = object_id('hm_whitelist') and objectproperty(id, 'isusertable') = 1) drop table hm_whitelist 

if exists (select * from sysobjects where id = object_id('hm_sslcertificates') and objectproperty(id, 'isusertable') = 1) drop table hm_sslcertificates 

if exists (select * from sysobjects where id = object_id('hm_groups') and objectproperty(id, 'isusertable') = 1) drop table hm_groups 

if exists (select * from sysobjects where id = object_id('hm_group_members') and objectproperty(id, 'isusertable') = 1) drop table hm_group_members 

if exists (select * from sysobjects where id = object_id('hm_acl') and objectproperty(id, 'isusertable') = 1) drop table hm_acl 

if exists (select * from sysobjects where id = object_id('hm_incoming_relays') and objectproperty(id, 'isusertable') = 1) drop table hm_incoming_relays

if exists (select * from sysobjects where id = object_id('hm_logon_failures') and objectproperty(id, 'isusertable') = 1) drop table hm_logon_failures

create proc hm_drop_table_objects
	@tablename	sysname
as
set nocount on
declare @constname	sysname,
		@cmd		varchar(1024)
declare curs_constraints cursor for
	select 	name
	from 	sysobjects 
	where 	xtype in ('C', 'F', 'PK', 'UQ', 'D')
	and	(status & 64) = 0
	and     parent_obj = object_id(@tablename)
open curs_constraints
fetch next from curs_constraints into @constname
while (@@fetch_status = 0)
begin
	select @cmd = 'ALTER TABLE ' + @tablename + ' DROP CONSTRAINT ' + @constname
	exec(@cmd)
	fetch next from curs_constraints into @constname
end
close curs_constraints
deallocate curs_constraints
DECLARE @indexName NVARCHAR(128)
DECLARE @dropIndexSql NVARCHAR(4000)
DECLARE tableIndexes CURSOR FOR
SELECT name FROM sysindexes
WHERE id = OBJECT_ID(@tablename) AND
  indid > 0 AND indid < 255 AND
  INDEXPROPERTY(id, name, 'IsStatistics') = 0
ORDER BY indid DESC
OPEN tableIndexes
FETCH NEXT FROM tableIndexes INTO @indexName
WHILE @@fetch_status = 0
BEGIN
  SET @dropIndexSql = N'DROP INDEX ' + @tablename + '.' + @indexName
  EXEC sp_executesql @dropIndexSql
  FETCH NEXT FROM tableIndexes INTO @indexName
END
CLOSE tableIndexes
DEALLOCATE tableIndexes
return 0 

CREATE proc [dbo].[hm_drop_table_column]
	@tablename sysname,
	@columnname sysname
as
	set nocount on
	declare @constname sysname,
	@cmd varchar(1024)
	declare curs_constraints cursor for
	-- delete constraints on column level
	SELECT CONSTRAINT_NAME FROM INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE
		WHERE TABLE_NAME = @tablename AND COLUMN_NAME = @columnname
	open curs_constraints
	fetch next from curs_constraints into @constname
	while (@@fetch_status = 0)
	begin
		select @cmd = 'ALTER TABLE ' + @tablename + ' DROP CONSTRAINT ' + @constname
		exec(@cmd)
		fetch next from curs_constraints into @constname
	end
	close curs_constraints
	deallocate curs_constraints
	-- delete constraints on table level
	declare curs_constraints cursor for
	select name from sysobjects where id in 
	(select constid from sysconstraints where id = OBJECT_ID(@tablename) and colid in
	(select colid from syscolumns where id = OBJECT_ID(@tablename) and name = @columnname))
	open curs_constraints
	fetch next from curs_constraints into @constname
	while (@@fetch_status = 0)
	begin
		select @cmd = 'ALTER TABLE ' + @tablename + ' DROP CONSTRAINT ' + @constname
		exec(@cmd)
		fetch next from curs_constraints into @constname
	end
	close curs_constraints
	deallocate curs_constraints
	declare curs_constraints cursor for
	-- delete indexes for column
	select name from sysindexes where id = OBJECT_ID(@tablename) and INDEXPROPERTY(id, name, 'IsStatistics') = 0 and
	indid in (select indid from sysindexkeys where id = OBJECT_ID(@tablename)
	and colid in (select colid from syscolumns where id = OBJECT_ID(@tablename) and name = @columnname))
	open curs_constraints
	fetch next from curs_constraints into @constname
	while (@@fetch_status = 0)
	begin
		select @cmd = 'DROP INDEX ' + @tablename + '.' + @constname
		exec(@cmd)
		fetch next from curs_constraints into @constname
	end
	close curs_constraints
	deallocate curs_constraints
	select @cmd = 'ALTER TABLE ' + @tablename + ' DROP COLUMN ' + @columnname
	exec(@cmd)
return 0

create table hm_accounts (
	accountid int identity (1, 1) not null,
	accountdomainid int not null,
	accountadminlevel tinyint not null ,
	accountaddress nvarchar(255) not null,
	accountpassword nvarchar(255) not null ,
	accountactive int not null, 
	accountisad int not null, 
	accountaddomain nvarchar(255) not null ,
	accountadusername nvarchar(255) not null,
	accountmaxsize int not null,
	accountvacationmessageon tinyint not null,
	accountvacationmessage nvarchar(1000) not null,
	accountvacationsubject nvarchar(200) not null,
	accountpwencryption tinyint not null,
	accountforwardenabled tinyint not null,
	accountforwardaddress nvarchar(255) not null,
	accountforwardkeeporiginal tinyint not null,
	accountenablesignature tinyint not null,
	accountsignatureplaintext ntext not null,
	accountsignaturehtml ntext not null,
	accountlastlogontime datetime not null,
	accountvacationexpires tinyint not null,
	accountvacationexpiredate datetime not null,
	accountpersonfirstname nvarchar(60) not null,
	accountpersonlastname nvarchar(60) not null
) 

ALTER TABLE hm_accounts ADD CONSTRAINT hm_accounts_pk PRIMARY KEY NONCLUSTERED (accountid) 

ALTER TABLE hm_accounts ADD CONSTRAINT u_accountaddress UNIQUE NONCLUSTERED (accountaddress) 

CREATE CLUSTERED INDEX idx_hm_accounts ON hm_accounts (accountaddress) 

create table hm_aliases (
	aliasid int identity (1, 1) not null,
	aliasdomainid int not null ,
	aliasname nvarchar(255) not null,
	aliasvalue nvarchar(255) not null ,	
	aliasactive tinyint not null
) 

ALTER TABLE hm_aliases ADD CONSTRAINT hm_aliases_pk PRIMARY KEY NONCLUSTERED (aliasid) 

ALTER TABLE hm_aliases ADD CONSTRAINT u_aliasname UNIQUE NONCLUSTERED (aliasname) 

CREATE CLUSTERED INDEX idx_hm_aliases ON hm_aliases (aliasdomainid, aliasname) 

create table hm_domains (
	domainid int identity (1, 1) not null,
	domainname nvarchar(80) not null,
	domainactive tinyint not null,
	domainpostmaster nvarchar(80) not null,
	domainmaxsize int not null,
	domainaddomain nvarchar(255) not null,
	domainmaxmessagesize int not null,
	domainuseplusaddressing  tinyint not null,
	domainplusaddressingchar nvarchar(1) not null,
	domainantispamoptions integer not null,
	domainenablesignature tinyint not null,
	domainsignaturemethod tinyint not null,
	domainsignatureplaintext ntext not null,
	domainsignaturehtml ntext not null,
	domainaddsignaturestoreplies tinyint not null,
	domainaddsignaturestolocalemail tinyint not null,
	domainmaxnoofaccounts int not null,
	domainmaxnoofaliases int not null,
	domainmaxnoofdistributionlists int not null,
	domainlimitationsenabled int not null,
	domainmaxaccountsize int not null,
   domaindkimselector nvarchar(255) not null,
   domaindkimprivatekeyfile nvarchar(255) not null
) 

ALTER TABLE hm_domains ADD CONSTRAINT hm_domains_pk PRIMARY KEY NONCLUSTERED (domainid) 

ALTER TABLE hm_domains ADD CONSTRAINT u_domainname UNIQUE NONCLUSTERED (domainname) 

CREATE CLUSTERED INDEX idx_hm_domains ON hm_domains (domainname)  

create table hm_domain_aliases (
	daid int identity (1, 1) not null,
	dadomainid integer not null ,
	daalias nvarchar(255) not null
) 

ALTER TABLE hm_domain_aliases ADD CONSTRAINT hm_domain_aliases_pk PRIMARY KEY NONCLUSTERED (daid) 

create table hm_messages (
	messageid bigint identity (1, 1) not null,
	messageaccountid int not null ,
	messagefolderid int not null ,
	messagefilename nvarchar(255) not null ,
	messagetype tinyint not null ,
	messagefrom nvarchar(255) not null ,
	messagesize bigint not null,
	messagecurnooftries int not null,
	messagenexttrytime datetime not null,
	messageflags tinyint not null,
	messagecreatetime datetime not null,
	messagelocked tinyint not null,
   messageuid bigint not null
) 

ALTER TABLE hm_messages ADD CONSTRAINT hm_messages_pk PRIMARY KEY NONCLUSTERED (messageid) 

CREATE CLUSTERED INDEX idx_hm_messages ON hm_messages (messageaccountid, messagefolderid)  

CREATE INDEX idx_hm_messages_type ON hm_messages (messagetype) 

create table hm_message_metadata 
(
   metadata_id bigint identity (1, 1) not null,
   metadata_accountid int not null,
   metadata_folderid int not null,
   metadata_messageid bigint  not null,
   metadata_dateutc datetime null,
   metadata_from nvarchar(255) not null,
   metadata_subject nvarchar(255) not null,
   metadata_to nvarchar(255) not null,
   metadata_cc nvarchar(255) not null
)

ALTER TABLE hm_message_metadata ADD CONSTRAINT hm_message_metadata_pk PRIMARY KEY CLUSTERED (metadata_id) 

ALTER TABLE hm_message_metadata ADD CONSTRAINT hm_message_metadata_unique UNIQUE NONCLUSTERED (metadata_accountid, metadata_folderid, metadata_messageid)

CREATE NONCLUSTERED INDEX idx_message_metadata_id ON hm_message_metadata (metadata_messageid)

create table hm_settings (
	settingid int identity (1, 1) not null,
	settingname nvarchar(30) not null,
	settingstring nvarchar(4000) not null ,
	settinginteger int not null
) 

ALTER TABLE hm_settings ADD CONSTRAINT hm_settings_pk PRIMARY KEY NONCLUSTERED (settingid) 

ALTER TABLE hm_settings ADD CONSTRAINT u_settingname UNIQUE NONCLUSTERED (settingname) 

create table hm_dbversion (
	value int not null
) 

create table hm_distributionlists 
(
	distributionlistid int identity (1, 1) not null,
	distributionlistdomainid int not null,
	distributionlistaddress nvarchar(255) not null,
	distributionlistenabled tinyint not null,	
   distributionlistrequireauth tinyint not null,
	distributionlistrequireaddress nvarchar(255) not null,
	distributionlistmode tinyint not null
) 

ALTER TABLE hm_distributionlists ADD CONSTRAINT hm_distributionlists_pk PRIMARY KEY NONCLUSTERED (distributionlistid) 

ALTER TABLE hm_distributionlists ADD CONSTRAINT u_distributionlistaddress UNIQUE NONCLUSTERED (distributionlistaddress) 

CREATE CLUSTERED INDEX idx_hm_distributionlists_distributionlistdomainid ON hm_distributionlists (distributionlistdomainid) 

create table hm_distributionlistsrecipients
(
	distributionlistrecipientid int identity (1, 1) not null,
	distributionlistrecipientlistid int not null,
	distributionlistrecipientaddress nvarchar(255) not null
) 

ALTER TABLE hm_distributionlistsrecipients ADD CONSTRAINT hm_distributionlistsrecipients_pk PRIMARY KEY NONCLUSTERED (distributionlistrecipientid) 

CREATE CLUSTERED INDEX idx_hm_distributionlistsrecipients_distributionlistrecipientlistid ON hm_distributionlistsrecipients (distributionlistrecipientlistid)  

create table hm_messagerecipients
(
	recipientid bigint identity (1, 1) not null,
   recipientmessageid bigint not null,
	recipientaddress nvarchar(255) not null,
	recipientlocalaccountid int not null,
	recipientoriginaladdress nvarchar(255) not null
) 

ALTER TABLE hm_messagerecipients ADD CONSTRAINT hm_messagerecipients_pk PRIMARY KEY NONCLUSTERED (recipientid) 

CREATE CLUSTERED INDEX idx_hm_messagerecipients_recipientmessageid ON hm_messagerecipients (recipientmessageid) 

create table hm_imapfolders 
(
	folderid int identity (1, 1) not null,
	folderaccountid int NOT NULL,
	folderparentid int NOT NULL,
	foldername nvarchar(255) NOT NULL,
	folderissubscribed tinyint NOT NULL,
   foldercreationtime datetime NOT NULL,
   foldercurrentuid bigint NOT NULL
) 

ALTER TABLE hm_imapfolders ADD CONSTRAINT hm_imapfolders_pk PRIMARY KEY NONCLUSTERED (folderid) 

CREATE CLUSTERED INDEX idx_hm_imapfolders_folderaccountid ON hm_imapfolders (folderaccountid)  

ALTER TABLE hm_imapfolders ADD CONSTRAINT idx_hm_imapfolders_unique UNIQUE NONCLUSTERED (folderaccountid, folderparentid, foldername)

create table hm_securityranges
(
	rangeid int identity (1, 1) not null,
  	rangepriorityid int not null,
	rangelowerip1 bigint not null,
   rangelowerip2 bigint null,
	rangeupperip1 bigint not null,
   rangeupperip2 bigint null,
	rangeoptions int not null,
	rangename nvarchar(100) not null,
   rangeexpires tinyint not null,
	rangeexpirestime datetime not null
) 

ALTER TABLE hm_securityranges ADD CONSTRAINT hm_securityranges_pk PRIMARY KEY NONCLUSTERED (rangeid) 

ALTER TABLE hm_securityranges ADD CONSTRAINT u_rangename UNIQUE NONCLUSTERED (rangename) 

create table hm_routes 
(
  routeid int identity (1, 1) not null,
  routedomainname nvarchar(255) NOT NULL,
  routedescription nvarchar(255) NOT NULL,
  routetargetsmthost nvarchar(255) NOT NULL,
  routetargetsmtport int NOT NULL,
  routenooftries int NOT NULL,
  routeminutesbetweentry int NOT NULL,
  routealladdresses tinyint NOT NULL,
  routeuseauthentication tinyint NOT NULL,
  routeauthenticationusername nvarchar(255) NOT NULL,
  routeauthenticationpassword nvarchar(255) NOT NULL,
  routetreatsecurityaslocal tinyint NOT NULL,
  routeconnectionsecurity tinyint not null,
  routetreatsenderaslocaldomain tinyint NOT NULL
) 

ALTER TABLE hm_routes ADD CONSTRAINT hm_routes_pk PRIMARY KEY NONCLUSTERED (routeid) 

ALTER TABLE hm_routes ADD CONSTRAINT u_routedomainname UNIQUE NONCLUSTERED (routedomainname) 

create table hm_routeaddresses
(
  routeaddressid int identity (1, 1) not null ,
  routeaddressrouteid int NOT NULL,
  routeaddressaddress nvarchar(255) NOT NULL
) 

ALTER TABLE hm_routeaddresses ADD CONSTRAINT hm_routeaddresses_pk PRIMARY KEY NONCLUSTERED (routeaddressid) 

create table hm_dnsbl
(
	sblid int identity (1, 1) not null,
   sblactive int not null,
   sbldnshost nvarchar(255) not null,
   sblresult nvarchar(255) not null,
   sblrejectmessage nvarchar(255) not null,
	sblscore int not null
) 

ALTER TABLE hm_dnsbl ADD CONSTRAINT hm_dnsbl_pk PRIMARY KEY NONCLUSTERED (sblid) 

create table hm_fetchaccounts
(	
	faid int identity(1,1) not null,
	faactive tinyint not null,
	faaccountid int not null,
	faaccountname nvarchar(255) not null,
	faserveraddress nvarchar(255) not null,
	faserverport int not null,
	faservertype tinyint not null,
	fausername nvarchar(255) not null,
	fapassword nvarchar(255) not null,
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
	famimerecipientheaders nvarchar(255) not null DEFAULT 'To,CC,X-RCPT-TO,X-Envelope-To'
) 

ALTER TABLE hm_fetchaccounts ADD CONSTRAINT hm_fetchaccounts_pk PRIMARY KEY NONCLUSTERED (faid) 

create table hm_fetchaccounts_uids
(
	uidid int identity(1,1) not null,
	uidfaid int not null,
	uidvalue nvarchar(255) not null,
	uidtime datetime not null
) 

ALTER TABLE hm_fetchaccounts_uids ADD CONSTRAINT hm_fetchaccounts_uids_pk PRIMARY KEY NONCLUSTERED (uidid) 

CREATE CLUSTERED INDEX idx_hm_fetchaccounts_uids ON hm_fetchaccounts_uids (uidfaid)  

create table hm_rules
(
	ruleid int identity(1,1) not null,
	ruleaccountid int not null,
	rulename nvarchar(100) not null,
	ruleactive tinyint not null,
	ruleuseand tinyint not null,
	rulesortorder int not null
) 

ALTER TABLE hm_rules ADD CONSTRAINT hm_rules_pk PRIMARY KEY NONCLUSTERED (ruleid) 

CREATE CLUSTERED INDEX idx_hm_rules ON hm_rules (ruleaccountid)  

create table hm_rule_criterias
(
	criteriaid int identity(1,1) not null,
	criteriaruleid int not null,
	criteriausepredefined tinyint not null,
	criteriapredefinedfield tinyint not null,
	criteriaheadername nvarchar(255) not null,
	criteriamatchtype tinyint not null,
	criteriamatchvalue nvarchar(255) not null
	
) 

ALTER TABLE hm_rule_criterias ADD CONSTRAINT hm_rule_criterias_pk PRIMARY KEY NONCLUSTERED (criteriaid) 

CREATE CLUSTERED INDEX idx_hm_rule_criterias ON hm_rule_criterias (criteriaruleid)  

create table hm_rule_actions
(
	actionid int identity(1,1) not null,
	actionruleid int not null,
	actiontype tinyint not null,
	actionimapfolder nvarchar(255) not null,
	actionsubject nvarchar(255) not null,
	actionfromname nvarchar(255) not null,
	actionfromaddress nvarchar(255) not null,
	actionto nvarchar(255) not null,
	actionbody ntext not null,
	actionfilename nvarchar(255) not null,
	actionsortorder int not null,
	actionscriptfunction nvarchar(255) not null,
	actionheader nvarchar(80) not null,
	actionvalue nvarchar(255) not null,
   actionrouteid int not null
) 

ALTER TABLE hm_rule_actions ADD CONSTRAINT hm_rule_actions_pk PRIMARY KEY NONCLUSTERED (actionid) 

CREATE CLUSTERED INDEX idx_hm_rule_actions ON hm_rule_actions (actionruleid) 

create table hm_surblservers
(
	surblid int identity(1,1) not null ,
	surblactive tinyint not null,
	surblhost nvarchar(255) not null,
	surblrejectmessage nvarchar(255) not null,
	surblscore int not null
) 

ALTER TABLE hm_surblservers ADD CONSTRAINT hm_surblid_pk PRIMARY KEY NONCLUSTERED (surblid) 

create table hm_greylisting_triplets
(
	glid bigint identity(1,1) not null,
	glcreatetime datetime not null,
	glblockendtime datetime not null,
	gldeletetime datetime not null,
	glipaddress1 bigint not null,
   glipaddress2 bigint null,
	glsenderaddress nvarchar(200) not null,
	glrecipientaddress nvarchar(200) not null,
	glblockedcount int not null,
	glpassedcount int not null
) 

ALTER TABLE hm_greylisting_triplets ADD CONSTRAINT hm_glid_pk PRIMARY KEY NONCLUSTERED (glid) 

ALTER TABLE hm_greylisting_triplets ADD CONSTRAINT u_gltriplet UNIQUE NONCLUSTERED (glipaddress1, glipaddress2, glsenderaddress, glrecipientaddress) 

create table hm_greylisting_whiteaddresses
(
	whiteid bigint identity(1,1) not null,
	whiteipaddress nvarchar(255) not null,
	whiteipdescription nvarchar(255) not null
) 

ALTER TABLE hm_greylisting_whiteaddresses ADD CONSTRAINT hm_glwhite_pk PRIMARY KEY NONCLUSTERED (whiteid) 

ALTER TABLE hm_greylisting_whiteaddresses ADD CONSTRAINT u_glwhite UNIQUE NONCLUSTERED (whiteipaddress) 

create table hm_blocked_attachments
(
	baid bigint identity(1,1) not null,
	bawildcard nvarchar(255) not null,
	badescription nvarchar(255) not null
) 

ALTER TABLE hm_blocked_attachments ADD CONSTRAINT hm_baid_pk PRIMARY KEY NONCLUSTERED (baid) 

create table hm_servermessages
(
	smid int identity(1,1) not null , 
	smname nvarchar(255) not null,
	smtext ntext not null
) 

ALTER TABLE hm_servermessages ADD CONSTRAINT hm_smid_pk PRIMARY KEY NONCLUSTERED (smid) 

create table hm_tcpipports
(
	portid int identity(1,1) not null, 
	portprotocol tinyint not null,
	portnumber int not null,
	portaddress1 bigint not null,
    portaddress2 bigint null,
	portconnectionsecurity tinyint not null,
	portsslcertificateid bigint not null
) 

ALTER TABLE hm_tcpipports ADD CONSTRAINT hm_tcpipports_pk PRIMARY KEY NONCLUSTERED (portid) 

create table hm_whitelist
(
	whiteid bigint identity(1,1) not null,
	whiteloweripaddress1 bigint not null,
	whiteloweripaddress2 bigint null,
	whiteupperipaddress1 bigint not null,
	whiteupperipaddress2 bigint null,
	whiteemailaddress nvarchar(255) not null,
	whitedescription nvarchar(255) not null
) 

ALTER TABLE hm_whitelist ADD CONSTRAINT hm_whitelist_pk PRIMARY KEY NONCLUSTERED (whiteid) 

create table hm_sslcertificates
(
	sslcertificateid bigint identity(1,1) not null,
	sslcertificatename nvarchar(255) not null,
	sslcertificatefile nvarchar(255) not null,
	sslprivatekeyfile nvarchar(255) not null
) 

ALTER TABLE hm_sslcertificates ADD CONSTRAINT hm_sslcertificates_pk PRIMARY KEY NONCLUSTERED (sslcertificateid) 

create table hm_groups
(
	groupid bigint identity(1,1) not null,
   groupname nvarchar(255)
) 

ALTER TABLE hm_groups ADD CONSTRAINT hm_groups_pk PRIMARY KEY NONCLUSTERED (groupid) 

create table hm_group_members
(
	memberid bigint identity(1,1) not null,
	membergroupid bigint not null,
	memberaccountid bigint not null
) 

ALTER TABLE hm_group_members ADD CONSTRAINT hm_group_members_pk PRIMARY KEY NONCLUSTERED (memberid) 

create table hm_acl
(
	aclid bigint identity(1,1) not null,
   aclsharefolderid bigint not null,
	aclpermissiontype tinyint not null,
	aclpermissiongroupid bigint not null,
	aclpermissionaccountid bigint not null,
	aclvalue bigint not null
) 

ALTER TABLE hm_acl ADD CONSTRAINT hm_acl_pk PRIMARY KEY NONCLUSTERED (aclid) 

ALTER TABLE hm_acl ADD CONSTRAINT hm_acl_unique UNIQUE (aclsharefolderid, aclpermissiontype, aclpermissiongroupid, aclpermissionaccountid) 

create table hm_incoming_relays
(
   relayid int identity(1,1) not null,
   relayname nvarchar(100) not null,
   relaylowerip1 bigint not null,
   relaylowerip2 bigint null,
   relayupperip1 bigint not null,
   relayupperip2 bigint null
) 

ALTER TABLE hm_incoming_relays ADD CONSTRAINT hm_incoming_relays_pk PRIMARY KEY NONCLUSTERED (relayid)

create table hm_logon_failures
(
   ipaddress1 bigint not null,
   ipaddress2 bigint null,
   failuretime datetime not null
) 

CREATE NONCLUSTERED INDEX idx_hm_logon_failures_ipaddress ON hm_logon_failures (ipaddress1, ipaddress2) 

CREATE CLUSTERED INDEX idx_hm_logon_failures_failuretime ON hm_logon_failures (failuretime) 

insert into hm_securityranges (rangepriorityid, rangelowerip1, rangelowerip2, rangeupperip1, rangeupperip2, rangeoptions, rangename, rangeexpires, rangeexpirestime) values (10, 0, NULL, 4294967295, NULL, 96203, 'Internet', 0, '2001-01-01') 

insert into hm_securityranges (rangepriorityid, rangelowerip1, rangelowerip2, rangeupperip1, rangeupperip2, rangeoptions, rangename, rangeexpires, rangeexpirestime) values (30, 2130706433, NULL, 2130706433, NULL, 71627, 'My computer', 0, '2001-01-01') 

insert into hm_servermessages (smname, smtext) values ('VIRUS_FOUND', 'Virus found')  

insert into hm_servermessages (smname, smtext) values ('VIRUS_ATTACHMENT_REMOVED', 'Virus found:' + NCHAR(13) + NCHAR(10) + 'The attachment(s) of this message was removed since a virus was detected in at least one of them.' + NCHAR(13) + NCHAR(10) + NCHAR(13) + NCHAR(10)) 

insert into hm_servermessages (smname, smtext) values ('VIRUS_NOTIFICATION', 'The message below contained a virus and did not' + NCHAR(13) + NCHAR(10) + 'reach some or all of the intended recipients.' + NCHAR(13) + NCHAR(10) + '' + NCHAR(13) + NCHAR(10) + '   From: %MACRO_FROM%' + NCHAR(13) + NCHAR(10) + '   To: %MACRO_TO%' + NCHAR(13) + NCHAR(10) + '   Sent: %MACRO_SENT%' + NCHAR(13) + NCHAR(10) + '   Subject: %MACRO_SUBJECT%' + NCHAR(13) + NCHAR(10) + '' + NCHAR(13) + NCHAR(10) + 'hMailServer' + NCHAR(13) + NCHAR(10) + '')  

insert into hm_servermessages (smname, smtext) values ('SEND_FAILED_NOTIFICATION', 'Your message did not reach some or all of the intended recipients.' + NCHAR(13) + NCHAR(10) + '' + NCHAR(13) + NCHAR(10) + '   Sent: %MACRO_SENT%' + NCHAR(13) + NCHAR(10) + '   Subject: %MACRO_SUBJECT%' + NCHAR(13) + NCHAR(10) + '' + NCHAR(13) + NCHAR(10) + 'The following recipient(s) could not be reached:' + NCHAR(13) + NCHAR(10) + '' + NCHAR(13) + NCHAR(10) + '%MACRO_RECIPIENTS%' + NCHAR(13) + NCHAR(10) + '' + NCHAR(13) + NCHAR(10) + 'hMailServer' + NCHAR(13) + NCHAR(10) + '')  

insert into hm_servermessages (smname, smtext) values ('MESSAGE_UNDELIVERABLE', 'Message undeliverable')  

insert into hm_servermessages (smname, smtext) values ('MESSAGE_FILE_MISSING', 'The mail server could not deliver the message to you since the file %MACRO_FILE% does not exist on the server.' + NCHAR(13) + NCHAR(10) + '' + NCHAR(13) + NCHAR(10) + 'The file may have been deleted by anti virus software running on the server.' + NCHAR(13) + NCHAR(10) + '' + NCHAR(13) + NCHAR(10) + 'hMailServer')  

insert into hm_servermessages (smname, smtext) values ('ATTACHMENT_REMOVED', 'The attachment %MACRO_FILE% was blocked for delivery by the e-mail server. Please contact your system administrator if you have any questions regarding this.' + NCHAR(13) + NCHAR(10) + '' + NCHAR(13) + NCHAR(10) + 'hMailServer' + NCHAR(13) + NCHAR(10) + '')  

insert into hm_dnsbl (sblactive, sbldnshost, sblresult, sblrejectmessage, sblscore) values (0, 'zen.spamhaus.org', '127.0.0.2-8|127.0.0.10-11', 'Rejected by Spamhaus.',3 ) 

insert into hm_dnsbl (sblactive, sbldnshost, sblresult, sblrejectmessage, sblscore) values (0, 'bl.spamcop.net', '127.0.0.2', 'Rejected by SpamCop.', 3) 

insert into hm_surblservers (surblactive, surblhost, surblrejectmessage, surblscore) values (0, 'multi.surbl.org', 'Rejected by SURBL.', 3) 

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

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.pif', 'Program Information file') 

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.reg', 'Registration key') 

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.scf', 'Windows Explorer command') 

insert into hm_blocked_attachments (bawildcard, badescription) values ('*.scr', 'Windows Screen saver') 


insert into hm_settings (settingname, settingstring, settinginteger) values ('maxpop3connections', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxsmtpconnections', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('mirroremailaddress', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('relaymode', '', 2) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('authallowplaintext', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('allowmailfromnull', '', 1) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('logging', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('logdevice', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpnoofretries', '', 4) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpminutesbetweenretries', '', 60) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpnooftries', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolimap', '', 1) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolsmtp', '', 1) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolpop3', '', 1) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomeimap', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomepop3', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomesmtp', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayer', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxdelivertythreads', '', 10) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('logformat', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwinenable', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwinexec', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwindb', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('avnotifysender', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('avnotifyreceiver', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('avaction', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('sendstatistics', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('hostname', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerusername', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerpassword', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('usesmtprelayerauthentication', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerport', '', 25) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('usecustomvirusscanner', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('customvirusscannerexecutable', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('customviursscannerreturnvalue', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('usespf', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('usemxchecks', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('usescriptserver', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('scriptlanguage', 'VBScript', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxmessagesize', '', 20480)  

insert into hm_settings (settingname, settingstring, settinginteger) values ('usecache', '', 1) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('domaincachettl', '', 60)  

insert into hm_settings (settingname, settingstring, settinginteger) values ('accountcachettl', '', 60) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('awstatsenabled', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('rulelooplimit', '', 5) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('backupoptions', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('backupdestination', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('defaultdomain', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('avmaxmsgsize', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpdeliverybindtoip', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapquota', '', 1) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapidle', '', 1) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapacl', '', 1) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('maximapconnections', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapsort', '', 1) 

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

insert into hm_settings (settingname, settingstring, settinginteger) values ('aliascachettl', '', 60) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('distributionlistcachettl', '', 60) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerconnectionsecurity', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('adddeliveredtoheader', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('groupcachettl', '', 60) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('imappublicfoldername', '#Public', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamenabled', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('usespfscore', '', 3) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('ascheckhostinheloscore', '', 2) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('usemxchecksscore', '', 2) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('spammarkthreshold', '', 5) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamdeletethreshold', '', 20) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinenabled', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinscore', '', 5) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinmergescore', '', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinhost', '127.0.0.1', 0) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinport', '', 783) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispammaxsize', '', 1024)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDKIMVerificationEnabled', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDKIMVerificationFailureScore', '', 5)

insert into hm_settings (settingname, settingstring, settinginteger) values ('AutoBanOnLogonFailureEnabled', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('MaxInvalidLogonAttempts', '', 3)

insert into hm_settings (settingname, settingstring, settinginteger) values ('LogonAttemptsWithinMinutes', '', 30)

insert into hm_settings (settingname, settingstring, settinginteger) values ('AutoBanMinutes', '', 60)

insert into hm_settings (settingname, settingstring, settinginteger) values ('IMAPHierarchyDelimiter', '.', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('MaxNumberOfAsynchronousTasks', '', 15)

insert into hm_settings (settingname, settingstring, settinginteger) values ('MessageIndexing', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnSPFSuccess', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnMailFromMX', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('MaxNumberOfMXHosts', '', 15)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ClamAVEnabled', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ClamAVHost', 'localhost', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ClamAVPort', '', 3310)

insert into hm_settings (settingname, settingstring, settinginteger) values ('SmtpDeliveryConnectionSecurity', '', 2)

insert into hm_settings (settingname, settingstring, settinginteger) values ('VerifyRemoteSslCertificate', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('SslCipherList', 'ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:ECDHE-RSA-RC4-SHA:ECDHE-ECDSA-RC4-SHA:AES128:AES256:RC4-SHA:HIGH:!aNULL:!eNULL:!EXPORT:!DES:!3DES:!MD5:!PSK;', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('SslVersions', '', 14)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ImapMasterUser', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ImapAuthAllowPlainText', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('EnableImapSASLPlain', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('EnableImapSASLInitialResponse', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ascheckptr', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ascheckptrscore', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('IPv6Preferred', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('TlsOptions', '', 0)

insert into hm_tcpipports (portprotocol, portnumber, portaddress1, portaddress2, portconnectionsecurity, portsslcertificateid) values (1, 25, 0, NULL, 0, 0) 

insert into hm_tcpipports (portprotocol, portnumber, portaddress1, portaddress2, portconnectionsecurity, portsslcertificateid) values (1, 587, 0, NULL, 0, 0) 

insert into hm_tcpipports (portprotocol, portnumber, portaddress1, portaddress2, portconnectionsecurity, portsslcertificateid) values (3, 110, 0, NULL, 0, 0) 

insert into hm_tcpipports (portprotocol, portnumber, portaddress1, portaddress2, portconnectionsecurity, portsslcertificateid) values (5, 143, 0, NULL, 0, 0) 

insert into hm_dbversion values (5705)