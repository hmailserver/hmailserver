ALTER TABLE hm_accounts ADD accountenablesignature TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_accounts ADD accountsignatureplaintext TEXT NOT NULL DEFAULT ''

ALTER TABLE hm_accounts ADD accountsignaturehtml TEXT NOT NULL DEFAULT ''

ALTER TABLE hm_domains ADD domainenablesignature TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD domainsignaturemethod TINYINT NOT NULL DEFAULT 1

ALTER TABLE hm_domains ADD domainsignatureplaintext TEXT NOT NULL DEFAULT ''

ALTER TABLE hm_domains ADD domainsignaturehtml TEXT NOT NULL DEFAULT ''

ALTER TABLE hm_domains ADD domainaddsignaturestoreplies TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD domainaddsignaturestolocalemail TINYINT NOT NULL DEFAULT 1

create table hm_greylisting_whiteaddresses
(
	whiteid bigint identity(1,1) not null CONSTRAINT hm_glwhite_pk PRIMARY KEY NONCLUSTERED,
	whiteipaddress varchar(255) not null,
	whiteipdescription varchar(255) not null
	CONSTRAINT u_glwhite UNIQUE NONCLUSTERED (whiteipaddress)
)

create table hm_tcpipports
(
	portid int identity(1,1) not null CONSTRAINT hm_tcpipports_pk PRIMARY KEY NONCLUSTERED, 
	portprotocol tinyint not null,
	portnumber int not null
) 

insert into hm_tcpipports select 1, settinginteger from hm_settings where settingname = 'smtpport'

insert into hm_tcpipports select 3, settinginteger from hm_settings where settingname = 'pop3port'

insert into hm_tcpipports select 5, settinginteger from hm_settings where settingname = 'imapport'

delete from hm_settings where settingname = 'smtpport'

delete from hm_settings where settingname = 'pop3port'

delete from hm_settings where settingname = 'imapport'

update hm_dbversion set value = 4301

