---
title: "API Example: Creating an account"
slug: com_example_account_create
parent: com_objects
index: 0
is_book: false
---

This example shows how to add a new account to an existing domain in your hMailServer installation. The script is written in VBA. To use it, follow these steps:

1. Copy the script to a text editor
2. Save it as a file with the extension .vbs, for example Example.vbs
3. In the Windows Explorer, double-click on the .vbs file to run it.

# Creating an account

``

```
Dim obApp
   Set obApp = CreateObject("hMailServer.Application")

   ' Authenticate. Without doing this, we won't have permission
   ' to change any server settings or add any objects to the
   ' installation.   
   Call obApp.Authenticate("Administrator", "your-main-hmailserver-password")
   
   ' Locate the domain we want to add the account to
   Dim obDomain
   Set obDomain = obApp.Domains.ItemByName("example.com")
   
   Dim obAccount
   Set obAccount = obDomain.Accounts.Add
   
   ' Set the account properties
   obAccount.Address = "account@example.com"
   obAccount.Password = "secret"
   obAccount.Active = True
   obAccount.MaxSize = 100 ' Allow max 100 megabytes
   
   obAccount.Save
```
