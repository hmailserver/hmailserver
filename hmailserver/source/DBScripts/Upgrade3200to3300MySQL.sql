alter table hm_domains drop requiresmtpauth

alter table hm_accounts drop accountreturnconv

alter table hm_securityranges add column rangename varchar (100) not null

delete from hm_securityranges

insert into hm_securityranges (rangepriorityid, rangelowerip, rangeupperip, rangeoptions, rangename) values (10, 0, 4294967295, 491, 'Internet')

insert into hm_securityranges (rangepriorityid, rangelowerip, rangeupperip, rangeoptions, rangename) values (15, 2130706433, 2130706433, 459, 'My computer')

create table hm_routes 
(
  routeid mediumint(9) NOT NULL auto_increment, primary key(routeid), unique(routeid),
  routedomainname varchar(255) NOT NULL,
  routetargetsmthost varchar(255) NOT NULL,
  routetargetsmtport mediumint(9) NOT NULL,
  routenooftries mediumint(9) NOT NULL,
  routeminutesbetweentry mediumint(9) NOT NULL,
  routealladdresses tinyint(3) unsigned NOT NULL,
  routeuseauthentication mediumint(9) NOT NULL,
  routeauthenticationusername varchar(255) NOT NULL,
  routeauthenticationpassword varchar(255) NOT NULL,
  routetreatsecurityaslocal mediumint(9) NOT NULL  
)

create table hm_routeaddresses
(
  routeaddressid mediumint(9) NOT NULL auto_increment, primary key(routeaddressid), unique(routeaddressid),
  routeaddressrouteid mediumint(9) NOT NULL,
  routeaddressaddress varchar(255) NOT NULL
)

drop table if exists hm_dnscache

delete from hm_settings where settingname = 'useproxy'

delete from hm_settings where settingname = 'proxycacheupdateinterval'

insert into hm_settings (settingname, settingstring, settinginteger) values ('sendstatistics', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('hostname', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerusername', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerpassword', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('usesmtprelayerauthentication', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerport', '', 25)

update hm_dbversion set value = 3300


