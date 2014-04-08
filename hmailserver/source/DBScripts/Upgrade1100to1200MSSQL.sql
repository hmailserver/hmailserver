insert into hm_settings (settingname, settingstring, settinginteger) values ('relaymode', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('authallowplaintext', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('allowmailfromnull', '', 1)

alter table hm_domains add requiresmtpauth int not null

update hm_dbversion set value = 1200

