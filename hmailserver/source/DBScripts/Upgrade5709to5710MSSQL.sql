insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashAlgorithm', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashMemoryCost', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashIterations', '', 0)

-- Existing installations keep the migration switched off, so that upgrading
-- does not re-hash every account during the first burst of logons.
insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashAutoUpgrade', '', 0)

update hm_dbversion set value = 5710
