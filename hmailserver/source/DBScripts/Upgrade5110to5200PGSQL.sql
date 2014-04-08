CREATE UNIQUE INDEX idx_hm_imapfolders_unique ON hm_imapfolders (folderaccountid, folderparentid, foldername);

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnSPFSuccess', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('MessageIndexing', '', 0);

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

ALTER TABLE  hm_messages add messageuid bigint NOT NULL DEFAULT 0;

ALTER TABLE  hm_imapfolders add foldercreationtime timestamp NOT NULL DEFAULT NOW();

ALTER TABLE  hm_imapfolders add foldercurrentuid bigint NOT NULL DEFAULT 0;

insert into hm_imapfolders
	(folderaccountid, folderparentid, foldername, folderissubscribed, foldercurrentuid, foldercreationtime)
select
	accountid, -1, 'INBOX', 1, 0, NOW() from hm_accounts;

update hm_messages set messageuid = messageid;

update hm_messages set messagefolderid = 
   COALESCE((select max(folderid) from hm_imapfolders where foldername = 'INBOX' and folderaccountid = messageaccountid and folderparentid = -1),0)
where messagefolderid = 0 and messagetype = 2;

update hm_imapfolders set foldercreationtime = NOW();

update hm_imapfolders set foldercurrentuid = COALESCE((select max(messageid) from hm_messages where folderid = messagefolderid), 0);

update hm_imapfolders set folderparentid = 
   (select folderid from hm_imapfolders folderinner where foldername = 'INBOX' and folderinner.folderaccountid = hm_imapfolders.folderaccountid)
where folderparentid = 0;

ALTER TABLE hm_fetchaccounts add column faenablerouterecipients smallint not null DEFAULT 0;

delete from hm_settings where settingname = 'maskpasswordsinlog';

update hm_dbversion set value = 5200;
