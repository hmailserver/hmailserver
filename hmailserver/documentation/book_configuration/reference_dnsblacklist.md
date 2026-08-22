---
title: "DNS blacklist"
slug: reference_dnsblacklist
parent: book_configuration
index: 0
is_book: false
---

## General

### DNS host

<div class="indented">The DNS host hMailServer should query when doing a DNS blacklist lookup.</div>

### Expected result

<div class="indented">The IP address that the DNS blacklist server will return if the senders IP address is found in the DNS blacklist. It's possible to use wildcards in the IP address.</div>

<div class="indented"> </div>

<div class="indented">There is some limited support for specifying multiple possible addresses. You can separate multiple addresses by using |, such as 127.0.0.1|127.0.0.2. In the last section of the IP address, you can use - to specify a range, as in 127.0.0.2-8. So if you specify 127.0.0.2-8|127.0.0.10-11, that will cover the IP adresses 127.0.0.2, 127.0.0.3 up until 127.0.0.8 and 127.0.0.10 and 127.0.0.11.</div>

### Rejection message

<div class="indented">The message that hMailServer will give to the sending client if an email is rejected.</div>

 
