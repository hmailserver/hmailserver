insert into hm_settings (settingname, settingstring, settinginteger) values ('ImapMasterUser', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ImapAuthAllowPlainText', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('EnableImapSASLPlain', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('EnableImapSASLInitialResponse', '', 0)

update hm_dbversion set value = 5700;