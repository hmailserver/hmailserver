insert into hm_settings (settingname, settingstring, settinginteger) values ('ImapMasterUser', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ImapAuthAllowPlainText', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('EnableImapSASLPlain', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('EnableImapSASLInitialResponse', '', 0);

drop table if exists hm_flags;

create table hm_flags 
(
	
	MsgID int not null ,
	UsrID in not null ,
	Flag int,
	primary key (MsgID,UsrID)
) DEFAULT CHARSET=utf8;

update hm_dbversion set value = 5700;