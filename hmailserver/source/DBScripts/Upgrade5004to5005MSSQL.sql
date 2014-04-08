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



drop table hm_groups

drop table hm_group_members

drop table hm_acl

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

ALTER TABLE hm_acl ADD CONSTRAINT hm_acl_unique UNIQUE (aclsharefolderid, aclpermissiontype, aclpermissiongroupid,aclpermissionaccountid) 

hm_drop_table_column 'hm_messages', 'messagerecipients'

hm_drop_table_column 'hm_messages', 'messagerecipientsparsed'

drop table hm_adsynchronization

hm_drop_table_column 'hm_messagerecipients', 'recipientislocal'

hm_drop_table_column 'hm_messagerecipients', 'recipientisenabled'

hm_drop_table_column 'hm_messagerecipients', 'recipientisexisting'

hm_drop_table_column 'hm_aliases', 'aliastype'

update hm_dbversion set value = 5005

