'
' Iterates over all domains and accounts and loads 
' all message list.
'
' Purpose is to determine the size of the message
' cache.
'
' Usage:
' Run from prompt using
' cscript MailboxLoader.vbs
'


Option Explicit

Dim oApp
Set oApp = CreateObject("hMailServer.Application")

Call oApp.Authenticate ("Administrator", "testar")

dim obDomains
set obDomains = oApp.Domains

dim domainCount
domainCount = obDomains.Count

dim i
For i = 0 to domainCount -1

   dim obDomain
   set obDomain = obDomains.Item(i)
   
   dim obAccounts
   set obAccounts = obDomain.Accounts
   
   dim a
   For a = 0 to obAccounts.Count - 1 
      
      Dim obAccount
      Set obAccount = obAccounts.Item(a)
      
      dim obMessages
      set obMessages = obAccount.Messages
            
   Next 

   WScript.Echo("Domain " & i+1 & " of " & domainCount)

Next

WScript.Echo("Done")
