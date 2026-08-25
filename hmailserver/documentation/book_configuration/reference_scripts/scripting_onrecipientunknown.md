---
title: "OnRecipientUnknown"
slug: scripting_onrecipientunknown
parent: reference_scripts
index: 0
is_book: false
---

## Overview

This event is fired when an SMTP client specifies a recipient in a local domain which does not exist. hMailServer rejects the recipient regardless of what the script does, so the event is used for logging or notification purposes.

## Signature

OnRecipientUnknown(oClient as hMailServer.Client, oMessage as hMailServer.Message)

## Result values

- None

## Examples

VB-script:

```
Sub OnRecipientUnknown(oClient, oMessage)

   EventLog.Write("Unknown recipient from " & oClient.IPAddress)

End Sub
```
