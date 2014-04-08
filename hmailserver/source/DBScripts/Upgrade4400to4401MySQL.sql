create table hm_whitelist
(
	whiteid bigint auto_increment not null, primary key(whiteid), unique(whiteid),
	whiteloweripaddress bigint not null,
	whiteupperipaddress bigint not null,
	whiteemailaddress varchar(255) not null,
	whitedescription varchar(255) not null
)

insert into hm_settings (settingname, settingstring, settinginteger) values ('aliascachettl', '', 60)

insert into hm_settings (settingname, settingstring, settinginteger) values ('distributionlistcachettl', '', 60)

ALTER TABLE hm_accounts ADD COLUMN accountvacationexpires tinyint unsigned not null DEFAULT 0

ALTER TABLE hm_accounts ADD COLUMN accountvacationexpiredate datetime NOT NULL

ALTER TABLE hm_fetchaccounts ADD COLUMN faprocessmimedate tinyint not null DEFAULT 0

update hm_dbversion set value = 4401

