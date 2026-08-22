---
title: "Backup & Restore"
slug: backup_restore
parent: book_maintenance
index: 10
is_book: false
---

## Overview

hMailServer includes built-in functionality which can be used for backup and restore. It's also possible to take backups using external tools.

The built in backup creates an XML file listing all messages on the server. This XML file is compressed using 7zip. It's also possible to include the actual messages in this compressed file.

The time it takes to create the backup depends on how many messages are stored on the server and the performance of the server.

<h3><span style="color: rgb(119, 119, 119); font-size: 18px; font-weight: bold;">What to backup</span></h3>

There are a couple of different things that you should backup.

- The database. The database contains all configuration data and links to the email messages stored on your disk. The easiest way to backup the database is to use mysqldump or, if you're using Microsoft SQL Server, the built-in MS SQL backup tools.
- The data directory. The data directory contains the actual email messages. It contains messages that have not yet been delivered as well as email messages that are stored in IMAP folders. The easiest way to backup the data directory is to use MS-DOS's xcopy.
- Other files. You might want to backup the Events directory (hMailServer version 4.0 onwards), the hMailServer.ini file, the log directory, etc.

## Built-in backup

hMailServer 4.2 and later versions includes built-in backup support. In the backup settings in hMailAdmin, you can define the type of objects that should be backed up. The built-in backup functionality is designed for small installations. If you have more than 50 accounts or 10 000 messages on the server, we strongly recommend that you use external tools to perform the backup.

## Hot backup

It is possible to backup hMailServer while the server is running. However, if you do so, there is a risk that some data will not be backed up. A typical scenario is when you try to backup when hMailServer has only partially received a message. If you want to be sure that everything is backed up, you should stop the server before doing the backup.   

  

`Robocopy` does everything that `xcopy` does, but it can also "synchronize", or mirror, two directories, thereby only copying the difference. That is much faster. You can copy files to a backup directory the usual way, while the hMailServer service is running. At a convenient time later, you stop the service and copy again using `robocopy`. This second copy will be much faster, as it will only copy the changes. This can help minimize downtime. However, beware that `robocopy` can cause disaster if you use it with the wrong switches. Please, read its documentation carefully before using it.

## Scheduled backup

In the hMailServer Addons folder, there's a VBA script called StartBackup.vbs. To be able to run this script, you need to edit it and enter your main hMailServer administration password. After this has been done, you can start a backup by double-clicking on the VBA file. You can use Scheduled Tasks in Windows to schedule the backup to run at specified times.

## Recommendations

Regardless of how you perform the back up, it's strongly recommended that you regulary confirm that the backups are working properly by restoring them to another server.

 
