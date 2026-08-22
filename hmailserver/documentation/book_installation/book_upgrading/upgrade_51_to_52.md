---
title: "Upgrading: 5.1 to 5.2"
slug: upgrade_51_to_52
parent: book_upgrading
index: 40
is_book: false
---

## Overview

This page describes how to upgrade from hMailServer version 5.1 to 5.2. If you follow these steps, the server downtime will only be a few minutes. Before upgrading, you should make sure that you have a backup of the entire hMailServer directory, hMailServer database and hMailServer.ini, in case something goes wrong. The steps below are the same regardless of the database server type that hMailServer is using. Please read through this entire page before you upgrade.

### Before installation

- If you are using the internal MySQLÂ database, and are upgrading from 4.2 or earlier, you should install the latest 4.4-build prior to upgrading to version 5. If you do not do this, the database upgrade will fail.
- If you are using other software which communicates with hMailServer, make sure that that software is compatible with the hMailServer version you are upgrading to. This may be the case for example if you're using 3'rd party web interfaces for the server.

### Install the new version

1. IMPORTANT: Back up your hMailServer database, your data directory and the hMailServer.ini file.
2. Read the Backwards compatibility section below. If something is unclear, it's recommended that you install hMailServer 5 on a test server first to learn about the new functionality.
3. Download hMailServer 5 from the hMailServer website.
4. Launch the setup executable by double-clicking it. You should not stop or uninstall the current version before instaling the new version.
5. Run through the setup dialogs and click Install in the Ready to install dialog.
6. The setup software now makes a copy of the files.
7. After the files have been copied, the setup software will automatically start hMailServer database upgrade. If you see the text "*Creating the hMailServer database*" in the installation wizard for more than 15 seconds, and no "*Database upgrade*"-dialog is shown, it's likely that the upgrade dialog is located "behind" the wizard. So if the upgrade dialog is not shown, try moving the installation wizard window.
8. Click Upgrade to upgrade the database to the latest structure.
9. Click Exit to return to the setup wizard
10. Click Finish to exit the setup and start hMailAdmin.

### What's new

- In hMailServer Administrator, under Utilities, a new basic **diagnostic tool** is now available. This tool helps you troubleshoot basic set up issues.
- If a client sends a message from a local domain, the sener is now considered local. The same applies if his address matches a route. The previous behavior was that a sender was only considered local if his address matched an local account.Â This means that if you send from an alias address, you are now considered local. Please note that it's still not possible to authenticate using an alias though.
- In the grey listing options you can now choose to **bypass greylisting if SPF passes**.
- A new rule action has been added; **Create Copy**. This can be used for example to deliver copies of the same message to different destination servers.
- A new rule criteria has been added; **Delivery attempts**. This can be used for example to deliver messages to different routes, depending on the current number of delivery attempts.
- If hMailServer tries to deliver the same message multiple times, global rules will now be executed every time.
- When you delete domains in hMailServer, you now need to confirm the deletion before the domain is removed.
- The database upgrade is now done in a transaction (assuming the underlying database supports it).
- In the database setup wizard, the creation log on the second last page is now cleared when the page is shown. Previously, the log could contain data from previous creation attempts, which could cause confusion.

### Backwards compatibility

hMailServer 5.2 includes a lot of changes. Some of these changes modify the behavior of existing functionality. The following functionality have been redesigned in hMailServer 5. After upgrade, please review this configuration to make sure it fits your needs.

**SMTPÂ authentication settings**  

Â    

In hMailServer 5.2, the settings related to SMTPÂ authentication in IPÂ ranges have been redesigned. After upgrading to 5.2, it's important that you [review the IPÂ range settings](?page=reference_iprange) to confirm that they fit your needs.

**Message UIDs**  

Â    

When upgrading to hMailServer 5.2, message UID's will be regenerated. If your email client has connected information to the specific UID's, this information may be lost. For example, some email clients store flag information such as Important and Follow up per message UID, which means that these flags may be lost during an upgrade to 5.2.

Â 
