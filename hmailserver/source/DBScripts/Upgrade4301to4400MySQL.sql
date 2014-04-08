ALTER TABLE hm_domains ADD COLUMN domainmaxnoofaccounts INT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD COLUMN domainmaxnoofaliases INT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD COLUMN domainmaxnoofdistributionlists INT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD COLUMN domainlimitationsenabled INT NOT NULL DEFAULT 0

ALTER TABLE hm_domains ADD COLUMN domainmaxaccountsize INT NOT NULL DEFAULT 0

ALTER TABLE hm_accounts ADD accountlastlogontime datetime NOT NULL DEFAULT 0

UPDATE hm_accounts SET accountlastlogontime = NOW()

update hm_dbversion set value = 4400

