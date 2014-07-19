ALTER TABLE hm_tcpipports RENAME COLUMN portusessl TO portconnectionsecurity;

update hm_dbversion set value = 5500;

