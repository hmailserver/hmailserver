create table hm_dnsbl
(
	sblid int auto_increment not null, primary key(sblid), unique(sblid),
	sblactive integer not null,
	sbldnshost varchar(255) not null,
	sblresult varchar(255) not null,
	sblrejectmessage varchar(255) not null
)

insert into hm_dnsbl (sblactive, sbldnshost, sblresult, sblrejectmessage) values (0, 'sbl-xbl.spamhaus.org', '127.0.0.*',  'Rejected by Spamhaus')

insert into hm_dnsbl (sblactive, sbldnshost, sblresult, sblrejectmessage) values (0, 'relays.ordb.org', '127.0.0.*', 'Rejected by ORDB')

insert into hm_dnsbl (sblactive, sbldnshost, sblresult, sblrejectmessage) values (0, 'bl.spamcop.net', '127.0.0.*', 'Rejected by SpamCop')

insert into hm_settings (settingname, settingstring, settinginteger) values ('usespf', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('usemxchecks', '', 0)

delete from hm_settings where settingname = 'useordb'

delete from hm_settings where settingname = 'usespamhaus'

update hm_dbversion set value = 3401

