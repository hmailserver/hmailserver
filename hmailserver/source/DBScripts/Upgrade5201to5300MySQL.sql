ALTER TABLE hm_securityranges CHANGE rangelowerip rangelowerip1 bigint NOT NULL;

ALTER TABLE hm_securityranges ADD COLUMN rangelowerip2 bigint NULL;

ALTER TABLE hm_securityranges CHANGE rangeupperip rangeupperip1 bigint NOT NULL;

ALTER TABLE hm_securityranges ADD COLUMN rangeupperip2 bigint NULL;

@@@HM_DROP_COLUMN_OBJECTS hm_securityranges.glipaddress1@@@;

ALTER TABLE hm_greylisting_triplets CHANGE glipaddress glipaddress1 bigint NOT NULL;

ALTER TABLE hm_greylisting_triplets ADD COLUMN glipaddress2 bigint NULL;

@@@HM_DROP_COLUMN_OBJECTS hm_greylisting_triplets.glipaddres1@@@;

CREATE UNIQUE INDEX idx_hm_greylisting_triplets ON hm_greylisting_triplets (glipaddress1, glipaddress2, glsenderaddress(2), glrecipientaddress(2));

ALTER TABLE hm_tcpipports CHANGE portaddress portaddress1 bigint NOT NULL;

ALTER TABLE hm_tcpipports ADD COLUMN portaddress2 bigint NULL;

ALTER TABLE hm_whitelist CHANGE whiteloweripaddress whiteloweripaddress1 bigint NOT NULL;

ALTER TABLE hm_whitelist ADD COLUMN whiteloweripaddress2 bigint NULL;

ALTER TABLE hm_whitelist CHANGE whiteupperipaddress whiteupperipaddress1 bigint NOT NULL;

ALTER TABLE hm_whitelist ADD COLUMN whiteupperipaddress2 bigint NULL;

ALTER TABLE hm_incoming_relays CHANGE relaylowerip relaylowerip1 bigint NOT NULL;

ALTER TABLE hm_incoming_relays ADD COLUMN relaylowerip2 bigint NULL;

ALTER TABLE hm_incoming_relays CHANGE relayupperip relayupperip1 bigint NOT NULL;

ALTER TABLE hm_incoming_relays ADD COLUMN relayupperip2 bigint NULL;

ALTER TABLE hm_logon_failures CHANGE ipaddress ipaddress1 bigint NOT NULL;

ALTER TABLE hm_logon_failures ADD COLUMN ipaddress2 bigint NULL;

@@@HM_DROP_COLUMN_OBJECTS hm_logon_failures.ipaddress1@@@;

CREATE  INDEX idx_hm_logon_failures ON hm_logon_failures (ipaddress1, ipaddress2);

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnMailFromMX', '', 0);

update hm_rule_actions set actionimapfolder = replace(actionimapfolder, '\\', (select settingstring from hm_settings where settingname = 'IMAPHierarchyDelimiter'));

update hm_rule_actions set actionimapfolder = replace(actionimapfolder, '/', (select settingstring from hm_settings where settingname = 'IMAPHierarchyDelimiter'));

update hm_rule_actions set actionimapfolder = replace(actionimapfolder, '.', (select settingstring from hm_settings where settingname = 'IMAPHierarchyDelimiter'));

ALTER TABLE hm_securityranges CHANGE rangelowerip1 rangelowerip1 bigint not null;

ALTER TABLE hm_securityranges CHANGE rangeupperip1 rangeupperip1 bigint not null;

update hm_dbversion set value = 5300;

