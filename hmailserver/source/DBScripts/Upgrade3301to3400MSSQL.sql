alter table hm_accounts add accountpwencryption	tinyint not null default 0

ALTER TABLE hm_accounts ALTER COLUMN accountpassword VARCHAR(255) NOT NULL

create table hm_deliverylog
(
	deliveryid numeric identity (1, 1) not null,
    deliveryfrom varchar(255) not null,
    deliveryfilename varchar(255) not null,
    deliverytime datetime not null,
	deliverysubject varchar(255) not null,
	deliverybody varchar(7000) not null
)

create table hm_deliverylog_recipients
(
	deliveryrecipientid numeric identity (1, 1) not null,
    deliveryid numeric not null,
	deliveryrecipientaddress varchar(255) not null
)

insert into hm_settings (settingname, settingstring, settinginteger) values ('usedeliverylog', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('usecustomvirusscanner', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('customvirusscannerexecutable', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('customviursscannerreturnvalue', '', 0)


update hm_dbversion set value = 3400

