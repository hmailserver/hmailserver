insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashAlgorithm', '', 1);

-- 0 means "use algorithm default" (Argon2id: memory cost 19456 KB, 2 iterations).
insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashMemoryCost', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashIterations', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('PasswordHashAutoUpgrade', '', 0);

update hm_dbversion set value = 5710;
