---
title: "Backup"
slug: reference_backup
parent: book_configuration
index: 0
is_book: false
---

## Overview

hMailServer backups are made by the hMailServer service. When you choose to start a backup in hMailServer Administrator, hMailServer Administrator connects to the hMailServer service using the COM API, and tells the hMailServer service to start a backup. Because of this, the hMailServer service must be running when a backup is being made.

The built-in backup functionality is designed for small installations. If you have more than 50 accounts or 10 000 messages on the server, we **strongly recommend** that you use external tools to perform the backup.  

  

A backup file made in a specific version cannot be restored in a different version. For example, you can not restore a backup created by 4.2 in 4.3.

### Backup to network drives

For a backup to be successful, the hMailServer service must have permission to write to the path you have specified as backup destination.   

  

If the backup destination is a network drive, you must ensure that the hMailServer service has permissions to write to this drive. Normally, you will have to change the Log-on account for the hMailServer service before backing up to a network drive. This is done in the service settings in the Windows control panel

### More information

For more information, see

- [Backup recommendations](?page=backup_restore)

## Settings

**Note**: Since backup is a critical part of running a server, and hMailServer 4.2 is the first version to include built-in backup support, you should consider the hMailServer backup feature to be experimental. Do not rely on it for business critical purposes.

The built-in backup functionality is designed for small installations. If you have more than 50 accounts or 10 000 messages on the server, we **strongly recommend** that you use external tools to perform the backup.

### Destination

<div class="indented">The path where the hMailServer backup will be stored.</div>

### Settings

<div class="indented">If this option is selected, hMailServer will backup the settings. The option includes global rules, SMTP, POP3, IMAP settings, cache, multihoming and the other options found under the <em>Settings</em> node in hMailServer Administrator.</div>

### Domains

<div class="indented">This option includes all hMailServer domains and the objects connected to the domains. This means that if you chose to backup domains, accounts, external accounts, account level rules, aliases, distribution lists and other objects that belongs to a domain will be backuped. This option does not include IMAP folders connected to accounts.</div>

### Messages

<div class="indented">If you've chosen to backup domains, you can choose to backup messages as well. If you choose this option, hMailServer will backup IMAP folders and the messages stored in these IMAP folders. Messages in the hMailServer delivery queue are not backuped.</div>
