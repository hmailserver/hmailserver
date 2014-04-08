drop table if exists hm_groups;

drop table if exists hm_group_members;

drop table if exists hm_acl;

create table hm_groups
(
	groupid bigint auto_increment not null, primary key(`groupid`), unique(`groupid`),
    groupname varchar(255)
) DEFAULT CHARSET=utf8;

create table hm_group_members
(
	memberid bigint auto_increment not null, primary key(`memberid`), unique(`memberid`),
	membergroupid bigint not null,
	memberaccountid bigint not null
) DEFAULT CHARSET=utf8;

create table hm_acl
(
	aclid bigint auto_increment not null, primary key(`aclid`), unique(`aclid`),
    aclsharefolderid bigint not null,
	aclpermissiontype tinyint not null,
	aclpermissiongroupid bigint not null,
	aclpermissionaccountid bigint not null,
	aclvalue bigint not null,
	unique(aclsharefolderid, aclpermissiontype, aclpermissiongroupid, aclpermissionaccountid)
) DEFAULT CHARSET=utf8;

alter table hm_messages drop messagerecipients;

ALTER TABLE hm_messages DROP COLUMN messagerecipientsparsed;

drop table if exists hm_adsynchronization;

ALTER TABLE hm_messagerecipients DROP COLUMN recipientislocal;

ALTER TABLE hm_messagerecipients DROP COLUMN recipientisenabled;

ALTER TABLE hm_messagerecipients DROP COLUMN recipientisexisting;

ALTER TABLE hm_aliases DROP COLUMN aliastype;

update hm_dbversion set value = 5005;

