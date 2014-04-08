create table hm_distributionlists 
(
	distributionlistid int identity (1, 1) not null,
	distributionlistdomainid int not null,
	distributionlistaddress varchar(255) not null,
	distributionlistenabled int not null,	
    	distributionlistrequireauth int not null,
	distributionlistrequireaddress varchar(255) not null

)

create table hm_distributionlistsrecipients
(
	distributionlistrecipientid int identity (1, 1) not null,
	distributionlistrecipientlistid int not null,
	distributionlistrecipientaddress varchar(255)	
)

create table hm_messagerecipients
(
	recipientid int identity (1, 1) not null,
    	recipientmessageid int not null,
	recipientaddress varchar(255) not null,
	recipientislocal int not null,
	recipientisenabled int not null,
	recipientisexisting int not null,
	recipientlocalaccountid int not null
)

alter table hm_messages add messagerecipientsparsed integer not null DEFAULT 0


update hm_dbversion set value = 1500

