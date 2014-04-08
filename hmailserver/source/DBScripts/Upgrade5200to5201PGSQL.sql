ALTER TABLE hm_routes add column routetreatsenderaslocaldomain smallint not null DEFAULT 0;

update hm_dbversion set value = 5201;
