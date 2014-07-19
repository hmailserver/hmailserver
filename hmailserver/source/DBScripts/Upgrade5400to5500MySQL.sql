ALTER TABLE hm_tcpipports CHANGE portusessl portconnectionsecurity tinyint not null

update hm_dbversion set value = 5500;

