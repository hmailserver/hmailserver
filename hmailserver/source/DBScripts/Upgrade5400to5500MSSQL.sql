sp_rename 'hm_tcpipports.portusessl', 'portconnectionsecurity', 'COLUMN'

sp_rename 'hm_routes.routeusessl', 'routeconnectionsecurity', 'COLUMN'

sp_rename 'hm_fetchaccounts.fausessl', 'faconnectionsecurity', 'COLUMN'

UPDATE hm_settings SET settingname = 'smtprelayerconnectionsecurity' WHERE settingname = 'smtprelayerusessl'

update hm_dbversion set value = 5500

