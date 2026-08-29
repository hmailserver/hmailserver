---
title: "Data directory synchronizer"
slug: userguide_dds
parent: book_other
index: 0
is_book: false
---

## Background

When hMailServer receives a message, information regarding this message is stored in two places. Information on SMTP envelope, receive-time, and message flags are stored in the hMailServer database (tables hm_messages and hm_messagerecipients). The actual message is stored in an .eml file in the hMailServer Data Directory.  

  

In some situations, these two may come out of synch. For example, if a database table becomes corrupt, you may have messages in the Data directory which is not referenced by the database. When this happens, you will not be able to download the messages using your e-mail client. And if the messages have not been delivered, hMailServer won't deliver them.

The solution to this problem is to use the hMailServer Data Directory Synchronizer. The tool is installed with hMailServer and located in the Addons folder.

## What does it do?

Data Directory Synchronizer goes through all the message files in the data folder. It searches for files with the extension .hma or .eml. (Older versions of hMailServer used .hma as extension, now .eml is used). When it has located an .eml file, it checks whether this .eml file is referenced by the database, if not, the message is inserted into the database.

If the .eml file exists in a user directory, hMailServer inserts the message into this users account. The message then ends up in the users Inbox folder. The IMAP folder information is stored in the database, and since this info has been lost, hMailServer does not know what folder to store the message in.

If the .eml file exists in the Data root directory, the message is inserted into the delivery queue. hMailServer parses the To and CC headers of the email to determine who the message should be delivered to. To prevent the email from being re-sent to recipients, hMailServer only sends the message to local recipients.

## How do I run it?

1. Back up your system. DDS makes a relatively large amount of changes to the database. Because of this, it's recommended that you backup everything prior to running DDS.
2. Locate DataDirectorySynchronizer.exe in the Addons folder and run it.
3. Click "Next" to start the synchronization.

### What is imported?

- Since IMAP message data, such as folder information, message flags etc are stored in the database and not in the message files, that information will not be recreated when recreate the message list. Hence, when messages are inserted into the database, all messages will end up in the inbox folder and marked as unread.
- Only files with the extension .eml or .hma are imported into the database
- For every file which already exists in the database, you will see a "Skipped" message.

### Where will imported messages be put?

- Files located in the root data folder will be delivered to local recipients in the To and CC headers. The original recipient information, from the SMTP envelope, is not available in the .EML files. Hence, the To/CC headers are used. E-mail are only delivered to local recipients to prevent mail from being re-sent to external addresses.
- Files located in data folder sub directories, such as domain\username will be connected to that user in the database.
- Files imported directly to user accounts won't be treated by filters, rules, virus scanning features in hMailServer

### What effect does it have on production?

- You don't have to stop hMailServer prior to running DDS.
- After all messages have been imported, DDS will automatically re-start the hMailServer server to refresh the cache.

### How can I improve its performance?

Data Directory Synchronizer iterates over all folders and files in the data directory. Whenever it sees a file, it checks whether this file exists in the database. If not, the message is added to the database. The time consuming part of this process is to check whether the message exists in the database. To speed up this part of the process, an index can be added to the hMailServer database. The index improves the performance when DDS searches for messages in the database.

To add this index, execute the following SQL statement in the hMailServer database:

<p><span class="Code">CREATE INDEX idx_custom_hm_messages_messagefilename ON hm_messages (messagefilename) </span></p>

 

 
