---
title: "Route"
slug: reference_route
parent: book_configuration
index: 0
is_book: false
---

## Overview

Routes specify how and where emails for specific domains should be delivered. Normally, hMailServer uses DNS lookups to determine where email should be delivered. Routes let you override this behaviour. For example,

- Routes enable you to deliver email for a specific domain through a specific server without using MX lookup.
- Routes enable your server to [act as a MX backup](?page=howto_act_as_mx_backup) for another server
- Using routes, you can configure hMailServer to forward email for specific accounts to other SMTP servers, even though the account domain exists in your hMailServer installation.

## Settings

### Domain

<div class="indented">The domain that this route should be applied to. The domain name is case insensitive.<br>
A * can be specified at either end of the domain or at both ends to achieve partial matches.<br>
Examples:</div>

-     mydomain.* will match all TLD versions of 'mydomain' (mydomain.com - mydomain.net - mydomain.co.uk ..)
-     *.mydomain.com will match all subdomains of 'mydomain.com' (sub1.mydomain.com - sub2.mydomain.com ..)
-     *domain* will match any domain that contains 'domain' (mydomain.com - yourdomain.com - domainhere.co.uk - mydomainhere.co.uk ..)

### Target SMTP host

<div class="indented">The host to which the emails that this route applies to will be delivered. This should typically be an internet host name such as mail.example.com.</div>

### Security

When a person tries to send an email where the recipient matches a route, hMailServer will use IP ranges and the security settings in the route to determine whether the delivery should be made. Using the two security settings, "*When sender matches route*" and *"When recipient matches route"*, you specify whether the sender and recipient should be seen as local or external. Combined with the settings in the IP ranges, this specifies when deliveries should be permitted, and whether or not SMTP authentication is required.

If you configure hMailServer to treat recipients matching the route as local addresses, and IP ranges does not permit deliveries to local addresses, the delivery will not be accepted by hMailServer. In the same manner, if you configure the recipient to be treated as external, and the IP ranges specifies that SMTP authentication is required for deliveries to external addresses, hMailServer will require SMTP authentication before accepting the delivery.

#### Examples:

- If you want external users (users on other email servers) to be able to send email to the route, select that "*When recipient matches route, treat recipient as local*". The default IP ranges in hMailServer permits delivery from external addresses to local addresses without any SMTP authentication.
- If you only want local users to be able to deliver messages to the route, select that the recipient should be treated as an external domain. By default, the IP ranges in hMailServer does not permit deliveries from external users to external recipients without the use of SMTP authentication.

### Addresses

<div class="indented">This setting lets you define which email addresses hMailServer should allow deliver to. For example, if you know that the only valid address on the target SMTP host is webmaster@domain.com, then you can add this email address to the list. In that case, hMailServer will only allow delivery to this specific address. This saves bandwidth usage. You can configure hMailServer to deliver to all addresses. If you do, hMailServer will forward any email addressed to the domain to the host specified in the route.</div>

### Delivery

<div class="indented">Use Number of retries to specify the number of times you want hMailServer to retry when sending to this domain. If the route is used for MX backup, you will most likely want to set this to a rather high value. Minutes between retries lets you specify the number of minutes between hMailServer's retries. For example, if the target SMTP host is down, there is no need to retry every minute. On the other hand, if the target SMTP host goes up, you don't want to wait 10 hours for the server to retry. Use Server requires authentication to specify user name and password if the target SMTP host requires authentication.</div>

## Examples

### Example 1 - Creating a new route

The following example explains how to configure all email for the domain `hmailserver.com` to go through `mx.hmailserver.com` instead of through the default server, `mail.hmailserver.com`:

1. Start hMailServer Administrator
2. Go to *Settings -> Protocols -> SMTP -> Routes*
3. Click on *Add*
4. In the *Domain* field, enter `hmailserver.com`
5. In the *Target SMTP host* field, enter `mx.hmailserver.com`.
6. Click on Save

Henceforth, all email sent to `hMailServer.com` will go through `mx.hmailserver.com`, instead of through the default server, `mail.hmailserver.com`.

### Example 2 - Route all email sent to the domain example.com to another server

This example explains how to route all email sent to the domain example.com to another server. In this example, the domain example.com is a "local" domain which we are hosting ourselves. This may be useful for example if we have updated the MX records for a domain but still want to receive email for the domain on the old IP address in case some SMTP sender has not updated their DNS cache.

1. Start hMailServer Administrator
2. Go to *Settings -> Protocols -> SMTP -> Routes*
3. Click on *Add*
4. In the *Domain* field, enter `example.com`
5. In the *Target SMTP host* field, enter the host name where email for this domain should be forwarded.
6. Click on Save

If hMailServer receives an email addressed to the domain example.com, and it cannot find the recipient in the local installation, it will now deliver the email to the host name you specified in the route. If hMailServer can find the recipient in the local installation, the email will be put in the local account instead - hMailServer only forwards email using routes if the recipients cannot be find locally.

### Example 3 - Setting up a server in DMZ forwarding to an internal email server

This example explains how to set up a hMailServer installation in a DMZ which forwards all incoming email to a backend server. One common reason for this set up is that you want anti spam and virus protection to be run outside your internal network. Another common set up is companies who are running MicrosoftExchange internally but do not want to expose this server to the Internet.

<ol>
    <li>Start hMailServer Administrator</li>
    <li>Go to <em>Settings -&gt; Protocols -&gt; SMTP -&gt; Routes</em></li>
    <li>Click on <em>Add</em></li>
    <li>In the <em>Domain</em> field, enter your domain name, for example <code>example.com</code></li>
    <li>In the <em>Target SMTP host</em> field, enter the host name of the internal email server.<span style="font-style: italic;"><br>
    </span></li>
</ol>

You should not add the domain to the server in the DMZ - only the route.

If hMailServer receives an email addressed to your domain name, it will forward the message to the backend server.  If hMailServer receives an email for an unknown domain, it will be rejected.

One problem with this configuration is that hMailServer does not know which recipients exists on the internal domain. Because of this, hMailServer will accept messages for any recipient matching your domain name and attempt to forward the message to your internal server. If the recipient does not exist in the internal server, a bounce-message will be created, notifying the sender that the recpiients address was invalid. A workaround to this problem is to add all valid recipients in the Route configuration, so that hMailServer knows what recipients are valid. If you do this, hMailServer will not accept the message from the sender and no bounce message will be sent.

 

 
