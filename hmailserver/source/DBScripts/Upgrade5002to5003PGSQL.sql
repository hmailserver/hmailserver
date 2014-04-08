DROP TABLE hm_acl;

create table hm_acl
(
	aclid bigserial not null primary key,
   aclsharefolderid bigint not null,
	aclpermissiontype smallint not null,
	aclpermissiongroupid bigint not null,
	aclpermissiondomainid bigint not null,
	aclpermissionaccountid bigint not null,
	aclvalue bigint not null,
	unique(aclsharefolderid, aclpermissiontype, aclpermissiongroupid, aclpermissiondomainid, aclpermissionaccountid)
);

update hm_settings set settingstring = '#Public' where settingname = 'imappublicfoldername';

update hm_dbversion set value = 5003;

