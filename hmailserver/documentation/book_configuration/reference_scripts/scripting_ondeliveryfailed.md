---
title: "OnDeliveryFailed"
slug: scripting_ondeliveryfailed
parent: reference_scripts
index: 0
is_book: false
---

## Overview

This event is executed if delivery to a recipient fails. If a message to multiple recipients cannot be sent, the event is executed once for every recipient.

Notice that the event is only executed if hMailServer has accepted a message for delivery. If hMailServer rejects a message, for instance during anti spam check, OnDeliveryFailed is not excuted.

## Signature

OnDeliveryFailed(oMessage, sRecipient, sErrorMessage)

## Parameters

- oMessage - The message which has not been delivered.
- sRecipient - The recipient which the message has not been delivered to
- sErrorMessage - The error which caused the delivery failure

## Result values

- None
