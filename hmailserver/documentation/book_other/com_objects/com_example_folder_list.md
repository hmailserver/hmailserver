---
title: "API example: List user folders"
slug: com_example_folder_list
parent: com_objects
index: 0
is_book: false
---

This example shows how to list a users IMAP folders using the hMailServer COM API. The script will display a message box listing all the IMAP folders (including sub folders) belonging to the account test@example.com.

# Listing user folders

``

```
Dim obApp
   Set obApp = CreateObject("hMailServer.Application")

   ' Authenticate. Without doing this, we won't have permission
   ' to change any server settings or add any objects to the
   ' installation.   
   Call obApp.Authenticate("Administrator", "<administrator-password>")
   
   ' Locate the domain we want to add the account to
   Dim obDomain
   Set obDomain = obApp.Domains.ItemByName("example.com")
   
   Dim obAccount
   Set obAccount = obDomain.Accounts.ItemByAddress("test@example.com")
   
   MsgBox ListFolders(obAccount.IMAPFolders, 0)

   Function ListFolders(obFolders, iRecursion)
      ' Loop over all folders
      
      iRecursion = iRecursion +1
      
      Dim sMessage
      
        dim i
   	for i = 0 to obFolders.Count -1
		   
           Dim obFolder
           Set obFolder = obFolders.Item(i) 
		   
           sMessage = sMessage & String(iRecursion*3, " ") & obFolder.Name & vbNewLine
           sMessage = sMessage & ListFolders(obFolder.SubFolders, iRecursion)
   		
        Next
   	
   	iRecursion = iRecursion -1
   	
   	ListFolders = sMessage
	
   End Function
```
