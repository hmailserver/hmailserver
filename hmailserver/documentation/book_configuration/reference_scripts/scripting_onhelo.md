---
title: "OnHELO"
slug: scripting_onhelo
parent: reference_scripts
index: 0
is_book: false
---

## Overview

This event is fired when an SMTP client sends the HELO or EHLO command. At this point the client has not been authenticated, so oClient.Username returns an empty string. The host name given by the client is available in oClient.HELO.

## Signature

OnHELO(oClient as hMailServer.Client)

## Result values

- 0 - hMailServer accepts the HELO/EHLO command
- 1 - hMailServer rejects the command with the error *554 Rejected*
- 2 - hMailServer rejects the command with *554* and a script-defined message
- 3 - hMailServer rejects the command with *453* and a script-defined message. This is a temporary error, so the client is expected to try again later.

## Examples

VB-script:

```
Sub OnHELO(oClient)

   If oClient.HELO = "" Then
      Result.Message = "Invalid HELO host name."
      Result.Value = 2
   End If

End Sub
```
