ALTER TABLE hm_tcpipports ADD portconnectionsecurity tinyint not null default 0

UPDATE hm_tcpipports SET portconnectionsecurity = portusessl

ALTER TABLE hm_tcpipports DROP COLUMN [portusessl]

ALTER TABLE hm_routes ADD routeconnectionsecurity tinyint not null default 0

UPDATE hm_routes SET routeconnectionsecurity = routeusessl

ALTER TABLE hm_routes DROP COLUMN routeusessl


ALTER TABLE hm_fetchaccounts ADD faconnectionsecurity tinyint not null default 0

UPDATE hm_fetchaccounts SET faconnectionsecurity = fausessl

ALTER TABLE hm_fetchaccounts DROP COLUMN fausessl


UPDATE hm_settings SET settingname = 'smtprelayerconnectionsecurity' WHERE settingname = 'smtprelayerusessl'

update hm_dbversion set value = 5500

