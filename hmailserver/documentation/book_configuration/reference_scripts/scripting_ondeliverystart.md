---
id: 224
title: "OnDeliveryStart"
slug: scripting_ondeliverystart
parent: reference_scripts
index: 0
is_book: false
updated: 2014-10-07
---

## Overview

This event is executed directly when the delivery of a message has started. The event is executed before any global rules are executed

## Signature

OnDeliveryStart(oMessage as hMailServer.Message)

## Result values

- 0 - Deliver the message
- 1 - Do not deliver the message

## Examples

VB-script:  

`Sub OnDeliveryStart(oMessage)              
   If oMessage.Size > 1000000 Then                   
      ' This message is to big. We should not                    
      ' deliver it.                   
      Result.Value = 1                         
   Else                   
      ' Accept the message                    
      Result.Value = 0            
   End If      
End Sub`
