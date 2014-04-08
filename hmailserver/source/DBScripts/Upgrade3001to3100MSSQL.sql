ALTER TABLE hm_accounts ADD  accountvacationmessageon integer NOT NULL DEFAULT 0

ALTER TABLE hm_accounts ADD  accountvacationmessage varchar (1000) not null default ''

ALTER TABLE hm_accounts ADD  accountvacationsubject varchar (200) not null default ''

update hm_dbversion set value = 3100

