---
title: "OnClientConnect"
slug: scripting_onclientconnect
parent: reference_scripts
index: 0
is_book: false
---

## Signature

OnClientConnect(oClient as hMailServer.Client)

### Description

This event is fired directly after a TCP/IP connection has been opened towards hMailServer. The event is fired before the user has logged on, which means that the oClient.Username property will return an empty string.

### Result values

- 0 - hMailServer accepts the connection
- 1 - hMailServer rejects the connection

### Examples

VB-script:

```
Sub OnClientConnect(oClient)   
  Result.Value = 0   
  If oClient.Port = 25 Then      
     If StrComp("127.0.0.0", "127.0.0.1") Then          
       ' We only allow SMTP connections from local host.         
       Result.Value = 1      
     End if   
  End If    
End Sub
```

## Properties and Methods Available

<p><span class="Code"><a href="https://www.hmailserver.com/documentation/latest/?page=com_object_client">oClient</a>.ipaddress<br>
</span></p>

 

<p><span class="Code"><a href="https://www.hmailserver.com/documentation/latest/?page=com_object_client">oClient</a>.port<br>
</span></p>

<p><span class="Code"><a href="https://www.hmailserver.com/documentation/latest/?page=com_object_message">oMessage</a> NONE<br>
</span></p>
