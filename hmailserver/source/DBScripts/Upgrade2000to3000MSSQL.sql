create table hm_imapfolders 
(
	folderid numeric identity (1, 1) not null,
	folderaccountid numeric NOT NULL,
	folderparentid int NOT NULL,
	foldername varchar(255) NOT NULL,
	folderissubscribed numeric NOT NULL
)

alter table hm_messages add messageflagseen numeric not null default 0

alter table hm_messages add messageflagdeleted numeric not null default 0

alter table hm_messages add messageflagrecent numeric not null default 0

alter table hm_messages add messagefolderid numeric not null default 0

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolimap', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolsmtp', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolpop3', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomeimap', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomepop3', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomesmtp', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayer', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('imapport', '', 143)

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxdelivertythreads', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('logformat', '', 0)

update hm_dbversion set value = 3000

