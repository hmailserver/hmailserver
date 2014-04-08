drop table hm_routes

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

delete from hm_routeaddresses

insert into hm_settings (settingname, settingstring, settinginteger) values ('usesmtprelayerauthentication', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerport', '', 25)

update hm_dbversion set value = 3301


