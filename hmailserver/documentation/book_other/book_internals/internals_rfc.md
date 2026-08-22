---
title: "RFC's"
slug: internals_rfc
parent: book_internals
index: 0
is_book: false
---

## Overview

This page describes RFC's which are relevant to hMailServer.

### Protocols

- [RFC 5321](http://www.ietf.org/rfc/rfc5321.txt) - Simple Mail Transfer Protocol, SMTP
- [RFC 3501](http://tools.ietf.org/html/rfc3501) - Internet Message Access Protocol, IMAP, v4
- [RFC 1939](http://www.ietf.org/rfc/rfc1939.txt) - Post Office Protocol, POP, v3

### Message encoding

- [RFC 2047](http://www.ietf.org/rfc/rfc2047.txt) - MIME (Multipurpose Internet Mail Extensions) Part Three: Message Header Extensions for Non-ASCII Text
   - RFC 2045 describes a mechanism for denoting textual body parts which are coded in various character sets, as well as methods for encoding such body parts as sequences of printable US-ASCII characters.  This memo describes similar techniques to allow the encoding of non-ASCII text in various portions of a RFC 822 [2] message header, in a manner which is unlikely to confuse existing message handling software.  

  
- [RFC 2184](http://www.ietf.org/rfc/rfc2184.txt) - MIME Parameter Value and Encoded Word Extensions: Character Sets, Languages, and Continuations  

   - a means to specify parameter values in character sets other than US-ASCII
  - to specify the language to be used should the value be displayed, and
  - a continuation mechanism for long parameter values to avoid problems with header line wrapping.

### IMAP extensions

- [RFC 2177](http://tools.ietf.org/html/rfc2177) - IMAP4 IDLE command  

 This command allows IMAP servers to notify connected clients when a new message has arrived (rather than having the clients poll for new messages).
- [RFC 4314](http://tools.ietf.org/html/rfc4314) - IMAP4 Access Control List  

 This extension is used to create public folders shared among several folders. The document specifies for example how access is granted by an IMAP client.
- [RFC 2087](http://tools.ietf.org/html/rfc2087) - IMAP Quota extension  

 With this extension, IMAP cients can check how much of their server disk quota has been used and how much is available.
- [RFC 5256](http://tools.ietf.org/html/rfc5256) - SORT and THREAD Extensions  

 The SORT extension provides server side sorting capabilities. This adds performance, since IMAP clients does not have to download all messages in a mailbox to be able to sort them. The Thread extension is not implemented in hMailServer.  

  

### Other

- [RFC 4871](http://tools.ietf.org/html/rfc4871) - DKIM specification  

 DomainKeys Identified Mail (DKIM) defines a domain-level authentication framework for email using public-key cryptography and key server technology to permit verification of the source and contents of messages by either Mail Transfer Agents (MTAs) or Mail User Agents (MUAs).

 
