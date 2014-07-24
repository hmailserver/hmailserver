ALTER TABLE hm_tcpipports RENAME COLUMN portusessl TO portconnectionsecurity;

ALTER TABLE hm_routes RENAME COLUMN routeusessl TO routeconnectionsecurity;

ALTER TABLE hm_fetchaccounts RENAME COLUMN fausessl TO faconnectionsecurity;

UPDATE hm_settings SET settingname = 'smtprelayerconnectionsecurity' WHERE settingname = 'smtprelayerusessl';

update hm_dbversion set value = 5500;

