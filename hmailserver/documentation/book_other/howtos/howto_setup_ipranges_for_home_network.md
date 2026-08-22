---
title: "Set up IP ranges for your home network"
slug: howto_setup_ipranges_for_home_network
parent: howtos
index: 0
is_book: false
---

## Background

IP range is one of the mechanisms hMailServer uses for security. Using IP ranges, you can give different access to different computers depending on their IP addresses. For example, you can enable IMAP for your network while people outside the network (e.g. on the Internet), can only access POP3.  

Normally, there should be no reason to add or remove IP ranges. You should not do this unless you are sure of what you want and how to achieve it. The default IP ranges created during installation should work for most users.   

  

The steps below describe a way to set up IP ranges for a home network with 3 computers. It is assumed that you have a home network where the computers have been assigned IP addresses starting with 192.168.1.

### Steps

1. Start up hMailAdmin.
2. Expand the settings node
3. Select IP ranges in the tree and click Add
4. Enter the following information:
   - Name: My home network
  - Priority: 25  

 When hMailServer determines what IP range to use, there might be several IP ranges matching the connected users. If so, the IP range with the highest priority is used. For this IP range to work properly, it needs to have higher priority than all other IP ranges.
  - Lower IP: 192.168.1.0
  - Upper IP: 192.168.1.255
  - Allow connections: SMTP, POP3 and IMAP
  - For the remaining settings, use the same settings as in the Internet IP range.

The above instructions are valid for 5.2 and later.  

  

![](images/howto_setup_ipranges_for_home_network.gif)
