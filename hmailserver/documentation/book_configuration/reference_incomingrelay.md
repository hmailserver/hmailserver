---
id: 428
title: "Incoming relay"
slug: reference_incomingrelay
parent: book_configuration
index: 0
is_book: false
updated: 2016-07-20
---

## Overview

Unless the Incoming Relays section is populated with a matching address, hMailServer assumes that itself is the MX for your domain.

When hMailServer performs anti-spam tests on a message it will use the connecting IP address to determine where the message is arriving from.

When hMailServer receives an email from a MX backup server, hMailServer can't use the senders TCP/IP address since this is the IP address of the backup server. If you add the MX backup servers IP address as an incoming relay, hMailServer will know that messages from this TCP/IP address is being forwarded and that the backup sender is not the original sender. hMailServer will then try to determine the original senders IP address by parsing the *Received* headers of the email message.

When hMailServer is not the MX, it must be made aware of those IP addresses that belong to your MX and the MX’s internal hand-off IPs. Set the Incoming Relays section to all the IPs that can appear in a mail header that belong to your MX and its internal hand-offs.

Most anti spam methods in hMailServer are performed before the actual message body has been delivered to hMailServer. hMailServer uses the HELO, MAIL FROM and RCPT TO commands to determine whether the message is likely to be spam. However, if a message arrives from a incoming relay, hMailServer has to wait until the entire message has been received before it can perform anti spam. In other words, using incoming relays will make the anti spam mechanisms more expensive, in terms of bandwidth and time.

Please note that incoming relays does not affect grey listing. Grey listing always takes place before the *Received* headers have been transmitted to hMailServer. You may want to add any incoming relays IP address to the grey listing white list, to prevent incoming relays from being grey listed.  

 
