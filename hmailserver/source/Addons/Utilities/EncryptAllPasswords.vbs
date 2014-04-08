option explicit

dim oApp
set oApp = CreateObject("hMailServer.Application")

' BEGIN: Authenticate the client.
Dim sAdminPwd
sAdminPwd = InputBox("Enter administrator password", "hMailServer")
Call oApp.Authenticate ("Administrator", sAdminPwd)
' END: Authenticate the client.

dim oDomains
set oDomains = oApp.Domains

dim iTotalCount

dim iDomainIdx
For iDomainIdx = 0 To oDomains.Count -1

	dim oDomain
	set oDomain = oDomains.Item(iDomainIdx)

	' Iterate over user accounts
	dim oAccounts
	set oAccounts = oDomain.Accounts

	dim iAccountCount
	iAccountCount = oAccounts.Count

	Dim iAccountIdx
	
	for iAccountIdx = 0 To iAccountCount - 1
		dim oAccount
		set oAccount = oAccounts.Item(iACcountIdx)

		oAccount.Save

		set oAccount = Nothing

		iTotalCount = iTotalCount +1
	next 
		
	set oDomain = Nothing
Next 

set oApp = Nothing

msgbox iTotalCount & " password(s) encrypted."