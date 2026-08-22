---
id: 20
title: "OnAcceptMessage"
slug: scripting_onacceptmessage
parent: reference_scripts
index: 0
is_book: false
updated: 2017-08-05
---

## Signature

OnAcceptMessage(oClient as hMailServer.Client, oMessage as hMailServer.Message)

## Result values

- 0 - hMailServer accepts the message
- 1 - hMailServer rejects the message with the error *542 Rejected*
- 2 - hMailServer rejects the message with a script-defined error.

## Examples

### Require authentication for local senders

The built-in SMTP authentication functionality checks only the sender address to determine whether SMTP authentication is required. Often, you want to check sender address as well. The following script makes hMailServer require authentication, if the senders address contains @example.com.

VB-script:

```
Sub OnAcceptMessage(oClient, oMessage)

   If (InStr(1, oMessage.FromAddress, "@example.com", 1) > 0) Then
      If (oClient.Username = "") Then
         Result.Message = "You must be authenticated to send from local domain."
         Result.Value = 2
      End If
   End If

End Sub
```

### Reject message larger than a specific size.

VB-script:

```
Sub OnAcceptMessage(oClient, oMessage)       

   If oMessage.Size > 10 Then          
      ' Reject the message since it's larger          
      ' than 10 kb.          
      Result.Message = "This message is too large"
      Result.Value = 2
   Else
      ' Accept the message          
      Result.Value = 0     
   End If     

End Sub
```

## Properties and Methods Available

<p><span class="Code"><a href="https://www.hmailserver.com/documentation/latest/?page=com_object_client">oClient</a> all<br>
</span></p>

<p><span class="Code"><a href="https://www.hmailserver.com/documentation/latest/?page=com_object_message">oMessage</a> all</span></p>

## Notes

- OnAcceptMessage is executed before hMailServer has accepted the email message and before it has been saved in the database. This means that you cannot access the ID of the message in this event.

 
