---
title: "Upgrading: 3.x to 4.0"
slug: upgrade_3x_to_40
parent: book_upgrading
index: 10
is_book: false
---

## Overview

![](images/scr_installation_small.jpg) This page describes how to upgrade from hMailServer version 3.x to hMailServer version 4.0. If you follow these steps, the server downtime will only be a few minutes. Before upgrading, you should make sure that you have a backup of the entire hMailServer directory, hMailServer database and hMailServer.ini, in case something goes wrong. The steps below are the same regardless of the database server type that hMailServer is using. Please read through this entire page before you upgrade.

## Install the new version

1. IMPORTANT: Back up your hMailServer database, your data directory and the hMailServer.ini file.
2. Download hMailServer 4.0 from the hMailServer website.
3. Launch the setup executable by double-clicking on it. There is no need to stop or uninstall the previous version before installing 4.0. The hMailServer 4.0 setup will take care of that.
4. Run through the setup dialogs and click Install in the Ready to install dialog.
5. The setup software now makes a copy of the files. If you get a message that it can't overwrite the file libmysql.dll, restart Apache/IIS and then click Retry.
6. After the files have been copied, the setup software will automatically start hMailServer database upgrade.
7. Click Upgrade to upgrade the database to the 4.0 structure.
8. After the upgrade of the database, the setup software will automatically start the hMailServer service.
9. Click Exit to return to the setup wizard
10. Click Finish to exit the setup and start hMailAdmin.

## What's new

### User levels in PHPWebAdmin

In hMailServer 4.0 PHPWebAdmin has support for different user levels. For every account on the server, you can define whether it has Server administration rights, domain administration rights or only user administration rights. Using these levels, it is possible to delegate tasks to domain owners, so that domain owners can add their accounts themselves using the PHPWebAdmin.

### Rules

hMailServer 4.0 has support for rules. Using rules, you can specify how email should be treated based on its content. For example, you may want to move all email with a certain header to an IMAP folder named "spam", or you may want to forward all email that goes to order@yourdomain.com to you@yourdomain.com. This and lots of other things are possible using rules.

### Scripting support

Using scripting, you can write your own VBA-scripts that will be run on certain events. As an example, an event called *OnMessageDeliver* is executed when a message is delivered. In this event, you can do whatever you want with the email messages, such as replacing its subject when that matches certain criteria. Simply put, using VBA-scripting, you can do advanced processing of email.

### POP3 fetcher

Support for downloading email from other POP3 servers has been added. You can specify server settings, retrieval interval and whether messages should be deleted from the remote server. The POP3 protocol is used for that.

### DCOM service

The COM library in hMailServer 4.0 has support for running remotely. This allows you to run PHPWebAdmin on one computer and the hMailServer server on another. However, it requires DCOM configuration, using Component Services.

## After installation

### hMailServer.ini

In hMailServer 3.x and earlier, hMailServer.ini was stored in the Windows directory. That sometimes caused problems on servers running Terminal Services (TS), since you may only have one Windows directory per user when running TS. When you upgrade to version 4.0, the hMailServer.ini file is copied from your Windows directory to the hMailServer's Bin directory. If your server does not start after upgrade, you may need to copy the file manually from the Windows directory to the Bin directory.

### New service type

In hMailServer 4.0, the service-component has completely been rewritten. Users are running it successfully on Windows NT 4, Windows 2000, XP and Windows 2003. If you have another version of Windows, such as a MediaCenter edition, you will be installing the service-component at your own risk.

### Service & COM API

In hMailServer 4.0, the COM library is much more tightly integrated into the server service, instead of being an external DLL as in previous versions. As a result, the service must be started for the COM library to work. Since hMailAdmin uses the COM library, you can no longer stop the service using hMailAdmin. Instead, when you click Stop or Start in hMailAdmin, you start and stop the hMailServer server running inside the service. When you stop the server, hMailServer will close all connections, ports and stop the delivery of emails. However, the hMailServer service will still be running.
