---
id: 208
title: "API Example: Deleting an account"
slug: com_example_account_delete
parent: com_objects
index: 0
is_book: false
updated: 2007-05-11
---

This example shows how to delete an account from your domain in your hMailServer installation. The script is written in VBA. To use it, follow these steps:

1. Copy the script to a text editor
2. Save it as a file with the extension .vbs, for example Example.vbs
3. In the Windows Explorer, double-click on the .vbs file to run it.

# Deleting an account

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

   ' To be able to delete an account, we need to know the database
   ' identifier for it. Because of this, we first need to fetch the account object.      
   Dim obAccount
   Set obAccount = obDomain.Accounts.ItemByAddress("account@example.com")
   
   obDomain.Accounts.DeleteByDBID obAccount.ID
```
