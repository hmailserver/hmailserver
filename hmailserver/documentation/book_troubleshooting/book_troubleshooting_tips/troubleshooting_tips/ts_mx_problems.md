---
id: 42
title: "MX problems"
slug: ts_mx_problems
parent: troubleshooting_tips
index: 0
is_book: false
updated: 2010-05-03
---

## What is MX?

MX is an abbreviation for Mail Exchanger and is used by all mail servers when they send an email. It's used by hMailServer when it sends an email to an external domain, and it is used by external domains when they send an e-mail to hMailServer. MX records are stored in DNS servers.

## How does it work?

Say, you want to send an email to webmaster@anexternaldomain.com. When you click Send in your mail client, the email is first submitted and stored in the hMailServer database. hMailServer then contacts the DNS server to obtain the IP of the server that is responsible for handling email for the domain, anexternaldomain.com. The DNS server responds with the IP address of the server. hMailServer connects to that IP and sends the email.   

  

The same process occurs the other way around. If you send an email via your ISP to hMailServer, your ISP will contact the DNS server to obtain the IP address of the server that is responsible for handling email for the domain, your-hmailserver-domain.com. The DNS server will respond with the IP address of your computer, the one on which your hMailServer is located. Your ISP's mail server then connects to that IP.

## So how does the DNS server know the IP of your computer?

You have to configure the your DNS to know it. The DNS server responsible for answering questions about your domain should have an MX record. If you own a domain, you should have access to some kind of control panel where you can specify the IP address for the MX record. If you do not have access to a DNS control panel, you should contact the company that registered your domain and ask them to add an MX record with your IP.   

  

It is not enough to only point your domain name to your IP address. The fact that your domain name resolves to your IP address doesn't make email work; the MX record must resolve to the correct IP address as well. One domain can have several MX records (if you, for example, for backup purposes, run multiple mail servers for a single domain).

##  

## How do I set up MX records?

Usually, where you register your domain, you can also add MX records via a web interface . If you do not have access to any web interface for your domain, you should contact the people who registered your domain, and ask them to set up an MX record for you. Setting up an MX record only takes a few minutes at the most. In fact, if you are lucky, they will do it for free.   

  

Nslookup is a command line program that basically queries DNS-servers for information. You can do all kinds of DNS lookups using nslookup. The text below shows how to do MX lookups. Start the command prompt.

```
Type nslookup -type=mx yourdomainname.com and press Enter. 
(Example: nslookup -type=mx hmailserver.com)
```

The response should look like this:

```
C:\Documents and Settings\your-username>nslookup -type=mx yourdomainname.com 
Server: your-isp-dns-host 
Address: your-isp-dns-ip 

Non-authoritative answer: 
yourdomainname.com MX preference = 10, mail exchanger = mail.yourdomainname.com
(the line above is your MX exchanger)

yourdomainname.com nameserver = your-dns-host-name 
yourdomainname.com nameserver = your-dns-host-name 
mail.yourdomainname.com internet address = your-mail-server-ip
ns2.yourdns.com internet address = your-dns-ip 
ns1.yourdns.com internet address = your-dns-ip
```

If the response does not include an MX record, then you have not defined any MX record for the domain.
