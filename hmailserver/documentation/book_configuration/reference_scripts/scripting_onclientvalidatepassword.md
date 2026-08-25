---
title: "OnClientValidatePassword"
slug: scripting_onclientvalidatepassword
parent: reference_scripts
index: 0
is_book: false
---

## Overview

This event is fired when hMailServer is about to validate the password for an account. It lets a script accept or reject the password, or leave the decision to hMailServer.

The password is passed to the script in clear text, which means the event is only fired for logon methods where the client sends the password itself.

## Signature

OnClientValidatePassword(oAccount as hMailServer.Account, sPassword)

## Parameters

- oAccount - The account the client is trying to log on to.
- sPassword - The password the client supplied.

## Result values

- 0 - The password is correct. The client is logged on.
- 1 - The password is incorrect. The logon is rejected.
- 2 - hMailServer validates the password as usual. This is the default.

## Examples

VB-script:

```
Sub OnClientValidatePassword(oAccount, sPassword)

   ' Let a service account log on with a password stored elsewhere.
   If oAccount.Address = "service@example.com" Then
      If sPassword = "secret" Then
         Result.Value = 0
      Else
         Result.Value = 1
      End If
   End If

End Sub
```
