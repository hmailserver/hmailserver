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
from an email client, such as Outlook or Thunderbird, to an email server, and when email is delivered from
one email server to another.

SMTP uses two different ports, for two different purposes:

* Port 25 is used when one email server delivers email to another. This is the port other mail servers on the internet connect to when they have mail for your domain.
* Port 587, the *submission* port, is used when an email client delivers email to your server. Clients authenticate on this port, so that only your own users can send mail through your server.

hMailServer listens on both ports after installation. A third port, 465, is also commonly used for submission from email clients; it does the same thing as port 587, but the connection is encrypted from the start. hMailServer does not set up port 465 by default, but you can add it yourself.

## POP3

POP3 stands for Post Office Protocol version 3. POP3 allows an email client to download an
email from an email server. The POP3 protocol is simple and does not offer many
features except for download. Its design assumes that the email client downloads all available email from the server, deletes them from the server and then disconnects. POP3 normally uses port 110, or port 995 for connections that are encrypted from the start.

## IMAP

IMAP stands for Internet Message Access Protocol. IMAP shares many similar features with POP3. It, too, is a protocol that an email
client can use to download email from an email server. However, IMAP includes many more
features than POP3. The IMAP protocol is designed to let users keep their email
on the server, which means that the same mailbox can be read from several devices, such as a computer and a phone, and look the same on all of them. IMAP requires more disk space on the server and more CPU resources than POP3, as all emails are stored on the server.
IMAP normally uses port 143, or port 993 for connections that are encrypted from the start.
Here is [more information about IMAP](?page=information_imap).

## Encryption

All three protocols were originally designed to send data, including passwords, unencrypted over the network. Encryption was added later, in two variants:

* STARTTLS, where the client connects to the normal port (25, 587, 110 or 143) and then asks the server to switch the connection over to an encrypted one.
* Implicit TLS, where the connection is encrypted from the start. This is what ports 465, 995 and 993 are used for.

hMailServer supports both variants. Which one a port uses is decided by the *Connection security* setting of the port, under Settings -> Advanced -> TCP/IP ports; see [SSL/TLS](?page=reference_ssltls) for a description of the alternatives. Encryption requires an SSL/TLS certificate, so you need one before your users can connect securely. The [installation guide](?page=howto_install) lists the recommended setting for each port.

## Examples

Suppose you use hMailServer as your email server to send an email to john@example.com.

1. You click *Send* in your email client, say, Thunderbird.
2. Thunderbird delivers the email to hMailServer using the SMTP protocol, on port 587.
3. hMailServer looks up the mail server responsible for example.com and delivers the email to it using SMTP, on port 25.
4. John's email client downloads the email from his mail server to his laptop using the IMAP protocol (or POP3).
