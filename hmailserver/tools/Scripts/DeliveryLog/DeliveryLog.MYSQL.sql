create table hm_deliverylog
(
	deliveryid bigint auto_increment not null, primary key(deliveryid), unique(deliveryid),
	deliveryfrom varchar(255) not null,
  	deliveryfilename varchar(255) not null,
   deliverytime datetime not null,
	deliverysubject varchar(255) not null,
	deliverybody text not null
) DEFAULT CHARSET=utf8;

create table hm_deliverylog_recipients
(
	deliveryrecipientid int auto_increment not null, primary key(deliveryrecipientid), unique(deliveryrecipientid),
   deliveryid  int not null,
	deliveryrecipientaddress varchar(255) not null
) DEFAULT CHARSET=utf8;