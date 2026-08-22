---
id: 104
title: "Administer hMailServer remotely"
slug: howto_administer_remote
parent: howtos
index: 0
is_book: false
updated: 2016-10-04
---

## Overview

This howto describes how to administer hMailServer remotely. *Computer A* is the computer where hMailServer should be running, and *Computer B* is the computer you want to administer hMailServer from. Both computers are in the same Windows domain. Remote administration is made using DCOM.

## Computer A

Install hMailServer as normal.

You need to allow the DCOM connection through the firewall. For Windows Firewall the process is:-

1. Windows Controlpanel->Open Firewall Settings->Advanced settings->Inbound Rules
2. Rightclick->New Rule->Program->This program path-><path>MailServer.exe
3. Next->Allow the connection->Next-> click Domain/Private/Public -> next
4. Give it some name (for example hms_dcom_in)

## Computer B

You need to have hMailServer Administrator installed, and the hMailServer COM library must be registered. Since there is no installation that does only this, you have to install a full hMailServer installation and then disable the hMailServer service. To install hMailServer, follow these steps:

1. Install the same version of hMailServer as on Computer A.
2. Open up the Control Panel, Administrative tools, Services.
3. Locate the hMailServer service, stop it and disable it.

Next, you need to tell Windows that when you're running hMailServer Administrator, you want to administer the hMailServer installation running on *Computer A*. This is done by modifying the settings for the hMailServer COM object on Computer B.

1. Open the Control panel, Administrative tools, Component services.
2. Expand *Component services->Computers->My computer->DCOM config*.
3. Locate the hMailServer
4. Right-click on it, and select Properties
5. On the *Location*-tab, de-select *Run application on this computer* and select "Run application on the following computer
6. Specify the name of Computer A
7. Start hMailAdmin.
