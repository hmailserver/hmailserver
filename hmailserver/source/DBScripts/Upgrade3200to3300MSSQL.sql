alter table hm_domains drop column requiresmtpauth

alter table hm_accounts drop column accountreturnconv

alter table hm_securityranges add  rangename varchar (100) not null default ''

delete from hm_securityranges

insert into hm_securityranges (rangepriorityid, rangelowerip, rangeupperip, rangeoptions, rangename) values (10, 0, 4294967295, 491, 'Internet')

insert into hm_securityranges (rangepriorityid, rangelowerip, rangeupperip, rangeoptions, rangename) values (15, 2130706433, 2130706433, 459, 'My computer')

create table hm_routes 
(
  routeid numeric identity (1, 1) not null,
  routedomainname varchar(255) NOT NULL,
  routetargetsmthost varchar(255) NOT NULL,
  routetargetsmtport numeric NOT NULL,
  routenooftries numeric NOT NULL,
  routeminutesbetweentry numeric NOT NULL,
  routealladdresses numeric NOT NULL,
  routeuseauthentication numeric NOT NULL,
  routeauthenticationusername varchar(255) NOT NULL,
  routeauthenticationpassword varchar(255) NOT NULL,
  routetreatsecurityaslocal numeric NOT NULL
)

create table hm_routeaddresses
(
  routeaddressid numeric identity (1, 1) not null,
  routeaddressrouteid numeric NOT NULL,
  routeaddressaddress varchar(255) NOT NULL
)

if exists (select * from sysobjects where id = object_id('hm_dnscache') and objectproperty(id, 'isusertable') = 1)
drop table hm_dnscache

delete from hm_settings where settingname = 'useproxy'

delete from hm_settings where settingname = 'proxycacheupdateinterval'

insert into hm_settings (settingname, settingstring, settinginteger) values ('sendstatistics', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('hostname', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerusername', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerpassword', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('usesmtprelayerauthentication', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerport', '', 25)

update hm_dbversion set value = 3300


