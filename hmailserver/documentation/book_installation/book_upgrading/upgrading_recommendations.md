---
title: "Upgrading recommendations"
slug: upgrading_recommendations
parent: book_upgrading
index: 0
is_book: false
---

## Overview

This document gives you a few general recommendations when it comes to upgrading hMailServer from one version to a newer.

### Step 1: Backup everything

Before you upgrade hMailServer, you should back up all your email messages and settings. It's very rare that an upgrade of hMailServer fails, but if it does, you might need to restore a backup of your installation. Remember that if an upgrade fails, all your email may be lost so this is very important that you backup your system before upgrading.

### Step 2: Uninstall old version before installing the new?

Generally, there's no need to uninstall the old hMailServer version before installing the new one. The hMailServer installation program will automatically stop the current hMailServer installation before installing the new version. The recommendation is therefore not to uninstall the existing version before installing the new one.   

  

The exception to this rule is if you upgrade from hMailServer 3.x to hMailServer 4.x. The hMailServer COM API has a new name in hMailServer 4.x. If you don't uninstall hMailServer 3.x before installing hMailServer 4.x, these old API will still exist on your system. This should never cause any problems. But if you don't want the old API to still be available, you should uninstall hMailServer 3.x before installing hMailServer 4.x.   

  

Uninstalling hMailServer does not remove any email messages, accounts or other settings. You can uninstall hMailServer and then install it again without losing any messages.

### Step 3: Install the new version

The below steps are the same for most hMailServer version. For version specific information, please see the Upgrading topic in the documentation.

1. Download hMailServer from the hMailServer website.
2. Launch the setup executable by double-clicking it.
3. Run through the setup dialogs and click Install in the Ready to install dialog.
4. The setup software now makes a copy of the files. If you get a message that it can't overwrite the file libmysql.dll, restart Apache/IIS and then click Retry.
5. After the files have been copied, the setup software will automatically start hMailServer database upgrade if needed.
6. If hMailServer database upgrade is started, click Upgrade to upgrade the database to the new database structure.
7. After the upgrade of the database, the setup software will automatically start the hMailServer service.
8. Click Exit to return to the setup wizard
9. Click Finish to exit the setup and start hMailAdmin.

The setup installs the MySQL/MariaDB client library libmariadb.dll into the hMailServer Bin directory. If you previously placed a libmysql.dll there yourself, it is left untouched, but hMailServer now uses libmariadb.dll instead. Delete libmariadb.dll if you want hMailServer to keep using your own libmysql.dll.

### Upgrading multiple version steps

When upgrading hMailServer to the latest version, you can install the latest version immediately in step 3. For example, when upgrading from 4.0 to 4.4, you do not have to install 4.1, 4.2 or 4.3 since version 4.4 contains all changes made for 4.1, 4.2 and 4.3. It's possible to upgrade directly from 2.0 and later versions to the latest version without installing any other versions in between. There are exceptions to this rule though:

- If you're using MySQL, and upgrading from 4.3 or earlier to version 5, you must upgrade to the latest 4.4 build prior to upgrading to version 5.

### Frequently asked questions

**Is my data preserved when I upgrade?**

When you upgrade from one version to a later version, the data in your database and all email messages are preserved. However, there's a risk that something goes wrong during the upgrade so it is still important to take a full backup of your installation.

**Will hMailServer continue using my current database?**

When you upgrade from one version to a later version, hMailServer will continue using the same database as before. The upgrade process will update the database table structure so that it is compatible with the new version of hMailServer.
