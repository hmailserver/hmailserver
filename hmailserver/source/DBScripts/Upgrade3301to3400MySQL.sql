ALTER TABLE hm_accounts ADD accountpwencryption TINYINT NOT NULL

ALTER TABLE hm_accounts CHANGE accountpassword accountpassword VARCHAR(255) NOT NULL

create table hm_deliverylog
(
	deliveryid int auto_increment not null, primary key(deliveryid), unique(deliveryid),
	deliveryfrom varchar(255) not null,
	deliveryfilename varchar(255) not null,
    	deliverytime datetime not null,
	deliverysubject varchar(255) not null,
	deliverybody text not null
)

create table hm_deliverylog_recipients
(
	deliveryrecipientid int auto_increment not null, primary key(deliveryrecipientid), unique(deliveryrecipientid),
    	deliveryid  int not null,
	deliveryrecipientaddress varchar(255) not null
)

insert into hm_settings (settingname, settingstring, settinginteger) values ('usedeliverylog', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('usecustomvirusscanner', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('customvirusscannerexecutable', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('customviursscannerreturnvalue', '', 0)

update hm_dbversion set value = 3400
