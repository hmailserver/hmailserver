---
title: "Upgrading: 5.0 to 5.1"
slug: upgrade_50_to_51
parent: book_upgrading
index: 30
is_book: false
---

## Overview

This page describes how to upgrade from hMailServer version 5.0 to 5.1. If you follow these steps, the server downtime will only be a few minutes. Before upgrading, you should make sure that you have a backup of the entire hMailServer directory, hMailServer database and hMailServer.ini, in case something goes wrong. The steps below are the same regardless of the database server type that hMailServer is using. Please read through this entire page before you upgrade.

### Before installation

- Note that the system requirements for version 5 is not the same as for version 4. For example, if you are using Microsoft SQLÂ Server, Microsoft SQLÂ Server 2005 is now a requirement.
- If you are using the internal MySQLÂ database, and are upgrading from 4.2 or earlier, you should install the latest 4.4-build prior to upgrading to version 5. If you do not do this, the database upgrade will fail.

### Install the new version

1. IMPORTANT: Back up your hMailServer database, your data directory and the hMailServer.ini file.
2. Read the Backwards compatibility section below. If something is unclear, it's recommended that you install hMailServer 5 on a test server first to learn about the new functionality.
3. Download hMailServer 5 from the hMailServer website.
4. Launch the setup executable by double-clicking it. You should not stop or uninstall the current version before installing the new version.
5. Run through the setup dialogs and click Install in the Ready to install dialog.
6. The setup software now makes a copy of the files.
7. After the files have been copied, the setup software will automatically start hMailServer database upgrade. If you see the text "*Creating the hMailServer database*" in the installation wizard for more than 15 seconds, and no "*Database upgrade*"-dialog is shown, it's likely that the upgrade dialog is located "behind" the wizard. So if the upgrade dialog is not shown, try moving the installation wizard window.
8. Click Upgrade to upgrade the database to the latest structure.
9. Click Exit to return to the setup wizard
10. Click Finish to exit the setup and start hMailAdmin.

### Backwards compatibility

hMailServer 5.1 includes a lot of changes. Some of these changes modify the behavior of existing functionality. The following functionality have been redesigned in hMailServer 5. After upgrade, please review this configuration to make sure it fits your needs.

**SMTPÂ authentication settings**  

Â    

In hMailServer 5.1, the settings related to SMTPÂ authentication in IPÂ ranges have been redesigned. After upgrading to 5.1, it's important that you [review the IPÂ range settings](?page=reference_iprange) to confirm that they fit your needs.

Â 
