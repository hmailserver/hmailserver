---
id: 447
title: "Trigger example: Remove headers"
slug: com_example_trigger_remove_headers
parent: com_objects
index: 0
is_book: false
updated: 2009-12-02
---

## Overview

It's possible to implement custom scripts in hMailServer which are executed when certain actions occrur. This example shows you how to remove all Received-headers from a message before it is accepted.

### Script

``

<p><span class="Code">Sub OnAcceptMessage(oClient, oMessage)<br>
   Dim oHeaders<br>
   set oHeaders = oMessage.Headers<br>
<br>
   EventLog.Write("Header count: " + CStr(oHeaders.Count))<br>
<br>
   Dim i<br>
   For i = oHeaders.Count -1 To 0 Step -1<br>
   <br>
      dim oHeader<br>
      Set oHeader =oHeaders.Item(i)<br>
   <br>
      EventLog.Write("Header found: " + oHeader.Name)<br>
      <br>
      ' Check i this is a header which we should delete.<br>
      if LCase(oHeader.Name) = "received" Then<br>
         oHeader.Delete<br>
         EventLog.Write("Header deleted.")<br>
      End If<br>
      <br>
   Next<br>
   <br>
   ' Save all the changes...<br>
   oMessage.Save<br>
   <br>
End Sub</span></p>
