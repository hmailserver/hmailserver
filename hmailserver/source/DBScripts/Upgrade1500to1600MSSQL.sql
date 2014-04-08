insert into hm_settings (settingname, settingstring, settinginteger) values ('usespamhaus', '', 0)

create table hm_securityranges
(
	rangeid int identity (1, 1) not null,
    	rangepriorityid int not null,
	rangelowerip int not null,
	rangeupperip int not null,
	rangeoptions int not null
)

update hm_dbversion set value = 1600

