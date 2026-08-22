---
title: "API example: Sending a message"
slug: com_example_message_send
parent: com_objects
index: 0
is_book: false
---

This example shows how to send an email using the hMailServer COM API.

# Sending a message

`dim oMessage 
Set oMessage = CreateObject("hMailServer.Message")
oMessage.From = "``Me <myaddress@mydomain.com>"
oMessage.FromAddress = "myaddress@mydomain.com" 
oMessage.Subject = "Hi" 
oMessage.AddRecipient "My friend", "myfriend@myfriend.com"
oMessage.Body = "This is the contents of the email." 
oMessage.Save`
