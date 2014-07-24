ALTER TABLE hm_tcpipports CHANGE portusessl portconnectionsecurity tinyint not null;

ALTER TABLE hm_routes CHANGE routeusessl routeconnectionsecurity tinyint not null;

ALTER TABLE hm_fetchaccounts CHANGE fausessl faconnectionsecurity tinyint not null;

UPDATE hm_settings SET settingname = 'smtprelayerconnectionsecurity' WHERE settingname = 'smtprelayerusessl';

update hm_dbversion set value = 5500;

