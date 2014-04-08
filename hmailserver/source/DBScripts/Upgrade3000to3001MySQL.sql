ALTER TABLE hm_messages ADD COLUMN messagecreatetime datetime NOT NULL DEFAULT '1901-01-01 00:00:00'

update hm_dbversion set value = 3001

