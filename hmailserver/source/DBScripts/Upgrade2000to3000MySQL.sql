create table hm_imapfolders 
(
	folderid mediumint(9) NOT NULL auto_increment, primary key(folderid), unique(folderid),
	folderaccountid mediumint(9) unsigned NOT NULL,
	folderparentid mediumint(9) NOT NULL,
	foldername varchar(255) NOT NULL,
	folderissubscribed tinyint(3) unsigned NOT NULL
)

alter table hm_messages add messageflagseen tinyint(3) not null default '0'

alter table hm_messages add messageflagdeleted tinyint(3) not null default '0'

alter table hm_messages add messageflagrecent tinyint(3) not null default '0'

alter table hm_messages add messagefolderid mediumint(9) not null default '0'

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolimap', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolsmtp', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('protocolpop3', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomeimap', '', '')

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomepop3', '', '')

insert into hm_settings (settingname, settingstring, settinginteger) values ('welcomesmtp', '', '')

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayer', '', '')

insert into hm_settings (settingname, settingstring, settinginteger) values ('imapport', '', 143)

insert into hm_settings (settingname, settingstring, settinginteger) values ('maxdelivertythreads', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('logformat', '', 0)

update hm_dbversion set value = 3000

