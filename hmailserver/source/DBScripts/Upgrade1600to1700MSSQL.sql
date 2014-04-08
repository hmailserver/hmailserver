alter table hm_messages add messagecurnooftries int not null default 0

alter table hm_messages add messagenexttrytime datetime not null default '1901-01-01'

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpnoofretries', '', 4)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpminutesbetweenretries', '', 60)

update hm_dbversion set value = 1700

