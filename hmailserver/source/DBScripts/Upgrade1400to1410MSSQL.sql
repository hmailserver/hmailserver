alter table hm_accounts add accountmaxsize integer not null DEFAULT 0

alter table hm_domains add domainmaxsize integer not null DEFAULT 0

insert into hm_settings (settingname, settingstring, settinginteger) values ('useordb', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpnooftries', '', 0)

update hm_dbversion set value = 1410

