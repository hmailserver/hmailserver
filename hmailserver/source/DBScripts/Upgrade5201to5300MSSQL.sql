sp_rename 'hm_securityranges.rangelowerip', 'rangelowerip1', 'COLUMN'

ALTER TABLE hm_securityranges ADD rangelowerip2 bigint NULL

sp_rename 'hm_securityranges.rangeupperip', 'rangeupperip1', 'COLUMN'

ALTER TABLE hm_securityranges ADD rangeupperip2 bigint NULL

hm_drop_column_objects 'hm_securityranges', 'glipaddress1'

sp_rename 'hm_greylisting_triplets.glipaddress', 'glipaddress1', 'COLUMN'

ALTER TABLE hm_greylisting_triplets ADD glipaddress2 bigint NULL

hm_drop_column_objects 'hm_greylisting_triplets', 'glipaddres1'

CREATE UNIQUE NONCLUSTERED INDEX idx_hm_greylisting_triplets ON hm_greylisting_triplets (glipaddress1, glipaddress2, glsenderaddress, glrecipientaddress)

sp_rename 'hm_tcpipports.portaddress', 'portaddress1', 'COLUMN'

ALTER TABLE hm_tcpipports ADD portaddress2 bigint NULL

sp_rename 'hm_whitelist.whiteloweripaddress', 'whiteloweripaddress1', 'COLUMN'

ALTER TABLE hm_whitelist ADD whiteloweripaddress2 bigint NULL

sp_rename 'hm_whitelist.whiteupperipaddress', 'whiteupperipaddress1', 'COLUMN'

ALTER TABLE hm_whitelist ADD whiteupperipaddress2 bigint NULL

sp_rename 'hm_incoming_relays.relaylowerip', 'relaylowerip1', 'COLUMN'

ALTER TABLE hm_incoming_relays ADD relaylowerip2 bigint NULL

sp_rename 'hm_incoming_relays.relayupperip', 'relayupperip1', 'COLUMN'

ALTER TABLE hm_incoming_relays ADD relayupperip2 bigint NULL

sp_rename 'hm_logon_failures.ipaddress', 'ipaddress1', 'COLUMN'

ALTER TABLE hm_logon_failures ADD ipaddress2 bigint NULL

hm_drop_column_objects 'hm_logon_failures', 'ipaddress1'

CREATE  NONCLUSTERED INDEX idx_hm_logon_failures ON hm_logon_failures (ipaddress1, ipaddress2)

insert into hm_settings (settingname, settingstring, settinginteger) values ('BypassGreylistingOnMailFromMX', '', 0)


declare @delimiter varchar(10)
select @delimiter = settingstring from hm_settings where settingname = 'IMAPHierarchyDelimiter'
update hm_rule_actions set actionimapfolder = REPLACE(actionimapfolder, '\', @delimiter)
update hm_rule_actions set actionimapfolder = REPLACE(actionimapfolder, '/', @delimiter)
update hm_rule_actions set actionimapfolder = REPLACE(actionimapfolder, '.', @delimiter)
   

update hm_dbversion set value = 5300

