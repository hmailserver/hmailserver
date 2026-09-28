insert into hm_settings (settingname, settingstring, settinginteger) values ('ASARCEnabled', '', 0)

create table hm_arc_trusted_sealers
(
   sealerid bigint identity(1,1) not null,
   sealerdomain nvarchar(255) not null,
   sealerdescription nvarchar(255) not null
)

ALTER TABLE hm_arc_trusted_sealers ADD CONSTRAINT hm_arc_trusted_sealers_pk PRIMARY KEY NONCLUSTERED (sealerid)

update hm_dbversion set value = 5714