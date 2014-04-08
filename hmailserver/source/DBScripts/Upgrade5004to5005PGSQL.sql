select hm_drop_table('hm_groups');

select hm_drop_table('hm_group_members');

select hm_drop_table('hm_acl');

create table hm_groups
(
	groupid bigserial not null primary key,
    groupname varchar(255)
);

create table hm_group_members
(
	memberid bigserial not null primary key,
	membergroupid bigint not null,
	memberaccountid bigint not null
);

create table hm_acl
(
	aclid bigserial not null primary key,
    aclsharefolderid bigint not null,
	aclpermissiontype smallint not null,
	aclpermissiongroupid bigint not null,
	aclpermissionaccountid bigint not null,
	aclvalue bigint not null,
	unique(aclsharefolderid, aclpermissiontype, aclpermissiongroupid,  aclpermissionaccountid)
);

alter table hm_messages drop column messagerecipients;

alter table hm_messages drop column messagerecipientsparsed;

select hm_drop_table('hm_adsynchronization');

ALTER TABLE hm_messagerecipients DROP COLUMN recipientislocal;

ALTER TABLE hm_messagerecipients DROP COLUMN recipientisenabled;

ALTER TABLE hm_messagerecipients DROP COLUMN recipientisexisting;

ALTER TABLE hm_aliases DROP COLUMN aliastype;

update hm_dbversion set value = 5005;

