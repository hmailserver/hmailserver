create table hm_whitelist
(
	whiteid bigint identity(1,1) not null CONSTRAINT hm_whitelist_pk PRIMARY KEY NONCLUSTERED,
	whiteloweripaddress bigint not null,
	whiteupperipaddress bigint not null,
	whiteemailaddress varchar(255) not null,
	whitedescription varchar(255) not null
)

insert into hm_settings (settingname, settingstring, settinginteger) values ('aliascachettl', '', 60)

insert into hm_settings (settingname, settingstring, settinginteger) values ('distributionlistcachettl', '', 60)

ALTER TABLE hm_accounts ADD accountvacationexpires tinyint NOT NULL DEFAULT 0

ALTER TABLE hm_accounts ADD accountvacationexpiredate datetime NOT NULL DEFAULT GETDATE()

ALTER TABLE hm_fetchaccounts ADD faprocessmimedate tinyint not null DEFAULT 0

update hm_dbversion set value = 4401

