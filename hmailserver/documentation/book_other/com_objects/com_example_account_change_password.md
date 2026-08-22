---
title: "API Example: Changing an account password"
slug: com_example_account_change_password
parent: com_objects
index: 0
is_book: false
---

This example shows how to change the password for an account in your hMailServer installation. The script is written in VBA. To use it, follow these steps:

1. Copy the script to a text editor
2. Save it as a file with the extension .vbs, for example Example.vbs
3. In the Windows Explorer, double-click on the .vbs file to run it.

# Changing password for account

``

```
Dim obApp
   Set obApp = CreateObject("hMailServer.Application")

   ' Authenticate. Without doing this, we won't have permission
   ' to change any server settings or add any objects to the
   ' installation.   
   Call obApp.Authenticate("Administrator", "testar")
   
   ' Locate the domain we want to add the account to
   Dim obDomain
   Set obDomain = obApp.Domains.ItemByName("example.com")
   
   Dim obAccount
   Set obAccount = obDomain.Accounts.ItemByAddress("account@example.com")
   
   ' Set the password to "secret"   
   obAccount.Password = "secret"   

   obAccount.Save
```
