insert into hm_settings (settingname, settingstring, settinginteger) values ('ImapMasterUser', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ImapAuthAllowPlainText', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('EnableImapSASLPlain', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('EnableImapSASLInitialResponse', '', 0)

if exists (select * from sysobjects where id = object_id('hm_flags') and objectproperty(id, 'isusertable') = 1) drop table hm_flags 

create table hm_flags (
   MsgID int not null,
   UsrID int not null,
   Flag int
)

ALTER TABLE hm_flags ADD CONSTRAINT hm_flags_pk PRIMARY KEY NONCLUSTERED (MsgID,UsrID) 

update hm_dbversion set value = 5700;