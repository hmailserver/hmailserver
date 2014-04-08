CREATE  NONCLUSTERED INDEX idx_message_metadata_id ON hm_message_metadata (metadata_messageid)

update hm_dbversion set value = 5310

