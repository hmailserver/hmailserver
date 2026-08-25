---
title: "OnClientLogon"
slug: scripting_onclientlogon
parent: reference_scripts
index: 0
is_book: false
---

## Overview

This event is fired when a client has logged on to hMailServer using SMTP, POP3 or IMAP. The event is fired after the user name and password have been verified, so it cannot be used to reject the logon. Use it for logging or notification purposes.

## Signature

OnClientLogon(oClient as hMailServer.Client)

## Result values

- None

## Examples

VB-script:

```
Sub OnClientLogon(oClient)

   EventLog.Write("User " & oClient.Username & " logged on from " & oClient.IPAddress)

End Sub
```
