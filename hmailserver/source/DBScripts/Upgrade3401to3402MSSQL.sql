drop table hm_messagelocks

alter table hm_messages add messagelocked tinyint not null default 0

update hm_dbversion set value = 3402