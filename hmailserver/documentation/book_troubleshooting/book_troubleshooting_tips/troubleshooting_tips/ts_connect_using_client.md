---
id: 41
title: "Connect using client"
slug: ts_connect_using_client
parent: troubleshooting_tips
index: 0
is_book: false
updated: 2010-06-18
---

## General suggestions

If you cannot connect to hMailServer using your email client, the most likely causes are:

- You have not entered the correct e-mail server host name in the account settings in your email client.
- The host name you have entered in your email client does not resolve to the IP address of the computer where the email server is running.
- A firewall is running which blocks traffic to hMailServer. This can be both software and hardware firewalls. Very often server administrators have not opened up the relevant ports (typically 25, 110 and 143) in the Windows XP firewall.
- Loopback connections is not enabled in the router, so even though you connect to the correct public IP address and the firewall is opened up, your router won't forward the connection to the mail server since you are connecting from inside the network.
- hMailServer is not listening on the TCP/IP ports. In most cases, this occurs if another email server is running on the same computer as hMailServer. Often server administrators have choosen to install Microsoft IIS SMTP server and then forgotten about it. Telnet your server on the ports 25, 110 and 143 to check whether another server is listening on them.
- Your IP address has been banned. Check whther Auto-ban has been enabled in hMailServer. Also check whether there's an auto-ban IP range matching your IP address.

## Problems connecting from the computer where hMailServer is installed

Check that you can ping the computer on which hMailServer runs. Check that the mail server has really started, by telnetting the server on port 25 and 110. In hMailAdmin, locate the IP ranges settings and make sure that you have not defined any IP ranges that make it impossible to send email. You can specify the maximum number of simultaneous connections in hMailAdmin. The default value is 0 (unlimited). Make sure that you have set it to 0 or to a high value. If these tips don't help, do the following:

1. Stop the server.
2. Turn on Application, Debug, SMTP, POP3 and IMAP logging.
3. Remove any old logs from the log directory (C:\Program Files\hMailServer\Logs)
4. Start the server
5. Try to connect with your email client.
6. Create a new topic in the hMailServer forum and post your log there. Before you do, check the log to make sure that your password isn't in it.

## Problem only when connecting from another computer

Make sure that firewalls aren't blocking ports. For example, many ISPs block port 25 to any outgoing traffic, and so might block connection to your server.
