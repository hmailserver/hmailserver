---
id: 429
title: "Auto-ban"
slug: reference_autoban
parent: book_configuration
index: 0
is_book: false
updated: 2022-09-13
---

## Overview

It is a common problem that people use weak passwords which spammers manages to figure out using dictionaries. Using the auto-ban functionality, hMailServer can automatically disconnect these spammers and reduce the risk of your server being used to send spam.

If *Max invalid logon attempts* are made from a specific IP address within *Minutes before reset*, the connecting clients IP address will be auto-banned for *Minutes to auto-ban* minutes.  

  

*Invalid logon attempts* only occur once a connection has been made and has been accepted by your hMailserver. Connections rejected or blocked due to IP range restrictions (including existing Auto-ban entires), or due to the INI setting 'DisableAuthList" will NOT be Auto-banned.  

  

Remember that if multiple IP ranges apply, the IP range with the highest priority (highest priority is highest number) will be used. If you have a higher priority IP range for your web-mail server, and the same IP exists as an Autoban, the higher priority IP range will be used. In this case further matching Auto-ban entires may be created.

When a user is banned, an IP range matching the user is automatically created. In this IP range, all protocols are de-selected which has the effect that the user will no longer be able to connect. The new IP range will have an expiry date set which means that it will be removed when *Minutes to auto-ban minutes have passed.*

### IP range naming

When a client is banned, an IP range matching his IP address will be created. This IP range will have the following name:

<p style="margin-left: 40px;">Auto-ban: username (random)</p>

Where "username" will be replaced with the username he is trying to log on with, and "random" is replaced with a 9 character random string.  

  

In hMailServer you can not have multiple IP ranges with the same name. This is the reason the random string is included.

### Potential problems

The Auto-ban functionality blocks IP addresses. If too many invalid logon attempts are made from the same IP address, the IP address will be banned. If you are using a webmail system, all connections to hMailServer from that webmail system will come from the same IP address. If too many invalid logon attempts are made on that webmail system, the IP address the webmail system is connecting from will be blocked.

To solve this problem, you can whitelist the webmail system. A workaround to this problem is to add a new IP range matching the shared IP address and give this IP range higher priority than any IP range added by the auto-ban functionality. The IP ranges added by auto-ban is given the priority 20, so if your own IP range has priority 25 it will take precedence.

## Settings

If *Max invalid logon attempts* are made from a specific IP address within *Minutes before reset*, the connecting clients IP address will be auto-banned for *Minutes to auto-ban* minutes.

 
