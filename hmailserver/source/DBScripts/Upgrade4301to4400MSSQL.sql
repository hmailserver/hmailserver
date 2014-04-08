ALTER TABLE hm_domains ADD domainmaxnoofaccounts INT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD domainmaxnoofaliases INT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD domainmaxnoofdistributionlists INT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD domainlimitationsenabled INT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD domainmaxaccountsize INT NOT NULL DEFAULT 0

ALTER TABLE hm_accounts ADD accountlastlogontime datetime NOT NULL DEFAULT GETDATE()

UPDATE hm_accounts SET accountlastlogontime = GETDATE()

update hm_dbversion set value = 4400

