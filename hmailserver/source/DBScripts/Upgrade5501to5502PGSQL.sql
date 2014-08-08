ALTER TABLE hm_settings ALTER COLUMN settingstring TYPE varchar (4000);

insert into hm_settings (settingname, settingstring, settinginteger) values ('SslCipherList', '', 0);

update hm_dbversion set value = 5502;

