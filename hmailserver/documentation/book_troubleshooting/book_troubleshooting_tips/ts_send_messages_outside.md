---
title: "Sending messages"
slug: ts_send_messages_outside
parent: book_troubleshooting_tips
index: 10
is_book: false
---

## Port 25 blocked for outbound traffic

To deliver an outbound message, all email servers must connect to the recipient's email server on TCP/IP port 25. Some Internet Service Providers (ISPs) block this port to reduce spam. This causes a problem for hMailServer, since it can't then connect to another email server to deliver mail.   

  

To test whether your ISP has blocked port 25, you can try manually connecting to our mail server, by doing the following:

1. Select *Start->Run*
2. Type `telnet mail.hmailserver.com 25` (Exactly this text. You should not replace mail.hmailserver.com with the name of your own email server)
3. Click *OK* to start the telnet session

If you can connect, then your ISP has not blocked outgoing traffic on port 25. If you can't, then your ISP may have blocked traffic on port 25. This means that the only way to send email out of your ISP's network is through their own mail servers. You may try relaying your outgoing messages throught your ISP's SMTP servers. If you're unsuccessful, contact your ISP and request that they either allow relaying through their SMTP servers or open port 25 for you.

## Incorrect DNS settings on local computer

Incorrect DNS settings on the local computer are known to cause delivery problems for hMailServer. If the DNS settings in incorrect, you normally get a bounce message saying that no mail servers could be found for the recipients. Windows uses different sets of DNS settings, so even if for example your web browser works properly, the DNS settings may be wrong. To confirm that your DNS settings are correct, follow these steps:

1. Select *Start->Run*
2. Type nslookup
3. Click OK
4. Type *set type=mx* and press Enter
5. Type gmail.com and press Enter  

 Now you should see a list of GMails email servers. If you don't see this list, it means that your DNS settings are not correct.

## Spam in delivery queue

If you misconfigure hMailServer, it will be possible for others to send spam through your server. One easy way to determine whether spam is being sent through your server is to open up the hMailServer data directory. The root of the data directory contains the hMailServer delivery queue. If there are a large number of messages in the delivery queue, it's likely that someone is sending spam through your server. Ensure that you've disabled *External to external* for all IP ranges in the settings in hMailAdmin.
