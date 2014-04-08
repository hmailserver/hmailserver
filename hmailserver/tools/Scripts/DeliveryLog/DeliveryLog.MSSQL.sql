create table hm_deliverylog
(
	deliveryid int identity (1, 1) not null ,
	deliveryfrom varchar(255) not null,
   deliveryfilename varchar(255) not null,
   deliverytime datetime not null,
	deliverysubject nvarchar(255) not null,
	deliverybody ntext not null
) 

ALTER TABLE hm_deliverylog ADD CONSTRAINT hm_deliverylog_pk PRIMARY KEY NONCLUSTERED (deliveryid) 

create table hm_deliverylog_recipients
(
	deliveryrecipientid int identity (1, 1) not null,
   deliveryid int not null,
	deliveryrecipientaddress varchar(255) not null
) 

ALTER TABLE hm_deliverylog_recipients ADD CONSTRAINT hm_deliverylog_recipients_pk PRIMARY KEY NONCLUSTERED (deliveryrecipientid) 
