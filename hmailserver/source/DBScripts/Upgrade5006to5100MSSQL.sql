if exists (select * from sysobjects where id = object_id('hm_drop_table_column')) drop proc hm_drop_table_column

CREATE proc hm_drop_table_column
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

insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDKIMVerificationEnabled', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDKIMVerificationFailureScore', '', 5)

delete from hm_settings where settingname = 'maskpasswordsinlog'

ALTER TABLE hm_domains ADD domaindkimselector nvarchar(255) NOT NULL DEFAULT ''

ALTER TABLE hm_domains ADD domaindkimprivatekeyfile nvarchar(255) NOT NULL DEFAULT ''

update hm_domains set domainantispamoptions = 1 where domainantispamoptions = 2047

create table hm_incoming_relays
(
   relayid int identity(1,1) not null,
   relayname nvarchar(100) not null,
   relaylowerip bigint not null,
   relayupperip bigint not null
) 

ALTER TABLE hm_incoming_relays ADD CONSTRAINT hm_incoming_relays_pk PRIMARY KEY NONCLUSTERED (relayid)

insert into hm_incoming_relays
(relayname, relaylowerip, relayupperip)
select rangename, rangelowerip, rangeupperip from hm_securityranges where rangeoptions & 2048 = 2048

update hm_securityranges set rangeoptions = rangeoptions | 66048  where (rangeoptions & 32) <> 32

update hm_securityranges set rangeoptions = rangeoptions | 90624  where rangeoptions & 32 = 32

update hm_securityranges set rangeoptions = rangeoptions & ~2048

update hm_securityranges set rangeoptions = rangeoptions & ~16

update hm_securityranges set rangeoptions = rangeoptions & ~32

EXEC sp_rename
    @objname = 'hm_messages.messageflagseen',
    @newname = 'messageflags',
    @objtype = 'COLUMN'

--- Make sure the column is of type tinyint. In early versions of
--- hMailServer the column was of type numeric. The bitwise operations
--- below won't work with that data type.
alter table hm_messages alter column messageflags tinyint not null

update hm_messages set messageflags = messageflags | 2 where messageflagdeleted = 1

update hm_messages set messageflags = messageflags | 4 where messageflagflagged = 1

update hm_messages set messageflags = messageflags | 8 where messageflaganswered = 1

update hm_messages set messageflags = messageflags | 16 where messageflagdraft = 1

update hm_messages set messageflags = messageflags | 32 where messageflagrecent = 1

hm_drop_table_column hm_messages, messageflagdeleted

hm_drop_table_column hm_messages, messageflagflagged

hm_drop_table_column hm_messages, messageflaganswered

hm_drop_table_column hm_messages, messageflagdraft

hm_drop_table_column hm_messages, messageflagrecent

alter table hm_fetchaccounts ADD fauseantispam tinyint not null default 0

alter table hm_fetchaccounts ADD fauseantivirus tinyint not null default 0

update hm_fetchaccounts set fauseantispam = 1

update hm_fetchaccounts set fauseantivirus = 1

alter table hm_securityranges ADD rangeexpires tinyint not null default 0

alter table hm_securityranges ADD rangeexpirestime datetime not null default '2001-01-01'

create table hm_logon_failures
(
   ipaddress bigint not null,
   failuretime datetime not null
) 

CREATE NONCLUSTERED INDEX idx_hm_logon_failures_ipaddress ON hm_logon_failures (ipaddress) 

CREATE CLUSTERED INDEX idx_hm_logon_failures_failuretime ON hm_logon_failures (failuretime) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('AutoBanOnLogonFailureEnabled', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('MaxInvalidLogonAttempts', '', 3)

insert into hm_settings (settingname, settingstring, settinginteger) values ('LogonAttemptsWithinMinutes', '', 30)

insert into hm_settings (settingname, settingstring, settinginteger) values ('AutoBanMinutes', '', 60)

update hm_securityranges set rangeoptions = rangeoptions & ~16

insert into hm_settings (settingname, settingstring, settinginteger) values ('IMAPHierarchyDelimiter', '.', 0)

delete from hm_settings where settingname = 'usedeliverylog'

update hm_dbversion set value = 5100
