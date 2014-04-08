ALTER TABLE hm_accounts ADD COLUMN accountvacationmessageon integer not null;

ALTER TABLE hm_accounts ADD COLUMN accountvacationmessage text not null;

ALTER TABLE hm_accounts ADD COLUMN accountvacationsubject varchar (200) not null;

update hm_dbversion set value = 3100;

