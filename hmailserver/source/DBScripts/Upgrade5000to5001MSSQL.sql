if exists (select * from sysobjects where id = object_id('hm_drop_table_objects')) drop proc hm_drop_table_objects

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

hm_drop_table_objects 'hm_accounts'

ALTER TABLE hm_accounts ALTER COLUMN accountaddomain nvarchar(255)

ALTER TABLE hm_accounts ALTER COLUMN accountadusername nvarchar(255)

ALTER TABLE hm_accounts ALTER COLUMN accountvacationmessage nvarchar(1000)

ALTER TABLE hm_accounts ALTER COLUMN accountvacationsubject nvarchar(200)

ALTER TABLE hm_accounts ADD accountsignatureplaintext_tmp ntext

UPDATE hm_accounts SET accountsignatureplaintext_tmp = accountsignatureplaintext

ALTER TABLE hm_accounts DROP COLUMN accountsignatureplaintext

sp_rename 'hm_accounts.accountsignatureplaintext_tmp', 'accountsignatureplaintext', 'COLUMN'

ALTER TABLE hm_accounts ADD accountsignaturehtml_tmp ntext

UPDATE hm_accounts SET accountsignaturehtml_tmp = accountsignaturehtml

ALTER TABLE hm_accounts DROP COLUMN accountsignaturehtml

sp_rename 'hm_accounts.accountsignaturehtml_tmp', 'accountsignaturehtml', 'COLUMN'

ALTER TABLE hm_accounts ALTER COLUMN accountpersonfirstname nvarchar(60)

ALTER TABLE hm_accounts ALTER COLUMN accountpersonlastname nvarchar(60)

alter table hm_accounts add constraint hm_accounts_pk primary key nonclustered (accountid) 

alter table hm_accounts add constraint u_accountaddress unique nonclustered (accountaddress) 

create clustered index idx_hm_accounts on hm_accounts (accountaddress) 

hm_drop_table_objects 'hm_aliases'

alter table hm_aliases add constraint hm_aliases_pk primary key nonclustered (aliasid) 

alter table hm_aliases add constraint u_aliasname unique nonclustered (aliasname) 

create clustered index idx_hm_aliases on hm_aliases (aliasdomainid, aliasname) 

hm_drop_table_objects 'hm_domains'

ALTER TABLE hm_domains ALTER COLUMN domainaddomain nvarchar(255)

ALTER TABLE hm_domains ADD domainsignatureplaintext_tmp ntext

UPDATE hm_domains SET domainsignatureplaintext_tmp = domainsignatureplaintext

ALTER TABLE hm_domains DROP COLUMN domainsignatureplaintext

sp_rename 'hm_domains.domainsignatureplaintext_tmp', 'domainsignatureplaintext', 'COLUMN'

ALTER TABLE hm_domains ADD domainsignaturehtml_tmp ntext

UPDATE hm_domains SET domainsignaturehtml_tmp = domainsignaturehtml

ALTER TABLE hm_domains DROP COLUMN domainsignaturehtml

sp_rename 'hm_domains.domainsignaturehtml_tmp', 'domainsignaturehtml', 'COLUMN'

alter table hm_domains add constraint hm_domains_pk primary key nonclustered (domainid) 

alter table hm_domains add constraint u_domainname unique nonclustered (domainname) 

create clustered index idx_hm_domains on hm_domains (domainname)  

hm_drop_table_objects 'hm_domain_aliases'

alter table hm_domain_aliases add constraint hm_domain_aliases_pk primary key nonclustered (daid) 

ALTER TABLE hm_settings ALTER COLUMN settingstring nvarchar(255)

hm_drop_table_objects 'hm_securityranges'

ALTER TABLE hm_securityranges ALTER COLUMN rangename nvarchar(100)

alter table hm_securityranges add constraint hm_securityranges_pk primary key nonclustered (rangeid) 

alter table hm_securityranges add constraint u_rangename unique nonclustered (rangename) 

hm_drop_table_objects 'hm_routes'

ALTER TABLE hm_routes ALTER COLUMN routedescription nvarchar(255)

alter table hm_routes add constraint hm_routes_pk primary key nonclustered (routeid) 

alter table hm_routes add constraint u_routedomainname unique nonclustered (routedomainname) 

hm_drop_table_objects 'hm_rules'

ALTER TABLE hm_rules ALTER COLUMN rulename nvarchar(100)

alter table hm_rules add constraint hm_rules_pk primary key nonclustered (ruleid) 

create clustered index idx_hm_rules on hm_rules (ruleaccountid)  

hm_drop_table_objects 'hm_rule_actions'

ALTER TABLE hm_rule_actions ALTER COLUMN actionsubject nvarchar(255)

ALTER TABLE hm_rule_actions ALTER COLUMN actionfromname nvarchar(255)

ALTER TABLE hm_rule_actions ALTER COLUMN actionto nvarchar(255)

ALTER TABLE hm_rule_actions ADD actionbody_tmp ntext

UPDATE hm_rule_actions SET actionbody_tmp = actionbody

ALTER TABLE hm_rule_actions DROP COLUMN actionbody

sp_rename 'hm_rule_actions.actionbody_tmp', 'actionbody', 'COLUMN'

ALTER TABLE hm_rule_actions ALTER COLUMN actionfilename nvarchar(255)

ALTER TABLE hm_rule_actions ALTER COLUMN actionvalue nvarchar(255)

alter table hm_rule_actions add constraint hm_rule_actions_pk primary key nonclustered (actionid) 

create clustered index idx_hm_rule_actions on hm_rule_actions (actionruleid) 

hm_drop_table_objects 'hm_greylisting_whiteaddresses'

ALTER TABLE hm_greylisting_whiteaddresses ALTER COLUMN whiteipdescription nvarchar(255)

alter table hm_greylisting_whiteaddresses add constraint hm_glwhite_pk primary key nonclustered (whiteid) 

alter table hm_greylisting_whiteaddresses add constraint u_glwhite unique nonclustered (whiteipaddress) 

hm_drop_table_objects 'hm_blocked_attachments'

ALTER TABLE hm_blocked_attachments ALTER COLUMN bawildcard nvarchar(255)

ALTER TABLE hm_blocked_attachments ALTER COLUMN badescription nvarchar(255)

alter table hm_blocked_attachments add constraint hm_baid_pk primary key nonclustered (baid) 

hm_drop_table_objects 'hm_servermessages'

ALTER TABLE hm_servermessages ADD smtext_tmp ntext

UPDATE hm_servermessages SET smtext_tmp = smtext

ALTER TABLE hm_servermessages DROP COLUMN smtext

sp_rename 'hm_servermessages.smtext_tmp', 'smtext', 'COLUMN'

alter table hm_servermessages add constraint hm_smid_pk primary key nonclustered (smid) 

hm_drop_table_objects 'hm_tcpipports'

alter table hm_tcpipports add constraint hm_tcpipports_pk primary key nonclustered (portid) 

hm_drop_table_objects 'hm_whitelist'

ALTER TABLE hm_whitelist ALTER COLUMN whitedescription nvarchar(255)

alter table hm_whitelist add constraint hm_whitelist_pk primary key nonclustered (whiteid) 

hm_drop_table_objects 'hm_sslcertificates'

ALTER TABLE hm_sslcertificates ALTER COLUMN sslcertificatename nvarchar(255)

ALTER TABLE hm_sslcertificates ALTER COLUMN sslcertificatefile nvarchar(255)

ALTER TABLE hm_sslcertificates ALTER COLUMN sslprivatekeyfile nvarchar(255)

alter table hm_sslcertificates add constraint hm_sslcertificates_pk primary key nonclustered (sslcertificateid) 

hm_drop_table_objects 'hm_groups'

ALTER TABLE hm_groups ALTER COLUMN groupname nvarchar(255)

alter table hm_groups add constraint hm_groups_pk primary key nonclustered (groupid) 

hm_drop_table_objects 'hm_group_members'

alter table hm_group_members add constraint hm_group_members_pk primary key nonclustered (memberid) 

hm_drop_table_objects 'hm_acl'

alter table hm_acl add constraint hm_acl_pk primary key nonclustered (aclid) 

alter table hm_acl add constraint hm_acl_unique unique (aclshareaccountid, aclsharefolderid, aclpermissiontype, aclpermissiongroupid, aclpermissiondomainid, aclpermissionaccountid) 

update hm_dbversion set value = 5001

