drop table hm_routes

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

delete from hm_routeaddresses

insert into hm_settings (settingname, settingstring, settinginteger) values ('usesmtprelayerauthentication', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('smtprelayerport', '', 25)

update hm_dbversion set value = 3301


