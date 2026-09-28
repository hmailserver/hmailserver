insert into hm_settings (settingname, settingstring, settinginteger) values ('ASARCEnabled', '', 0);

create table hm_arc_trusted_sealers
(
   sealerid bigserial not null primary key,
   sealerdomain varchar(255) not null,
   sealerdescription varchar(255) not null
);

update hm_dbversion set value = 5714;
