alter table hm_accounts add column accountvacationabortspamflagged tinyint not null default 0;

alter table hm_accounts add column accountforwardabortspamflagged tinyint not null default 0;

alter table hm_rule_actions add column actionabortspamflagged tinyint not null default 0;

alter table hm_messages modify column messageflags tinyint unsigned not null;

update hm_dbversion set value = 5708;