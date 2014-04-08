drop table hm_iphomes

ALTER TABLE hm_accounts ADD accountpersonfirstname varchar(60) NOT NULL DEFAULT ''

ALTER TABLE hm_accounts ADD accountpersonlastname varchar(60) NOT NULL DEFAULT ''

ALTER TABLE hm_messagerecipients ADD recipientoriginaladdress varchar(255) not null DEFAULT ''

ALTER TABLE hm_routes ADD routedescription varchar(255) NOT NULL DEFAULT ''

ALTER TABLE hm_routes ADD routeusessl tinyint not null DEFAULT 0

ALTER TABLE hm_dnsbl ADD sblscore int not null DEFAULT 3

ALTER TABLE hm_fetchaccounts ADD fausessl tinyint not null DEFAULT 0

ALTER TABLE hm_rule_actions ADD actionheader varchar(80) not null default ''

ALTER TABLE hm_rule_actions ADD actionvalue varchar(255) not null default ''

ALTER TABLE hm_surblservers ADD surblscore int not null default 3

ALTER TABLE hm_tcpipports ADD portaddress bigint not null default 0

ALTER TABLE hm_tcpipports ADD portusessl tinyint not null default 0

ALTER TABLE hm_tcpipports ADD portsslcertificateid int not null default 0

create table hm_sslcertificates
(
	sslcertificateid bigint identity(1,1) not null CONSTRAINT hm_sslcertificates_pk PRIMARY KEY NONCLUSTERED,
	sslcertificatename nvarchar(255) not null,
	sslcertificatefile nvarchar(255) not null,
	sslprivatekeyfile nvarchar(255) not null
)

create table hm_groups
(
	groupid bigint identity(1,1) not null CONSTRAINT hm_groups_pk PRIMARY KEY NONCLUSTERED,
	groupparentgroupid bigint not null,
	groupdomainid bigint not null,
   groupname nvarchar(255)
)

create table hm_group_members
(
	memberid bigint identity(1,1) not null CONSTRAINT hm_group_members_pk PRIMARY KEY NONCLUSTERED,
	membergroupid bigint not null,
	memberaccountid bigint not null
)

create table hm_acl
(
	aclid bigint identity(1,1) not null CONSTRAINT hm_acl_pk PRIMARY KEY NONCLUSTERED,
   aclshareaccountid bigint not null,
   aclsharefolderid bigint not null,
	aclpermissiontype tinyint not null,
	aclpermissiongroupid bigint not null,
	aclpermissiondomainid bigint not null,
	aclpermissionaccountid bigint not null,
	aclvalue bigint not null,
	unique(aclshareaccountid, aclsharefolderid, aclpermissiontype, aclpermissiongroupid, aclpermissiondomainid, aclpermissionaccountid)
)

delete from hm_settings where settingname = 'listenonalladdresses'

delete from hm_settings where settingname = 'spamaction'

insert into hm_settings (settingname, settingstring, settinginteger) values ('enableimapacl', '', 1)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerusessl', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('adddeliveredtoheader', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('groupcachettl', '', 60)

insert into hm_settings (settingname, settingstring, settinginteger) values ('imappublicfoldername', 'Public folders', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('antispamenabled', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('usespfscore', '', 3)

insert into hm_settings (settingname, settingstring, settinginteger) values ('ascheckhostinheloscore', '', 2)

insert into hm_settings (settingname, settingstring, settinginteger) values ('usemxchecksscore', '', 2)

insert into hm_settings (settingname, settingstring, settinginteger) values ('spammarkthreshold', '', 5)

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamdeletethreshold', '', 20)

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinenabled', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinscore', '', 5)

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinmergescore', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinhost', 'localhost', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('spamassassinport', '', 783)

UPDATE hm_securityranges SET rangeoptions = rangeoptions | 4096

delete from hm_dnsbl where sbldnshost = 'relays.ordb.org'

UPDATE hm_rule_actions SET actionrouteid = 8 WHERE actionrouteid = 7

update hm_dbversion set value = 5000

