create table hm_deliverylog
(
	deliveryid bigserial not null primary key,
	deliveryfrom varchar(255) not null,
  	deliveryfilename varchar(255) not null,
   deliverytime timestamp not null,
	deliverysubject varchar(255) not null,
	deliverybody text not null
);

create table hm_deliverylog_recipients
(
	deliveryrecipientid bigserial not null primary key,
   deliveryid  int not null,
	deliveryrecipientaddress varchar(255) not null
);