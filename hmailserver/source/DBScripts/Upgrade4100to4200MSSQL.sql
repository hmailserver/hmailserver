insert into hm_settings (settingname, settingstring, settinginteger) values ('accountcachettl', '', 60)

insert into hm_settings (settingname, settingstring, settinginteger) values ('awstatsenabled', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('rulelooplimit', '', 5)

insert into hm_settings (settingname, settingstring, settinginteger) values ('backupoptions', '', 0)

insert into hm_settings (settingname, settingstring, settinginteger) values ('backupdestination', '', 0)

ALTER TABLE hm_messages ALTER COLUMN messagefrom VARCHAR(500) NOT NULL

ALTER TABLE hm_messages ALTER COLUMN messagesize bigint NOT NULL

update hm_securityranges set rangeoptions = rangeoptions | 1024

CREATE INDEX idx_hm_messages_type ON hm_messages (messagetype)

ALTER TABLE hm_accounts ADD CONSTRAINT hm_accounts_pk PRIMARY KEY NONCLUSTERED (accountid)

ALTER TABLE hm_aliases ADD CONSTRAINT hm_aliases_pk PRIMARY KEY NONCLUSTERED (aliasid)

ALTER TABLE hm_domains ADD CONSTRAINT hm_domains_pk PRIMARY KEY NONCLUSTERED (domainid)

ALTER TABLE hm_domain_aliases ADD CONSTRAINT hm_domain_aliases_pk PRIMARY KEY NONCLUSTERED (daid)

ALTER TABLE hm_messages ADD CONSTRAINT hm_messages_pk PRIMARY KEY NONCLUSTERED (messageid)

ALTER TABLE hm_settings ADD CONSTRAINT hm_settings_pk PRIMARY KEY NONCLUSTERED (settingid)

ALTER TABLE hm_serverlog ADD CONSTRAINT hm_serverlog_pk PRIMARY KEY NONCLUSTERED (logid)

ALTER TABLE hm_distributionlists ADD CONSTRAINT hm_distributionlists_pk PRIMARY KEY NONCLUSTERED (distributionlistid)

ALTER TABLE hm_distributionlistsrecipients ADD CONSTRAINT hm_distributionlistsrecipients_pk PRIMARY KEY NONCLUSTERED (distributionlistrecipientid)

ALTER TABLE hm_messagerecipients ADD CONSTRAINT hm_messagerecipients_pk PRIMARY KEY NONCLUSTERED (recipientid)

ALTER TABLE hm_imapfolders ADD CONSTRAINT hm_imapfolders_pk PRIMARY KEY NONCLUSTERED (folderid)

ALTER TABLE hm_securityranges ADD CONSTRAINT hm_securityranges_pk PRIMARY KEY NONCLUSTERED (rangeid)

ALTER TABLE hm_routes ADD CONSTRAINT hm_routes_pk PRIMARY KEY NONCLUSTERED (routeid)

ALTER TABLE hm_routeaddresses ADD CONSTRAINT hm_routeaddresses_pk PRIMARY KEY NONCLUSTERED (routeaddressid)

ALTER TABLE hm_deliverylog ADD CONSTRAINT hm_deliverylog_pk PRIMARY KEY NONCLUSTERED (deliveryid)

ALTER TABLE hm_deliverylog_recipients ADD CONSTRAINT hm_deliverylog_recipients_pk PRIMARY KEY NONCLUSTERED (deliveryrecipientid)

ALTER TABLE hm_dnsbl ADD CONSTRAINT hm_dnsbl_pk PRIMARY KEY NONCLUSTERED (sblid)

ALTER TABLE hm_fetchaccounts ADD CONSTRAINT hm_fetchaccounts_pk PRIMARY KEY NONCLUSTERED (faid)

ALTER TABLE hm_fetchaccounts_uids ADD CONSTRAINT hm_fetchaccounts_uids_pk PRIMARY KEY NONCLUSTERED (uidid)

ALTER TABLE hm_rules ADD CONSTRAINT hm_rules_pk PRIMARY KEY NONCLUSTERED (ruleid)

ALTER TABLE hm_rule_criterias ADD CONSTRAINT hm_rule_criterias_pk PRIMARY KEY NONCLUSTERED (criteriaid)

ALTER TABLE hm_rule_actions ADD CONSTRAINT hm_rule_actions_pk PRIMARY KEY NONCLUSTERED (actionid)

ALTER TABLE hm_adsynchronization ADD CONSTRAINT hm_adsynchronization_pk PRIMARY KEY NONCLUSTERED (synchid)

ALTER TABLE hm_iphomes ADD CONSTRAINT hm_iphomes_pk PRIMARY KEY NONCLUSTERED (iphomeid)

update hm_dbversion set value = 4200
