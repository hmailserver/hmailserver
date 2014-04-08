CREATE UNIQUE INDEX idx_hm_imapfolders_unique ON hm_imapfolders (`folderaccountid`, `folderparentid`,  `foldername`);

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnSPFSuccess', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('MessageIndexing', '', 0);

create table hm_message_metadata 
(
   metadata_id bigint auto_increment not null, primary key(`metadata_id`),
   metadata_accountid int not null,
   metadata_folderid int not null,
   metadata_messageid bigint  not null,
   metadata_dateutc datetime null,
   metadata_from varchar(255) not null,
   metadata_subject varchar(255) not null,
   metadata_to varchar(255) not null,
   metadata_cc varchar(255) not null
) DEFAULT CHARSET=utf8;

CREATE UNIQUE INDEX idx_message_metadata_unique ON hm_message_metadata (`metadata_accountid`,  `metadata_folderid`, `metadata_messageid`);

CREATE INDEX idx_message_metadata_id ON hm_message_metadata (`metadata_messageid`);

ALTER TABLE  hm_messages add column messageuid bigint NOT NULL;

ALTER TABLE  hm_imapfolders add column foldercreationtime datetime NOT NULL;

ALTER TABLE  hm_imapfolders add column foldercurrentuid bigint NOT NULL;

insert into hm_imapfolders
	(folderaccountid, folderparentid, foldername, folderissubscribed, foldercurrentuid, foldercreationtime)
select
	accountid, -1, 'INBOX', 1, 0, NOW() from hm_accounts;

update hm_messages set messageuid = messageid;

update hm_messages set messagefolderid = 
	IFNULL((select max(folderid) from hm_imapfolders where foldername = 'Inbox' and folderaccountid = messageaccountid and folderparentid = -1) ,0)
where messagefolderid = 0 and messagetype = 2;

update hm_imapfolders set foldercreationtime = NOW();

update hm_imapfolders, (select messagefolderid, max(messageid) uid from hm_messages group by messagefolderid) as x
set 
	hm_imapfolders.foldercurrentuid = x.uid
where hm_imapfolders.folderid = messagefolderid

/* MySQL does not support support UPDATE .. FROM syntax when the target table is the same as the source table. Hence the inner join
   rather than a simple UPDATE TABLE SET VALUE = (SELECT...) */
update hm_imapfolders folders1
	inner join hm_imapfolders folders2 on (folders1.folderaccountid = folders2.folderaccountid and folders2.foldername = 'Inbox')
set folders1.folderparentid = folders2.folderid
where folders1.folderparentid = 0;

ALTER TABLE hm_fetchaccounts add column faenablerouterecipients tinyint not null DEFAULT 0;

delete from hm_settings where settingname = 'maskpasswordsinlog';

update hm_dbversion set value = 5200;

