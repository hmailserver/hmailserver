ALTER TABLE hm_messages ADD messageflaganswered integer not null DEFAULT 0

ALTER TABLE hm_messages ADD messageflagflagged integer not null DEFAULT 0

ALTER TABLE hm_messages ADD messageflagdraft integer not null DEFAULT 0

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwinenable', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwinexec', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avclamwindb', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avnotifysender', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avnotifyreceiver', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('avaction', '', 0)


update hm_dbversion set value = 3200

