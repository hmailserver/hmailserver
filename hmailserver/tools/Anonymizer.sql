DECLARE @domainname varchar(80)
DECLARE @counter int
DECLARE @newdomainname varchar(80)

DECLARE my_cursor CURSOR FOR
SELECT domainname FROM hm_domains
OPEN my_cursor

SET @counter = 0

FETCH NEXT FROM my_cursor
INTO @domainname

WHILE @@FETCH_STATUS = 0
BEGIN
	SET @counter = @counter + 1
	SET @newdomainname = 'example-' + CONVERT(varchar, @counter) + '.com'

	UPDATE hm_accounts SET accountaddress = REPLACE(accountaddress, @domainname, @newdomainname)
	UPDATE hm_accounts SET accountpassword = ''
	UPDATE hm_accounts SET accountaddomain = 'dummy' where accountaddomain <> ''
	UPDATE hm_accounts SET accountadusername = 'dummy' where accountadusername <> ''
	UPDATE hm_accounts SET accountvacationmessage = 'dummy' where accountvacationmessage <> ''
	UPDATE hm_accounts SET accountvacationsubject = 'dummy' where accountvacationsubject <> ''
	UPDATE hm_accounts SET accountforwardaddress = 'dummy@example.com' where accountforwardaddress <> ''
	UPDATE hm_accounts SET accountsignatureplaintext = 'dummy' where len(convert(varchar, accountsignatureplaintext)) > 0
	UPDATE hm_accounts SET accountsignaturehtml = 'dummy' where len(convert(varchar, accountsignaturehtml)) > 0
	
	UPDATE hm_aliases SET aliasname = REPLACE(aliasname, @domainname, @newdomainname)
	UPDATE hm_aliases SET aliasvalue = 'example@example.com'

	UPDATE hm_distributionlists SET distributionlistaddress = REPLACE(distributionlistaddress, @domainname, @newdomainname)
	UPDATE hm_distributionlists SET distributionlistrequireaddress = 'dummy' where distributionlistrequireaddress <> ''

	UPDATE hm_distributionlistsrecipients SET distributionlistrecipientaddress = 'dummy@example.com' WHERE distributionlistrecipientaddress <> ''

	UPDATE hm_domain_aliases SET daalias = 'example' + CONVERT(varchar, daid) + '.com'

	UPDATE hm_domains SET domainname = @newdomainname WHERE domainname = @domainname
	UPDATE hm_domains SET domainpostmaster = 'dummy' WHERE domainpostmaster <>''
	UPDATE hm_domains SET domainaddomain = 'dummy' WHERE domainaddomain <>''
	UPDATE hm_domains SET domainsignatureplaintext = 'dummy' WHERE convert(varchar, domainsignatureplaintext) <>''
	UPDATE hm_domains SET domainsignaturehtml = 'dummy' WHERE convert(varchar, domainsignaturehtml) <>''

	DELETE FROM hm_fetchaccounts

    FETCH NEXT FROM my_cursor
    INTO @domainname
END

CLOSE my_cursor
DEALLOCATE my_cursor

UPDATE hm_imapfolders SET foldername = 'Folder' + CONVERT(varchar, folderid)
DELETE FROM hm_messagerecipients
UPDATE hm_messages SET messagefrom = 'From' + CONVERT(varchar, messageid )

UPDATE hm_routes SET routedomainname = 'example' + CONVERT(varchar, routeid )
UPDATE hm_routes SET routetargetsmthost = 'example' + CONVERT(varchar, routeid )
UPDATE hm_routes SET routeauthenticationusername = 'dummy' where routeauthenticationusername <> ''
UPDATE hm_routes SET routeauthenticationpassword = 'dummy' where routeauthenticationpassword <> ''

DELETE FROM hm_rule_actions
DELETE FROM hm_rule_criterias
DELETE FROM hm_rules

select * from hm_settings

update hm_settings set settingname = '' where settingname = 'smtprelayerusername'
update hm_settings set settingname = '' where settingname = 'smtprelayerpassword'


update hm_greylisting_triplets set glsenderaddress = convert(varchar, glid) + '@e.com'
update hm_greylisting_triplets set glrecipientaddress = convert(varchar, glid) + '@e.com'
