---
id: 38
title: "Blocked ports"
slug: ts_blocked_ports
parent: troubleshooting_tips
index: 0
is_book: false
updated: 2008-02-17
---

## Troubleshooting blocked ports

If you can't receive e-mail from other servers, and you are sure that your MX records are defined correctly, try the following:  From outside your network, telnet your computer on port 25. If this fails, then there is some software or hardware blocking connection to your port 25.

Possible reasons:

- You have not opened up port 25 in your software firewall.
- A router / switch in your network which receives the traffic from the Internet does not forward the traffic to the computer where hMailServer is running.
- Your ISP (Internet service provide) is blocking port 25 for incoming traffic.

Windows XP SP2's firewall is turned on by default, and may block incoming connections. Earlier versions of XP also have a built-in firewall, but it's turned off by default. If you are sure that you have not blocked port 25 in your firewall, but you still cannot connect from the outside of your network, you should ask your ISP whether they have blocked port 25. Some ISPs block server ports (such as e-mail, FTP and HTTP) because they don't want their users to set up servers.

## Workarounds

If your ISP has blocked port 25, [there are some alternatives](?page=ts_using_nondefault_smtp_port).
