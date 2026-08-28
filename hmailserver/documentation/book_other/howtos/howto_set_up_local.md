---
title: "Set up local / stand-alone server"
slug: howto_set_up_local
parent: howtos
index: 0
is_book: false
---

## Background

This document describes how to set up an email server for internal usage only. Users in your network will be able to send and receive email via the server. They will not be able to send or receive email from other users on the Internet.

### Create the domain

Start hMailServer Administrator and select *Add domain*.

Type in a made-up domain name such as companyname.local. To prevent confusion, it's recommended that you do not choose a domain name already used by someone else on the Internet.

Make sure the domain is enabled / active.

### Create the accounts

After the domain is created, create accounts for your users. For example, you can set up accounts named bill@companyname.local and sales@companyname.local.

### Configure firewalls

Make sure that any firewall running on the same server as hMailServer is configured to allow incoming traffic on ports 25, 110 (for POP3) and 143 (for IMAP).

### Configure email clients

When the accounts have been created and firewall configured, it's time to configure your email clients. As user name, you should use the full email address, for example bill@companyname.local.  As server address, use the name or IP address of the computer which hMailServer runs on. Note that the server name is typically not the same as the domain name.

### Done

After the above steps have been performed, your users in your network will be able to send email to each other, using the internal addresses.

 
