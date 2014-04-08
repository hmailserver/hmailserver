ALTER TABLE hm_imapfolders ADD CONSTRAINT idx_hm_imapfolders_unique UNIQUE NONCLUSTERED (folderaccountid, folderparentid, foldername)

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnSPFSuccess', '', 0)

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

@@@UPDATE_MESSAGES_SET_FOLDER_INBOX@@@
--- Updates all messages currently pointing at folder ID 0 (old inbox) so that they point at
--- the actual Inbox folder ID instead.

update hm_imapfolders set foldercreationtime = GETDATE()

@@@UPDATE_FOLDERS_SET_CURRENT_UID@@@
--- Updates the hm_imapfolders table to contain up-to-date foldercurrentuid's.

@@@UPDATE_FOLDERS_SET_NEW_PARENTFOLDERID_WHERE_ZERO@@@
--- Updates the hm_imapfolders table and sets the folderparentid to the ID of the inbox where it's currently set to zero.

ALTER TABLE hm_fetchaccounts ADD faenablerouterecipients tinyint not null DEFAULT 0

delete from hm_settings where settingname = 'maskpasswordsinlog'

update hm_dbversion set value = 5200
