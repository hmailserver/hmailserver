create table hm_serverlog (
	logid int identity (1, 1) not null ,
	logprocessid int not null,
	logthreadid int not null,
	logsource int not null,
	logtime datetime not null,
	logremotehost varchar(15) not null,
	logtext varchar(600)
)

create table hm_filters (
	filterid int identity (1, 1) not null ,
	filterdomainid int not null,
	filteraccountid int not null,
	filteractionid int not null,
	filtertypeid int not null,
	filtervalue varchar (255) not null,
	filterresult varchar(255) not null
) 

insert into hm_settings (settingname, settingstring, settinginteger) values ('logging', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('logdevice', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('filters', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('tarpitdelay', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('tarpitcount', '', 0)

update hm_dbversion set value = 1400

