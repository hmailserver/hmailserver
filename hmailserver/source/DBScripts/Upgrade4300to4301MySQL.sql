ALTER TABLE hm_accounts ADD COLUMN accountenablesignature TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_accounts ADD COLUMN accountsignatureplaintext TEXT NOT NULL

ALTER TABLE hm_accounts ADD COLUMN accountsignaturehtml TEXT NOT NULL

ALTER TABLE hm_domains ADD COLUMN domainenablesignature TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD COLUMN domainsignaturemethod TINYINT NOT NULL DEFAULT 1

ALTER TABLE hm_domains ADD COLUMN domainsignatureplaintext TEXT NOT NULL

ALTER TABLE hm_domains ADD COLUMN domainsignaturehtml TEXT NOT NULL

ALTER TABLE hm_domains ADD COLUMN domainaddsignaturestoreplies TINYINT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD COLUMN domainaddsignaturestolocalemail TINYINT NOT NULL DEFAULT 1

create table hm_greylisting_whiteaddresses
(
	whiteid bigint auto_increment not null, primary key(whiteid), unique(whiteid),
	whiteipaddress varchar(255) not null,
	whiteipdescription varchar(255) not null
)

create table hm_tcpipports
(
	portid bigint auto_increment not null, primary key(portid), unique(portid),
	portprotocol tinyint not null,
	portnumber int not null
)

insert into hm_tcpipports select null, 1, settinginteger from hm_settings where settingname = 'smtpport'

insert into hm_tcpipports select null, 3, settinginteger from hm_settings where settingname = 'pop3port'

insert into hm_tcpipports select null, 5, settinginteger from hm_settings where settingname = 'imapport'

delete from hm_settings where settingname = 'smtpport'

delete from hm_settings where settingname = 'pop3port'

delete from hm_settings where settingname = 'imapport'

update hm_dbversion set value = 4301

