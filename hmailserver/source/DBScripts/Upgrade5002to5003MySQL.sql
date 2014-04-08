drop table hm_acl

create table hm_acl
(
	aclid bigint auto_increment not null, primary key(`aclid`), unique(`aclid`),
   aclsharefolderid bigint not null,
	aclpermissiontype tinyint not null,
	aclpermissiongroupid bigint not null,
	aclpermissiondomainid bigint not null,
	aclpermissionaccountid bigint not null,
	aclvalue bigint not null,
	unique(aclsharefolderid, aclpermissiontype, aclpermissiongroupid, aclpermissiondomainid, aclpermissionaccountid)
) DEFAULT CHARSET=utf8;

update hm_settings set settingstring = '#Public' where settingname = 'imappublicfoldername'

update hm_dbversion set value = 5003

