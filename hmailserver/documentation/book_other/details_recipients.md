---
title: "Resolving recipients"
slug: details_recipients
parent: book_other
index: 0
is_book: false
---

## Overview

This document describes how recipients are resolved in hMailServer. When a SMTP client delivers a message to hMailServer, it specifies a sender and a recipient. The recipient address specified by the client may not be the final recipient address, if aliases, domain aliases and routing is used. If these features are used, hMailServer may modify the recipient address. This document describes the process of this.

### Recipient rewriting

When a client specifies the recipient address (using the SMTP RCPT TO command), the following happens:

1. hMailServer checks if the recipient domain name matches a local domain name alias. If this is the case, the recipient address is rewritten to use the primary domain name. For instance, if the domain example.com has the alias sub.example.com, and a message is sent to someone@sub.example.com, the recipient address is rewritten to someone@example.com.  

  
2. If a domain exists matching the recipient domain:  

   

   1. If the domain is inactive, hMailServer returns an error to the sender.
  2. hMailServer attempts to locate an account matching the recipient address.  

     - If an account is found, hMailServer will deliver the message to the account.
    - If the account is inactive, hMailServer returns an error tot he sender.
  3. hMailServer attempts to locate an alias matching the recipient address.  

     - If an alias is found, hMailServer restarts the entire rewriting process, but this time with the address specified in the alias value.
    - If an alias if found, but it's inacitve, an error message is given to the sender.
  4. hMailServer attempts to locate a distribution list matching the recipient address
     - If a distribution list is found, hMailServer will add the recipients of the distribution list to the message, using the process described in this document.  

  
3. If no domain exists matching the recipient domain:  

   

   1. hMailServer tries to locate a route matching the recipient address. Note that when this is done, the original address is used. For instance, if the domain example.com has the alias sub.example.com, and a message is sent to someone@sub.example.com, hMailServer will try to locate a route for the domain someone@sub.example.com.

### Examples

**Simple delivery**

The domain example.com and account user@example.com. A client sends a message to user@example.com.

1. hMailServer will locate the domain example.com and confirm it's enabled.
2. hMailServer will locate the account user@example.com and confirm it's enabled.
3. hMailServer will deliver the message to user@example.com.

**Delivery using aliases**

The domain example.com and account user@example.com. An alias is set up pointing from someone@example.com at user@example.com. A client sends a message to someone@example.com.

1. hMailServer will locate the domain example.com and confirm it's enabled.
2. hMailServer will attempt to locate the account someone@example.com but will fail.
3. hmailServer will locate the alias someone@example.com and see that it's pointing at user@example.com.
4. hMailServer will locate the domain example.com and confirm it's enabled.
5. hMailServer will locate the account user@example.com and confirm it's enabled.
6. hMailServer will deliver the message to user@example.com.

**Delivery using routes**

No domain is set up, but a route with the name example.com is set up. A message is sent to the domain someone@example.com.

1. hMailServer will attempt to locate the domain example.com but fail.
2. hMailServer will locate the route example.com and confirm its enabled.
3. hMailServer will deliver to the address in the route.
