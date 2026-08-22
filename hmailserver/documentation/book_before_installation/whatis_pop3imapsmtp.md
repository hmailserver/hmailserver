---
title: "What are SMTP, POP3 and IMAP?"
slug: whatis_pop3imapsmtp
parent: book_before_installation
index: 20
is_book: false
---

## Overview

SMTP, POP3 and IMAP are TCP/IP protocols used for mail delivery. If you plan to set up an email server such as hMailServer, you must know
what they are used for. Each protocol is just a specific set of communication rules between computers.

## SMTP

SMTP stands for Simple Mail Transfer Protocol. SMTP is used when email is delivered
from an email client, such as Outlook Express, to an email server or when email is delivered from
one email server to another. SMTP uses port 25.

## POP3

POP3 stands for Post Office Protocol. POP3 allows an email client to download an
email from an email server. The POP3 protocol is simple and does not offer many
features except for download. Its design assumes that the email client downloads all available email from the server, deletes them from the server and then disconnects. POP3 normally uses port 110.

## IMAP

IMAP stands for Internet Message Access Protocol. IMAP shares many similar features with POP3. It, too, is a protocol that an email
client can use to download email from an email server. However, IMAP includes many more
features than POP3. The IMAP protocol is designed to let users keep their email
on the server. IMAP requires more disk space on the server and more CPU resources than POP3, as all emails are stored on the server.
IMAP normally uses port 143.
Here is [more information about IMAP](?page=information_imap).

## Examples

Suppose you use hMailServer as your email server to send an email to bill@microsoft.com.

1. You click *Send* in your email client, say, Outlook Express.
2. Outlook Express delivers the email to hMailServer using the SMTP protocol.
3. hMailServer delivers the email to Microsoft's mail server, mail.microsoft.com, using SMTP.
4. Bill's Mozilla Mail client downloads the email from mail.microsoft.com to his laptop using the POP3 protocol (or IMAP).
