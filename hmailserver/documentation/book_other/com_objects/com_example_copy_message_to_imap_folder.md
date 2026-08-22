---
title: "API Example: Copying a message to a new IMAP folder"
slug: com_example_copy_message_to_imap_folder
parent: com_objects
index: 0
is_book: false
---

# Overview

This example shows how to copy a message from one IMAP folder to another. The script requires hMailServer 5 or later.

## Copying a message

``

```
Dim obApp
   Set obApp = CreateObject("hMailServer.Application")

   ' Authenticate. Without doing this, we won't have permission
   ' to change any server settings or add any objects to the
   ' installation.   
   Call obApp.Authenticate("Administrator", "secret")
   
   ' Locate the domain we want to add the account to
   Dim obDomain
   Set obDomain = obApp.Domains.ItemByName("example.com")
   
   Dim obAccount
   Set obAccount = obDomain.Accounts.ItemByAddress("test@example.com")
   
   Dim obInboxFolder 
   Set obInboxFolder = obAccount.IMAPFolders.ItemByName("Inbox")

   ' Assume that the backup folder already exists. This may
   ' not be the case.      
   Dim obBackupFolder 
   Set obBackupFolder = obAccount.IMAPFolders.ItemByName("Backup")
   
   ' Copy all messages in Inbox to the backup folder
   Dim obMessages
   Set obMessages = obInboxFolder.Messages
   
   For i = 0 to obMessages.Count - 1
      Call obMessages.Item(i).Copy(obBackupFolder.ID)
   Next
```
