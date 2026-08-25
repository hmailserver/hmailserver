---
title: "OnTooManyInvalidCommands"
slug: scripting_ontoomanyinvalidcommands
parent: reference_scripts
index: 0
is_book: false
---

## Overview

This event is fired when an SMTP client has sent more invalid commands than allowed, and hMailServer is about to disconnect it. The limit is configured in *Settings->Protocols->SMTP*. The event is only fired if hMailServer is set to disconnect clients sending invalid commands.

The client is disconnected regardless of what the script does, so the event is used for logging or notification purposes.

## Signature

OnTooManyInvalidCommands(oClient as hMailServer.Client, oMessage as hMailServer.Message)

## Result values

- None

## Examples

VB-script:

```
Sub OnTooManyInvalidCommands(oClient, oMessage)

   EventLog.Write("Disconnecting " & oClient.IPAddress & " - too many invalid commands.")

End Sub
```
