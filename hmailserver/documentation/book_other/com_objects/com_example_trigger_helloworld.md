---
id: 446
title: "Trigger example: Hello World"
slug: com_example_trigger_helloworld
parent: com_objects
index: 0
is_book: false
updated: 2009-12-02
---

## Overview

It's possible to implement custom scripts in hMailServer which are executed when certain actions occrur. This example shows you how to write the text Hello World to the Event log whenever a message is accepted for delivery by hMailServer.

### Script

``

<p><span class="Code">Sub OnAcceptMessage(oClient, oMessage)<br>
   EventLog.Write("Hello World")<br>
End Sub</span></p>
