ALTER TABLE hm_settings ALTER COLUMN settingstring nvarchar(4000) NOT NULL

insert into hm_settings (settingname, settingstring, settinginteger) values ('SslCipherList', '', 0)

update hm_dbversion set value = 5502

