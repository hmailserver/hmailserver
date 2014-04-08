drop table hm_iphomes;

ALTER TABLE hm_accounts ADD COLUMN accountpersonfirstname varchar(60) not null

ALTER TABLE hm_accounts ADD COLUMN accountpersonlastname varchar(60) not null

ALTER TABLE hm_messagerecipients ADD COLUMN recipientoriginaladdress varchar(255) not null

ALTER TABLE hm_routes ADD COLUMN routedescription varchar(255) NOT NULL

ALTER TABLE hm_routes ADD COLUMN routeusessl tinyint not null

ALTER TABLE hm_dnsbl ADD COLUMN sblscore int not null default 3

ALTER TABLE hm_fetchaccounts ADD COLUMN fausessl tinyint not null

ALTER TABLE hm_rule_actions ADD COLUMN actionheader varchar(80) not null

ALTER TABLE hm_rule_actions ADD COLUMN actionvalue varchar(255) not null

ALTER TABLE hm_surblservers ADD COLUMN surblscore int not null default 3

ALTER TABLE hm_tcpipports ADD COLUMN portaddress bigint not null

ALTER TABLE hm_tcpipports ADD COLUMN portusessl tinyint not null

ALTER TABLE hm_tcpipports ADD COLUMN portsslcertificateid int not null

create table hm_sslcertificates
(
	sslcertificateid bigint auto_increment not null, primary key(sslcertificateid), unique(sslcertificateid),
	sslcertificatename varchar(255) not null,
	sslcertificatefile varchar(255) not null,
	sslprivatekeyfile varchar(255) not null
)

create table hm_groups
(
	groupid bigint auto_increment not null, primary key(`groupid`), unique(`groupid`),
	groupparentgroupid bigint not null,
	groupdomainid bigint not null,
   groupname varchar(255)
)

create table hm_group_members
(
	memberid bigint auto_increment not null, primary key(`memberid`), unique(`memberid`),
	membergroupid bigint not null,
	memberaccountid bigint not null
)

create table hm_acl
(
	aclid bigint auto_increment not null, primary key(`aclid`), unique(`aclid`),
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

