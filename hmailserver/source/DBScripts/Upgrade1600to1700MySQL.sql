alter table hm_messages add messagecurnooftries int not null;

alter table hm_messages add messagenexttrytime datetime not null

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpnoofretries', '', 4)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtpminutesbetweenretries', '', 60)

update hm_dbversion set value = 1700

