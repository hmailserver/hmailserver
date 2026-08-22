---
id: 255
title: "DNS error messages"
slug: reference_error_messages_dns
parent: book_error_messages
index: 0
is_book: false
updated: 2008-09-25
---

## DNS errors

### General information

hMailServer uses the Windows API to query the DNS server. hMailServer itself does not not ask Windows to use a specific DNS server. The DNS query is made using the Windows API function DnsQuery. Depending on what DNS error that has occured, DnsQuery return different values. If an error occurs, this error is listed in the hMailServer log.

### 1460, ERROR_TIMEOUT

DNS error 1460 means that there was a timeout when the DnsQuery was made. This can occur if the DNS server is rebooting or isn't available due to network problems.

### 9002, DNS_ERROR_RCODE_SERVER_FAILURE

Error number 9002 means that there was a DNS server failure.
