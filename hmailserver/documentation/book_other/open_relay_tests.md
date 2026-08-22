---
title: "Open relay tests"
slug: open_relay_tests
parent: book_other
index: 0
is_book: false
---

As soon as you finish installing and configuring hMailServer, you should run some open relay tests. It ensures that your server will not be used for spamming.[](http://members.iinet.net.au/~remmie/relay/)

- [http://www.aupads.org/test-relay.html](http://www.aupads.org/test-relay.html)
- [http://www.dnsgoodies.com/](http://www.dnsgoodies.com/)

## False positives

In some cases, open relay tests may return false positives. This means that the open relay test claims that your server may be an open relay even though it's not. In other words, the server may give you a warning even though you have nothing to worry about.

### Default domain and catch-all address

If you specify a default domain in hMailServer, this domain name will be appended to all email addresses which are missing a domain name (the setting is located in hMailServer Administrator under Settings->Advanced). If you specify the default doman example.com, and someone tries to send an email to webmaster, hMailServer will assume the messge should be sent to webmaster@example.com.  

  

If you specify a catch-all address for a domain, hMailServer will accept all email sent to this domain. If the recipient address does not match an account, the message will be forwarded to the catch-all address.

Combining the default domain setting with the catch-all address setting may lead to false positives. If you have enabled both settings, and someone connects to your server and wants to deliver an email to <any-random-string>, hMailServer will accept the delivery. hMailServer will first append the default domain to the address - <any-random-string>@example.com. After that, it will try to find the account <any-random-string>@example.com. If this fails, the message will still be delivered to the catch-all account.

 
