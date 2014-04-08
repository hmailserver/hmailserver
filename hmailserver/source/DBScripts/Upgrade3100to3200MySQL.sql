ALTER TABLE hm_messages ADD COLUMN messageflaganswered tinyint not null

ALTER TABLE hm_messages ADD COLUMN messageflagflagged tinyint not null

ALTER TABLE hm_messages ADD COLUMN messageflagdraft tinyint not null

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwinenable', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwinexec', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwindb', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avnotifysender', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avnotifyreceiver', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avaction', '', 0)


update hm_dbversion set value = 3200

