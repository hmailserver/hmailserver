drop table hm_groups

drop table hm_group_members

drop table hm_acl

create table hm_groups
(
	groupid bigint identity(1,1) not null,
    groupname nvarchar(255)
) 

ALTER TABLE hm_groups ADD CONSTRAINT hm_groups_pk PRIMARY KEY NONCLUSTERED (groupid) 

create table hm_group_members
(
	memberid bigint identity(1,1) not null,
	membergroupid bigint not null,
	memberaccountid bigint not null
) 

ALTER TABLE hm_group_members ADD CONSTRAINT hm_group_members_pk PRIMARY KEY NONCLUSTERED (memberid) 

create table hm_acl
(
	aclid bigint identity(1,1) not null,
    aclsharefolderid bigint not null,
	aclpermissiontype tinyint not null,
	aclpermissiongroupid bigint not null,
	aclpermissionaccountid bigint not null,
	aclvalue bigint not null
)

ALTER TABLE hm_acl ADD CONSTRAINT hm_acl_pk PRIMARY KEY NONCLUSTERED (aclid) 

ALTER TABLE hm_acl ADD CONSTRAINT hm_acl_unique UNIQUE (aclsharefolderid, aclpermissiontype, aclpermissiongroupid,aclpermissionaccountid) 

ALTER TABLE hm_messages DROP COLUMN messagerecipients

ALTER TABLE hm_messages DROP COLUMN messagerecipientsparsed

drop table hm_adsynchronization

ALTER TABLE hm_messagerecipients DROP COLUMN recipientislocal

ALTER TABLE hm_messagerecipients DROP COLUMN recipientisenabled 

ALTER TABLE hm_messagerecipients DROP COLUMN recipientisexisting 

ALTER TABLE hm_aliases DROP COLUMN aliastype

update hm_dbversion set value = 5005
