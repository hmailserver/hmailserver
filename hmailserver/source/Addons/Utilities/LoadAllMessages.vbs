Option Explicit

Dim oApp
Set oApp = CreateObject("hMailServer.Application")

Dim lTotalCount
lTotalCount = 0


' BEGIN: Authenticate the client.
Dim sAdminPwd
sAdminPwd = InputBox("Enter your main hMailServer administrator password.", "hMailServer")
Call oApp.Authenticate ("Administrator", sAdminPwd)
' END: Authenticate the client.

ProcessAllAccounts(oApp)

MsgBox lTotalCount & " messages were loaded."

Sub ProcessAllAccounts(oApp)
   
   Dim obDomains
   Set obDomains = oApp.Domains
   obDomains.Refresh
   
   
     
   Dim lDomainIdx
   For lDomainIdx = 0 To  obDomains.Count -1
      
      Dim obDomain
      Set obDomain = obDomains.Item(lDomainIdx)
      
      ' Loop over the accounts to check whether
      ' they should expire
      
      Dim obAccounts
      Set obAccounts = obDomain.Accounts
      
      Dim lAccountsToDelete 
      lAccountsToDelete = 0
      
      ReDim arrAccounts(lAccountsToDelete)
      
      Dim lAccountIdx
      For lAccountIdx = 0 To obAccounts.Count -1
         
         Dim obAccount 
         Set obAccount = obAccounts.Item(lAccountIdx)
         
         Dim sLastLogonTime
         sLastLogonTime = obAccount.LastLogonTime
         
         ProcessAccount(obAccount)
         
      Next
      
      ' Remove expired accounts
      For lAccountIdx = 0 to UBound(arrAccounts)
         obAccounts.DeleteByDBID(arrAccounts(lAccountIdx))
      Next

      ' The list should be re-created for the next domain
      ReDim arrAccounts(0)
   Next
   
End Sub

Sub ProcessAccount(obAccount)
	Dim obFolders
	Set obFolders = obAccount.IMAPFolders
	
	Dim lFolderIdx
	For lFolderIdx = 0 To obFolders.Count - 1
		dim obImapFolder
		set obImapFolder = obFolders.Item(lFolderIdx)
		
		ProcessImapFolder(obImapFolder)
	Next
	
End Sub

' Recursively process folders
Sub ProcessImapFolder(obImapFolder)
	Dim obSubFolders
	Set obSubFolders = obImapFolder.SubFolders
	
	Dim lFolderIdx
	For lFolderIdx = 0 To obSubFolders.Count - 1
		dim obSubFolder
		set obSubFolder = obSubFolders.Item(lFolderIdx)

		ProcessImapFolder(obSubFolder)
	Next
	
	lTotalCount = lTotalCount + obImapFolder.Messages.Count
End Sub