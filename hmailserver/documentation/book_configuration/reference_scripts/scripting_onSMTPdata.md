---
id: 423
title: "OnSMTPData"
slug: scripting_onSMTPdata
parent: reference_scripts
index: 0
is_book: false
updated: 2019-05-08
---

## Overview

OnSmtpData is called ONCE per message.  

  

Do NOT try to save a message (oMessage.Save) in "Sub OnSMTPData(oClient, oMessage)" or you will seriously disrupt the message!

The following properties are the only ones populated at the point of OnSMTPData firing:

<ul>
    <li><strong>oMessage.Charset	</strong>	eg, utf-8</li>
    <li><strong>oMessage.Filename</strong>		eg, e:\hMailData\{88ECA44E-D65A-4C8C-9797-EA1C3639D723}.eml</li>
    <li><strong>oMessage.FromAddress</strong>		eg, <a href="mailto:user@domain.com">user@domain.com</a></li>
    <li><strong>oMessage.recipients.item(j).address	</strong>	eg, <a href="mailto:touser@domain.com">touser@domain.com</a><strong><span id="1436011050168S" style="display: none;"> </span><br>
    </strong></li>
</ul>

## Signature

OnSMTPData(oClient as hMailServer.Client, oMessage as hMailServer.Message)

## Result values

- 0 - hMailServer accepts the message
- 1 - hMailServer rejects the message with the error *542 Rejected*
- 2 - hMailServer rejects the message with a script-defined error.

## Examples

### Require authentication for local senders

The built-in SMTP authentication functionality checks only the sender address to determine whether SMTP authentication is required. Often, you want to check sender address as well. The following script makes hMailServer require authentication, if the senders address contains @example.com.

VB-script:

```
Sub OnSMTPData(oClient, oMessage)

   If (InStr(1, oMessage.FromAddress, "@example.com", 1) > 0) Then
      If (oClient.Username = "") Then
         Result.Message = "You must be authenticated to send from local domain."
         Result.Value = 2
      End If
   End If

End Sub
```

 
