---
id: 48
title: "Non-default SMTP port"
slug: ts_using_nondefault_smtp_port
parent: troubleshooting_tips
index: 0
is_book: false
updated: 2005-11-22
---

Email sent between mail servers is always sent on port 25. This is not configurable and is the same for all mail servers. If you change the SMTP port to anything other than 25, other servers will not be able to send you email. Unless, you have set up some kind of portforwarding. 

If your ISP has blocked port 25 for incoming traffic in their firewall, or you can't use port 25 for some reason, there are only two things you can do:

- Change your ISP

or

- Use routing services. There are companies that offer to accept email for your domains on port 25 and then forward them to your hMailServer on a non-standard SMTP-port that is not blocked by your ISP.
