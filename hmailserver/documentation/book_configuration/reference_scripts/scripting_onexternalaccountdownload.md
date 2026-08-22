---
title: "OnExternalAccountDownload"
slug: scripting_onexternalaccountdownload
parent: reference_scripts
index: 0
is_book: false
---

## Overview

This event is executed when a message has been downloaded from an external account to hMailServer.

## Signature

Sub OnExternalAccountDownload(oFetchAccount, oMessage, sRemoteUID)

## Parameters

- oFetchAccount - The external account the message was downloaded for.
- oMessage - The message which has been downloaded. If the message has already been downloaded before, this parameter will be Nothing / Null
- sRemoteUID - The UID of the message on the remote server.

## Result values

Using the Result object, it's possible to define whether hMailServer should delete the message from the remote server or leave it. Set the below values to the Result.Value variable.

- 1 - Delete the message from the remote server immediately.
- 2 - Delete the message after a specified number of days. Set the number of days to Result.Parameter variable.
- 3 - Never delete messages from the remote server.

Note that downloaded messages will still be delivered. The above settings only affect whether or not messages are deleted from the remote server.

 
