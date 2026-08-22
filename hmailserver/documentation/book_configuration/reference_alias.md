---
title: "Alias"
slug: reference_alias
parent: book_configuration
index: 0
is_book: false
---

## Overview

Aliases are used to forward email from one specific address to another. Imagine them as addresses without a mailbox; instead of having their own mailbox, they store received messages in another account's mailbox. This can be useful if you want to monitor several email addresses, but only have one real email account on the server. For example, you may want to receive email messages sent to webmaster@domain.com, feedback@domain.com and yourname@domain.com, but you just want to create the webmaster@domain.com account instead of 3 different accounts. Then feedback@domain.com and yourname@domain.com can be made aliases of webmaster@domain.com

### Adding an alias

1. Navigate to the domain in hMailServer Administrator
2. Select the *Aliases*node under the domain.
3. Click *Add...*
4. Enter an email address in *Redirect from*. This is an alias email address, e.g. `feedback@domain.com` in the illustration above. Email messages sent to it will be forwarded to the address you specify in the *To* field.
5. Enter the main email address in the *To* field.
6. Click *Save*

### Notes

- You cannot use an alias address that matches the email address of an existing account.
- It is not possible to use an alias to forward an email to two different accounts. Use distribution lists instead.
- An alias may forward email messages to any account - even to accounts for domains not residing in the same server.
- When logging on the server, an alias cannot be used. Only account addresses may be used during log-on.

## Settings

### Redirect from

<div class="indented">An alias email address from which messages are to be redirected. The email address can not be the same as an account address or an address in a distribution list.</div>

### To

<div class="indented">The email address that the alias should redirect to. It can be any account, another alias, a distribution list or an email address on an external domain.</div>

 
