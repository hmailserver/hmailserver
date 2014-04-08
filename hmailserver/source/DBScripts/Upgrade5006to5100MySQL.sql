insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDKIMVerificationEnabled', '', 0);

insert into hm_settings (settingname, settingstring, settinginteger) values ('ASDKIMVerificationFailureScore', '', 5);

delete from hm_settings where settingname = 'maskpasswordsinlog';

ALTER TABLE hm_domains ADD COLUMN domaindkimselector varchar(255) NOT NULL DEFAULT '';

ALTER TABLE hm_domains ADD COLUMN domaindkimprivatekeyfile varchar(255) NOT NULL DEFAULT '';

update hm_domains set domainantispamoptions = 1 where domainantispamoptions = 2047;

create table hm_incoming_relays
(
   relayid int auto_increment not null, primary key(`relayid`), unique(`relayid`),
   relayname varchar(100) not null,
   relaylowerip bigint not null,
   relayupperip bigint not null
) DEFAULT CHARSET=utf8; 

insert into hm_incoming_relays
(relayname, relaylowerip, relayupperip)
select rangename, rangelowerip, rangeupperip from hm_securityranges where rangeoptions & 2048 = 2048;

update hm_securityranges set rangeoptions = rangeoptions | 66048  where (rangeoptions & 32) <> 32;

update hm_securityranges set rangeoptions = rangeoptions | 90624  where rangeoptions & 32 = 32;

update hm_securityranges set rangeoptions = rangeoptions & ~2048;

update hm_securityranges set rangeoptions = rangeoptions & ~16;

update hm_securityranges set rangeoptions = rangeoptions & ~32;

ALTER TABLE hm_messages CHANGE COLUMN messageflagseen messageflags tinyint NOT NULL DEFAULT 0;

update hm_messages set messageflags = messageflags | 2 where messageflagdeleted = 1;

update hm_messages set messageflags = messageflags | 4 where messageflagflagged = 1;

update hm_messages set messageflags = messageflags | 8 where messageflaganswered = 1;

update hm_messages set messageflags = messageflags | 16 where messageflagdraft = 1;

update hm_messages set messageflags = messageflags | 32 where messageflagrecent = 1;

alter table hm_messages drop column messageflagdeleted;

alter table hm_messages drop column messageflagflagged;

alter table hm_messages drop column messageflaganswered;

alter table hm_messages drop column messageflagdraft;

alter table hm_messages drop column messageflagrecent;

alter table hm_fetchaccounts ADD COLUMN fauseantispam tinyint not null default 0;

alter table hm_fetchaccounts ADD COLUMN fauseantivirus tinyint not null default 0;

update hm_fetchaccounts set fauseantispam = 1;

update hm_fetchaccounts set fauseantivirus = 1;

ALTER TABLE hm_securityranges ADD COLUMN rangeexpires tinyint not null default 0;

ALTER TABLE hm_securityranges ADD COLUMN rangeexpirestime datetime not null default '2001-01-01';

create table hm_logon_failures
(
   ipaddress bigint not null,
   failuretime datetime not null
) DEFAULT CHARSET=utf8;

CREATE INDEX idx_hm_logon_failures_ipaddress ON hm_logon_failures (ipaddress);

CREATE INDEX idx_hm_logon_failures_failuretime ON hm_logon_failures (failuretime);

insert into hm_settings (settingname, settingstring, settinginteger) values ('AutoBanOnLogonFailureEnabled', '', 1);

insert into hm_settings (settingname, settingstring, settinginteger) values ('MaxInvalidLogonAttempts', '', 3);

insert into hm_settings (settingname, settingstring, settinginteger) values ('LogonAttemptsWithinMinutes', '', 30);

insert into hm_settings (settingname, settingstring, settinginteger) values ('AutoBanMinutes', '', 60);

update hm_securityranges set rangeoptions = rangeoptions & ~16;

insert into hm_settings (settingname, settingstring, settinginteger) values ('IMAPHierarchyDelimiter', '.', 0);

delete from hm_settings where settingname = 'usedeliverylog';

update hm_dbversion set value = 5100;

