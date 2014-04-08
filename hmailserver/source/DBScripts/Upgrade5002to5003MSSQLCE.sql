drop table hm_acl

create table hm_acl
(
	aclid bigint identity(1,1) not null,
   aclsharefolderid bigint not null,
	aclpermissiontype tinyint not null,
	aclpermissiongroupid bigint not null,
	aclpermissiondomainid bigint not null,
	aclpermissionaccountid bigint not null,
	aclvalue bigint not null
) 

ALTER TABLE hm_acl ADD CONSTRAINT hm_acl_pk PRIMARY KEY NONCLUSTERED (aclid) 

ALTER TABLE hm_acl ADD CONSTRAINT hm_acl_unique UNIQUE (aclsharefolderid, aclpermissiontype, aclpermissiongroupid, aclpermissiondomainid, aclpermissionaccountid) 

update hm_settings set settingstring = '#Public' where settingname = 'imappublicfoldername'

update hm_dbversion set value = 5003
