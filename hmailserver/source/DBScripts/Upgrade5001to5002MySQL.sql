ALTER TABLE hm_rule_actions ADD COLUMN actionrouteid int not null DEFAULT 0 /* [IGNORE-ERRORS] */

update hm_dbversion set value = 5002

