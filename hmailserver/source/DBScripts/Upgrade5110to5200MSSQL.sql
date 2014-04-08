ALTER TABLE hm_imapfolders ADD CONSTRAINT idx_hm_imapfolders_unique UNIQUE NONCLUSTERED (folderaccountid, folderparentid, foldername)

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnSPFSuccess', '', 0)

if exists (select * from sysobjects where id = object_id('hm_drop_table_indexes')) drop proc hm_drop_table_indexes

-- Drops all indexes on a specific table. Does not drop unique constraints/indexes.
CREATE proc hm_drop_table_indexes
	@tablename sysname
as
	set nocount on
	declare @constname sysname,
	@cmd varchar(1024)
	declare curs_constraints cursor for
	-- delete indexes for column
	select name from sysindexes 
	where id = OBJECT_ID(@tablename) and 
		  INDEXPROPERTY(id, name, 'IsStatistics') = 0 and
		  indid in (select indid from sysindexkeys where id = OBJECT_ID(@tablename)) and
		  name not in (SELECT CONSTRAINT_NAME FROM INFORMATION_SCHEMA.CONSTRAINT_COLUMN_USAGE WHERE TABLE_NAME = @tablename) 
	
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
return 0

--- We will recreate the index in the bottom of this script.
hm_drop_table_indexes 'hm_messages'

insert into hm_settings (settingname, settingstring, settinginteger) values ('MessageIndexing', '', 0)

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

ALTER TABLE  hm_messages add messageuid bigint NOT NULL DEFAULT 0

ALTER TABLE  hm_imapfolders add foldercreationtime datetime NOT NULL DEFAULT GETDATE()

ALTER TABLE  hm_imapfolders add foldercurrentuid bigint NOT NULL DEFAULT 0

insert into hm_imapfolders
	(folderaccountid, folderparentid, foldername, folderissubscribed, foldercurrentuid, foldercreationtime)
select
	accountid, -1, 'INBOX', 1, 0, GETDATE() from hm_accounts
   
update hm_messages set messageuid = messageid

update hm_messages set messagefolderid = 
   (select ISNULL(max(folderid), 0) from hm_imapfolders where foldername = 'Inbox' and folderaccountid = messageaccountid and folderparentid = -1)
   where messagefolderid = 0 and messagetype = 2
   
update hm_imapfolders set foldercreationtime = GETDATE()

update hm_imapfolders set foldercurrentuid = ISNULL((select max(messageid) from hm_messages where folderid = messagefolderid), 0)

update hm_imapfolders set folderparentid = 
   (select folderid from hm_imapfolders folderinner where foldername = 'Inbox' and folderinner.folderaccountid = hm_imapfolders.folderaccountid)
where folderparentid = 0

ALTER TABLE hm_fetchaccounts ADD faenablerouterecipients tinyint not null DEFAULT 0

delete from hm_settings where settingname = 'maskpasswordsinlog'

--- Recreate the indes we dropped int the beginning of this script.
CREATE CLUSTERED INDEX idx_hm_messages ON hm_messages (messageaccountid, messagefolderid)  

CREATE INDEX idx_hm_messages_type ON hm_messages (messagetype) 

update hm_dbversion set value = 5200
