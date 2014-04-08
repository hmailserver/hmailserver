insert into hm_settings (settingname, settingstring, settinginteger) values ('usespamhaus', '', 0)

create table hm_securityranges
(
	rangeid int auto_increment not null, primary key(rangeid), unique(rangeid),
   	rangepriorityid int not null,
	rangelowerip bigint unsigned not null,
	rangeupperip bigint unsigned not null,
	rangeoptions int not null
)

update hm_dbversion set value = 1600

