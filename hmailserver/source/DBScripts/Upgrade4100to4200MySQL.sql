insert into hm_settings (settingname, settingstring, settinginteger) values ('accountcachettl', '', 60)

insert into hm_settings (settingname, settingstring, settinginteger) values ('awstatsenabled', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('rulelooplimit', '', 5)

insert into hm_settings (settingname, settingstring, settinginteger) values ('backupoptions', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('backupdestination', '', 0)

ALTER TABLE hm_messages CHANGE messagefrom messagefrom VARCHAR(255) NOT NULL

ALTER TABLE hm_messages CHANGE messagesize messagesize bigint NOT NULL

update hm_securityranges set rangeoptions = rangeoptions | 1024

CREATE INDEX idx_hm_messages_type ON hm_messages (messagetype)

update hm_dbversion set value = 4200

