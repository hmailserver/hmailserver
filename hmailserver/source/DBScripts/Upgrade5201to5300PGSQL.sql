ALTER TABLE hm_securityranges RENAME COLUMN rangelowerip TO rangelowerip1;

ALTER TABLE hm_securityranges ADD COLUMN rangelowerip2 bigint NULL;

ALTER TABLE hm_securityranges RENAME COLUMN rangeupperip TO rangeupperip1;

ALTER TABLE hm_securityranges ADD COLUMN rangeupperip2 bigint NULL;

SELECT hm_drop_column_objects('hm_securityranges', 'glipaddress1');

ALTER TABLE hm_greylisting_triplets RENAME COLUMN glipaddress TO glipaddress1;

ALTER TABLE hm_greylisting_triplets ADD COLUMN glipaddress2 bigint NULL;

SELECT hm_drop_column_objects('hm_greylisting_triplets', 'glipaddres1');

CREATE UNIQUE INDEX idx_hm_greylisting_triplets ON hm_greylisting_triplets (glipaddress1, glipaddress2, glsenderaddress, glrecipientaddress);

ALTER TABLE hm_tcpipports RENAME COLUMN portaddress TO portaddress1;

ALTER TABLE hm_tcpipports ADD COLUMN portaddress2 bigint NULL;

ALTER TABLE hm_whitelist RENAME COLUMN whiteloweripaddress TO whiteloweripaddress1;

ALTER TABLE hm_whitelist ADD COLUMN whiteloweripaddress2 bigint NULL;

ALTER TABLE hm_whitelist RENAME COLUMN whiteupperipaddress TO whiteupperipaddress1;

ALTER TABLE hm_whitelist ADD COLUMN whiteupperipaddress2 bigint NULL;

ALTER TABLE hm_incoming_relays RENAME COLUMN relaylowerip TO relaylowerip1;

ALTER TABLE hm_incoming_relays ADD COLUMN relaylowerip2 bigint NULL;

ALTER TABLE hm_incoming_relays RENAME COLUMN relayupperip TO relayupperip1;

ALTER TABLE hm_incoming_relays ADD COLUMN relayupperip2 bigint NULL;

ALTER TABLE hm_logon_failures RENAME COLUMN ipaddress TO ipaddress1;

ALTER TABLE hm_logon_failures ADD COLUMN ipaddress2 bigint NULL;

SELECT hm_drop_column_objects('hm_logon_failures', 'ipaddress1');

CREATE  INDEX idx_hm_logon_failures ON hm_logon_failures (ipaddress1, ipaddress2);

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnMailFromMX', '', 0);

update hm_rule_actions set actionimapfolder = replace(actionimapfolder, E'\\', (select settingstring from hm_settings where settingname = 'IMAPHierarchyDelimiter'));
update hm_rule_actions set actionimapfolder = replace(actionimapfolder, '/', (select settingstring from hm_settings where settingname = 'IMAPHierarchyDelimiter'));
update hm_rule_actions set actionimapfolder = replace(actionimapfolder, '.', (select settingstring from hm_settings where settingname = 'IMAPHierarchyDelimiter'));

update hm_dbversion set value = 5300;

