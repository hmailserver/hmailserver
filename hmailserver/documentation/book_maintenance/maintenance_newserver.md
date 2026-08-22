---
id: 231
title: "Moving to a new server"
slug: maintenance_newserver
parent: book_maintenance
index: 20
is_book: false
updated: 2019-09-21
---

## Overview

This document describes how to move hMailServer from one physical server to another. The procedure is split up into six steps.

1. Check requirements
2. Install hMailServer on new server
3. Backup your system from the old machine
4. Restore your system to the new machine
5. Check TCP/IP-related settings
6. Confirm the migration

If it is the first time you're following these steps, it's recommended that you do it in a test installation prior to doing it in a production installation. This is to ensure that you fully understand exactly what you need to do.

## Step 1 - Check requirements

Before you begin moving your hMailServer installation to a new server, you should check that the new server matches the hMailServer requirements. Apart from the [standard requirements](?page=system_requirements).

<h3><span style="color: rgb(119, 119, 119); font-size: 18px;">Step 2 - Install hMailServer on new server</span></h3>

Before you begin the actual data migration, you should install hMailServer on the new server. It is critical that you install the SAME VERSION of hMailServer as you used on the old server, and that you install it in the same directory as on the old server. If you had hMailServer in C:\hMailServer on old server, you should have it in the same path on the new.

## Step 3 - Backup your system using the internal Backup

Select all options including compression

## Step 4 - Copy the backup and restore to new system

Copy the completed backup to the new machine and restore

>

## Step 5 - TCP/IP-related settings

After restoring the backup, remember to:

- Update any MX record which was pointing at the old host name.
- Update the host name in your hMailServer configuration (if it has changed)
- Update WebMail Cleint coinfiguration (if server settings have changed)
- Check that your Windows Firewall is not blocking traffic to hMailServer
- Ensure that port forwards from router / edge device / Hardware Firewall are pointing to the new server
- Test connections to SpamAssassin and AntiVirus installations
- Run the built in Self-Diagnostic tests

## Step 6 - Confirm the migration

- Confirm that you can send email to your server from an external server
- Confirm that you can log on the new server and access your email
- Notify your users that the migration is complete (if required), and optionally  instruct them to update their email clients with the new host information.

 
