insert into hm_settings (settingname, settingstring, settinginteger) values ('SRSEnabled', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('SRSSecret', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('SRSMaxAgeDays', '', 21)

insert into hm_settings (settingname, settingstring, settinginteger) values ('SRSHashLength', '', 8)

update hm_dbversion set value = 5712
