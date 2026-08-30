insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDMARCEnabled', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDMARCFailureScore', '', 5)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDMARCHonorPolicy', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ASAddAuthResultsHeader', '', 0)

update hm_dbversion set value = 5711
