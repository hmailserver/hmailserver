create table hm_dbversion (
	value int not null
)

insert into hm_dbversion (value) values (1100)

alter table hm_messages drop column messagetext

