@@@HM_DROP_COLUMN_OBJECTS hm_securityranges.rangelowerip@@@

ALTER TABLE hm_securityranges ADD rangelowerip1 bigint NOT NULL DEFAULT 0

UPDATE hm_securityranges SET rangelowerip1 = rangelowerip

ALTER TABLE hm_securityranges DROP COLUMN rangelowerip

ALTER TABLE hm_securityranges ADD rangelowerip2 bigint NULL

@@@HM_DROP_COLUMN_OBJECTS hm_securityranges.rangeupperip@@@

ALTER TABLE hm_securityranges ADD rangeupperip1 bigint NOT NULL DEFAULT 0

UPDATE hm_securityranges SET rangeupperip1 = rangeupperip

ALTER TABLE hm_securityranges DROP COLUMN rangeupperip

ALTER TABLE hm_securityranges ADD rangeupperip2 bigint NULL

@@@HM_DROP_COLUMN_OBJECTS hm_securityranges.glipaddress1@@@

@@@HM_DROP_COLUMN_OBJECTS hm_greylisting_triplets.glipaddress@@@

ALTER TABLE hm_greylisting_triplets ADD glipaddress1 bigint NOT NULL DEFAULT 0

UPDATE hm_greylisting_triplets SET glipaddress1 = glipaddress

ALTER TABLE hm_greylisting_triplets DROP COLUMN glipaddress

ALTER TABLE hm_greylisting_triplets ADD glipaddress2 bigint NULL

@@@HM_DROP_COLUMN_OBJECTS hm_greylisting_triplets.glipaddres1@@@

CREATE UNIQUE NONCLUSTERED INDEX idx_hm_greylisting_triplets ON hm_greylisting_triplets (glipaddress1, glipaddress2, glsenderaddress, glrecipientaddress)

@@@HM_DROP_COLUMN_OBJECTS hm_tcpipports.portaddress@@@

ALTER TABLE hm_tcpipports ADD portaddress1 bigint NOT NULL DEFAULT 0

UPDATE hm_tcpipports SET portaddress1 = portaddress

ALTER TABLE hm_tcpipports DROP COLUMN portaddress

ALTER TABLE hm_tcpipports ADD portaddress2 bigint NULL

@@@HM_DROP_COLUMN_OBJECTS hm_whitelist.whiteloweripaddress@@@

ALTER TABLE hm_whitelist ADD whiteloweripaddress1 bigint NOT NULL DEFAULT 0

UPDATE hm_whitelist SET whiteloweripaddress1 = whiteloweripaddress

ALTER TABLE hm_whitelist DROP COLUMN whiteloweripaddress

ALTER TABLE hm_whitelist ADD whiteloweripaddress2 bigint NULL

@@@HM_DROP_COLUMN_OBJECTS hm_whitelist.whiteupperipaddress@@@

ALTER TABLE hm_whitelist ADD whiteupperipaddress1 bigint NOT NULL DEFAULT 0

UPDATE hm_whitelist SET whiteupperipaddress1 = whiteupperipaddress

ALTER TABLE hm_whitelist DROP COLUMN whiteupperipaddress

ALTER TABLE hm_whitelist ADD whiteupperipaddress2 bigint NULL

@@@HM_DROP_COLUMN_OBJECTS hm_incoming_relays.relaylowerip@@@

ALTER TABLE hm_incoming_relays ADD relaylowerip1 bigint NOT NULL DEFAULT 0

UPDATE hm_incoming_relays SET relaylowerip1 = relaylowerip

ALTER TABLE hm_incoming_relays DROP COLUMN relaylowerip

ALTER TABLE hm_incoming_relays ADD relaylowerip2 bigint NULL

@@@HM_DROP_COLUMN_OBJECTS hm_incoming_relays.relayupperip@@@

ALTER TABLE hm_incoming_relays ADD relayupperip1 bigint NOT NULL DEFAULT 0

UPDATE hm_incoming_relays SET relayupperip1 = relayupperip

ALTER TABLE hm_incoming_relays DROP COLUMN relayupperip

ALTER TABLE hm_incoming_relays ADD relayupperip2 bigint NULL

@@@HM_DROP_COLUMN_OBJECTS hm_logon_failures.ipaddress@@@

ALTER TABLE hm_logon_failures ADD ipaddress1 bigint NOT NULL DEFAULT 0

UPDATE hm_logon_failures SET ipaddress1 = ipaddress

ALTER TABLE hm_logon_failures DROP COLUMN ipaddress

ALTER TABLE hm_logon_failures ADD ipaddress2 bigint NULL

@@@HM_DROP_COLUMN_OBJECTS hm_logon_failures.ipaddress1@@@

CREATE  NONCLUSTERED INDEX idx_hm_logon_failures ON hm_logon_failures (ipaddress1, ipaddress2)

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnMailFromMX', '', 0)

@@@SQLCE_UPDATE_IMAP_HIERARCHY_DELIMITER@@@

update hm_dbversion set value = 5300

