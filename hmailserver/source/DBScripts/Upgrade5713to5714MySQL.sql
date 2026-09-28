insert into hm_settings (settingname, settingstring, settinginteger) values ('ASARCEnabled', '', 0);

create table hm_arc_trusted_sealers
(
   sealerid bigint auto_increment not null, primary key(sealerid), unique(sealerid),
   sealerdomain varchar(255) not null,
   sealerdescription varchar(255) not null
) DEFAULT CHARSET=utf8;

update hm_dbversion set value = 5714;
