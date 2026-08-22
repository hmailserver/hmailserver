---
title: "IPv6 support"
slug: details_ipv6
parent: book_other
index: 0
is_book: false
---

## Overview

hMailServer supports both IPv4 and IPv6. Most email servers on the Internet are using IP version 4.

### How does it work?

IPv6 support will be automatically enabled in hMailServer if it is enabled in the operating system. If you enable IPv6 in the operating system, you will need to restart the hMailServer service for hMailServer to become fully IPv6-aware.

In all dialogs in Administrator and WebAdmin where you can enter IP addresses, you can enter both IPv4 and IPv6 addresses. The underlying database storage can store both IPv4 and IPv6-addresses.

As an example, if you want to relay all outgoing email to a server in your network, you can specify one of the three values in the SMTP relay settings:

- The server host name
- The server IPv4 address
- The server IPv6 address

Similarly, when you configure hMailServer to listen or bind to a specific IP address, you can enter either an IPv4 address or an IPv6 address.

### Configuration

The default configuration of hMailServer does not listen to IPv6 addresses. If you want hMailServer to listen on IPv6 address, you must do two things. Note that these configuration modifications are only needed if you want hMailServer to listen on IPv6 addresses. If you only want to use IPv6 for example when delivering outgoing email, these changes are not needed.

#### Add a TCP/IP port.

In hMailServer Administrator, navigate to Settings, Advanced, TCP/IP ports. Add a new TCP/IP port and specify the IPv6 address you want to listen on. If you do not do this, hMailServer will not listen on any IPv6 address. The equivalent value to IPv4 = 0.0.0.0 for IPv6 is ::  

  

You will need to create a new TCP/IP port for each IPv6 port that your hMailsever will listen on, and set a protocol for each

#### Add IP ranges

Create a new IP range covering the IPv6 Internet  >> Lower IP="::", Upper IP = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"  

Create a new IP range covering "My computer" >> Lower IP=" ::1", Upper IP = "::1"  

Create other IP ranges to suit your environment.  

  

In the IP range, choose the same settings as in the already existing Internet IP range. If you do not do this, hMailServer will still accept incoming connections over IPv6, but it will immediately disconnect the clients since the clients IP address will not match any IP range.

### Limitations

The communication protocols used between Administrator / WebAdmin and hMailServer varies depending on your operating system configuration. Microsoft Windows 2003 supports DCOM over IPv6. Earlier versions of Windows may not support DCOM over IPv6. This is not really a limitation in hMailServer but limitations in the Windows operating system support for DCOM.

 
