ALTER TABLE hm_messages ADD  messagecreatetime datetime NOT NULL DEFAULT GETDATE()

update hm_dbversion set value = 3001

