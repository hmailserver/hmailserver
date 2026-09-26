alter table hm_imapfolders add column folderuidvalidity bigint not null default 0;

create table hm_uidvalidity (uidvalidityaccountid bigint not null primary key, uidvalidityvalue bigint not null) DEFAULT CHARSET=utf8;

update hm_dbversion set value = 5713;