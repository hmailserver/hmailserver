dim app
set app = createObject("hMailServer.Application")

call app.Authenticate("Administrator", "testar")

dim obDomains
set obDomains = app.Domains

dim i 
for i = 1 to 25000
	set newDomain = obDomains.Add
	sDomainName = "test" + CStr(i) + ".com"
	newDomain.Name = sDomainName 
	newDomain.Save
   
   set obAccounts = newDomain.Accounts
   
   for a = 1 to 10
      dim address
      address = "account" & CStr(a) & "@" & sDomainName
      
      set obAccount = obAccounts.Add
      
      obAccount.Address = address
      
      obAccount.Save
   
   next
   
	
next