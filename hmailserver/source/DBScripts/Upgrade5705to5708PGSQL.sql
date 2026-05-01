alter table hm_accounts add column accountvacationabortspamflagged smallint not null default 0;

alter table hm_accounts add column accountforwardabortspamflagged smallint not null default 0;

alter table hm_rule_actions add column actionabortspamflagged smallint not null default 0;

update hm_dbversion set value = 5708;