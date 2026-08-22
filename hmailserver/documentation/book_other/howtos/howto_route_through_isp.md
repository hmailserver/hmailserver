---
title: "Route all outgoing email through another server"
slug: howto_route_through_isp
parent: howtos
index: 0
is_book: false
---

## Background

Many ISPs (companies providing users a connection to the Internet) block outgoing traffic on port 25. This makes it impossible for hMailServer to connect directly to other SMTP servers on the Internet. A workaround for this is to route all your outgoing email through your ISP's mail server.

Also, for different users, people may want to route all outgoing email through a 3'rd party service such as gmail.

## Steps

1. Start up hMailServer Administrator
2. Go to the Settings ⇒ Protocols ⇒ SMTP
3. Select the *Delivery of e-mail* tab.
4. Specify the following settings:  

 SMTP relay server - Your ISP's email server hostname or IP address.  

 TCP Port - The TCP/IP port your ISP's server is listening on, usually 25 (unless SSL is used).
5. If your ISP's server requires authentication, select the *Server requires authentication* and enter username and password.

The above instructions are valid for 3.4 and later.  

  

![](images/howto_route_through_isp.gif)
