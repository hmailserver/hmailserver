ALTER TABLE hm_tcpipports ADD portconnectionsecurity tinyint not null default 0

UPDATE hm_tcpipports SET portconnectionsecurity = portusessl

ALTER TABLE hm_tcpipports DROP COLUMN [portusessl]

update hm_dbversion set value = 5500

