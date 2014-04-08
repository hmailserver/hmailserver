Option Explicit

' This script removes unused accounts from your hMailServer 
' installation. An account is considered to be unused if no
' log on has been made in the last X days.

Const sAdminPassword = "<ADMINISTRATORPASSWORD>"
Const lDaysBeforeDeletion = -1

If lDaysBeforeDeletion = -1 Or sAdminPassword = "<ADMINISTRATORPASSWORD>" Then
   
   MsgBox "Please configure this script before using it.", 0, "Configuration error"

Else
   
   Dim oApp
   Set oApp = CreateObject("hMailServer.Application")
   
   ' Authenticate the client.
   Call oApp.Authenticate ("Administrator", sAdminPassword)
   
   Call RemoveOldAccounts(oApp, lDaysBeforeDeletion)

   MsgBox "Script has completed", 0, "Remove unused accounts"
End If



Sub RemoveOldAccounts(oApp, lDaysBeforeDeletion)
   
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
         
         Dim lTimeSinceLastLogon
         lTimeSinceLastLogon = DateDiff("d", sLastLogonTime, Now())
         
         If lTimeSinceLastLogon > lDaysBeforeDeletion Then
            ' This account should be removed
            ReDim Preserve arrAccounts(lAccountsToDelete)
            
            arrAccounts(UBound(arrAccounts)) = obAccount.ID
            
            lAccountsToDelete = lAccountsToDelete +1
         End If
         
      Next
      
      ' Remove expired accounts
      For lAccountIdx = 0 to UBound(arrAccounts)
         obAccounts.DeleteByDBID(arrAccounts(lAccountIdx))
      Next

      ' The list should be re-created for the next domain
      ReDim arrAccounts(0)
   Next
   
End Sub