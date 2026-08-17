insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashAlgorithm', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashMemoryCost', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashIterations', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashAutoUpgrade', '', 0)

update hm_dbversion set value = 5710
