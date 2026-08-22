---
title: "Configuration tutorial"
slug: basic_configuration
parent: book_quickstartguide
index: 10
is_book: false
---

## Overview

This page describes the basics of configuring hMailServer. It does not include information on how to set up virus scanners or spam protection. If you are unsure about how hMailServer works, you should read this page before configuring the server.

## Connecting to hMailServer

1. From the Start menu, select hMailServer Administrator  

 Now the hMailServer Administrator - Connect dialog is opened. This dialog allows you to connect to different hMailServer services.
2. Double-click on the "localhost" host name to connect to the hMailServer instance running on localhost.
3. In the password dialog, specify the password you specified during the installation of hMailServer - the main hMailServer administration password, and then click OK
4. Now hMailServer Administrator is started.

## Domains & Accounts

Every hMailServer domain should be connected to an internet domain. Say that you're the owner of the domain `something.com`, then you should add `something.com` as a domain in hMailAdmin:

1. Start hMailAdmin.
2. Click *Add domain*.
3. Enter `something.com` as domain name.
4. Click *Save*

The next step is to add accounts to your server. The normal setup is to have one account per email address you want to be able to send and receive email from. If you want the addresses `webmaster@something.com` and `info@something.com`, simply add this to hMailAdmin:

1. Start hMailAdmin
2. Expand the *Domains* node in the tree to the left
3. Click on the domain *something.com*
4. Click *Add account*
5. Enter `webmaster` as the account address, set the password and click *Save*
6. Click on the domain *something.com* in the tree to the left
7. Click *Add account*
8. Enter `info` as the account address, set the password and click *Save*

## Specifying public host name

For an email server to work properly, it needs to know its public name on the Internet. This is normally something like `mail.something.com`. Since there is no good way for software to automatically detect the public host name of the computer where it is running, you need to tell hMailServer what public hostname to use. While it's possible to run hMailServer without telling it its public hostname, some email servers will reject email from you if you don't specify it.

1. Start hMailAdmin
2. In the tree to the left, go to *Settings* -> *Protocols* -> *SMTP*
3. To the right, the SMTP settings are now shown. Click on the *Delivery of e-mail* tab.
4. Under *host name*, enter the public hostname of the computer where hMailServer is running.
5. Save the change

## Specifying SMTP relayer

Some internet service providers (the people that offer you the internet connection) block outgoing traffic on port 25. Since outgoing traffic on port 25 is required for email to work properly, you either need to convince your ISP to open up port 25 for you (if it's not open), or you need to configure hMailServer to forward all email through your ISP's email server.   

To determine whether port 25 is blocked, try typing `telnet mail.hmailserver.com 25` on your command line. If you can connect, port 25 is not blocked; if you can't, port 25 is blocked.   

If you configure your ISP's email server as SMTP relayer, hMailServer will deliver all outgoing email to your ISP's email server, which in turn will deliver it to the correct destination. Since it's almost always possible to send email through your ISP's email server, this is a workaround if port 25 is blocked.

1. Start hMailAdmin
2. In the tree to the left, go to *Settings* -> *Protocols* -> *SMTP*
3. To the right, the SMTP settings are now shown. Click on the *Delivery of e-mail* tab.
4. In the *SMTP Relayer* field, enter the host name of your ISP's email server, along with your credentials on that server in case it demands authentication.
5. Save your changes

Please note that you should **never** specify `localhost`, `127.0.0.1`, or your own hostname as the *SMTP Relayer*, since that would mean that hMailServer would try to forward messages to itself. That would result in an infinite loop. Also, you should leave this field empty if your ISP is not blocking port 25.

## Configuring IP ranges

IP ranges are used in hMailServer to specify who should be allowed to send email through your server. For example, you can use the IP ranges to configure hMailServer such that only computers in your local network are allowed to use the server to send email. By default, hMailServer comes with 2 different IP ranges. These default IP ranges should be sufficient for almost all users. Unless you're using old email clients with a lack of features, you should never have to modify these. Do not modify them unless you are absolutely sure what you want to achieve using IP ranges, and how to achieve it.

## Advanced Configuration

There are many guides on the [forum](https://www.hmailserver.com/forum/viewforum.php?f=21) that detail advanced configuration, from adding SpamAssassin to using Self-Signed SSL Certificates.

## Troubleshooting

In most cases where users report problems with hMailServer installation and configuration, the problem is not in the hMailServer set-up but with network settings, firewalls, DNS configuration and other external software. This tutorial covers only hMailServer set-up so it is strongly recommended that you read the [troubleshooting section](?page=troubleshooting_tips) in case you run into problem.
