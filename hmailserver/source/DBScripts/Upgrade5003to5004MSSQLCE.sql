insert into hm_servermessages (smname, smtext) values ('VIRUS_ATTACHMENT_REMOVED', 'Virus found:' + NCHAR(13) + NCHAR(10) + 'The attachment(s) of this message was removed since a virus was detected in at least one of them.' + NCHAR(13) + NCHAR(10) + NCHAR(13) + NCHAR(10)) 

update hm_dbversion set value = 5004

