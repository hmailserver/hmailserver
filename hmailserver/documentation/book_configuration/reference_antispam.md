---
title: "Anti spam"
slug: reference_antispam
parent: book_configuration
index: 0
is_book: false
---

## Overview

hMailServer has a number of built-in spam protection methods. These work by checking the sender of email messages, the content of the message and the way the message is delivered to hMailServer. For example, if the email message contains links to spammer web pages, or is sent from an address which is known to send spam, the message may be classified as spam. A complete list of built-in spam protection methods [can be found here](?page=details_antispam_methods).

### Spam scoring

Each of the tests performed by hMailServer generates a Spam score. If a specific spam test then tells hMailServer that the message is spam, a configured - or calculated - spam score is added to the message. When all the spam tests are run, hMailServer compares the total spam score of the message to two different thresholds set up in hMailServer.

The first threshold is the Mark threshold. If the total spam score for the message reaches the Mark spam threshold, the subject of the email message is modified to indicate that the message contains spam. Using marking of messages, users can easier find and delete the spam message, or you as a server administrator can set up Rules to move the spam messages to a specific IMAP folder, or forward them to a specific folder.  

  

The second spam threshold is the Delete threshold. If the message reaches this threshold, the message is rejected to the sender with a delivery failure error for SMTP delivered mail, and is deleted without a rejection notice for mail retrieved via POP3 download.

### When is spam protection run?

hMailServer tries to determine whether the message is spam as early as possible in the communication with the email sender. The earlier the detection is made, the less resources from your server will be required to handle the email message. Another benefit with early detection is that hMailServer can more easily tell the sender that the message is rejected due to spam protection and the sender can be notified.

If an email message is delivered to hMailServer using SMTP, hMailServer does spam protection in the following stages:

- After the RCPT TO command. When the recipient of the message has been specified, hMailServer runs spam protection.
- After the DATA command. When the entire message has been transmitted to hMailServer, hMailServer runs spam protection on the message content.

If hMailServer downloads messages from an external account, spam protection is run before the message is saved in the account folder.

### Which messages are scanned?

hMailServer scans all messages which are delivered to use accounts, assuming the following is met:

- The message is delivered to hMailServer by SMTP, or downloaded from an external account using POP3.
- At least one spam protection method is enabled in the Anti-spam setting.
- The sender IP address or domain is not white listed using a white listing record.
- The senders IP address matches an IP range where Anti-spam is enabled.

## Settings

### Spam mark threshold

When hMailServer runs spam protection, each spam protection mechanism gives a score. If the total score of the message reaches this value - but stays below Spam delete threshold, the message will be marked as spam.

<div class="indented">
<ul>
    <li>Add X-hMailServer-Spam - Adds a X-hMailServer-Spam MIME header to the email message.</li>
    <li>Add X-hMailServer-Reason - When enabled, hMailServer will add a message header which contains information on why hMailServer considered the email to be spam.</li>
    <li>Add to message subject - Using this setting, you can specify a text that hMailServer should prepend to the message subject. In combination with rules, spam messages can be moved to specific IMAP folders.</li>
</ul>
</div>

### Spam delete threshold

When hMailServer runs spam protection, each spam protection mechanism gives a score. If the total score of the message reaches this value the message will not be delivered to its recipients.

If the message arrived via SMTP, the sender will receive a rejection code.

If the message arrived via External POP3 Account download, then the message is simply deleted, with no rejection message returned to the sender.

### Maximum message size to scan (KB)

If the size of an email message exceeds this size, hMailServer will not scan it for spam. In most cases, spammers sends small messages to save bandwidth so scanning large messages serves no purpose in most cases. Scanning large  messages for spam may require a lot of CPU processing.

### Add Authentication-Results header

When this option is enabled, hMailServer adds an Authentication-Results header to incoming messages, listing the DMARC, SPF and DKIM results for the message. Any Authentication-Results header already in the message which claims to come from this server is removed first, so a sender cannot fake a result. Headers added by other servers are kept.

## Spam tests

### Use SPF

<div class="indented">Select to enable spam protection using SPF. The SPF record for the sending domain will be checked. Only a hard fail will score.</div>

<div class="indented">A hard fail happens when the mechanism that matches the sending IP address is prefixed with '-' (deny) - most commonly a trailing '-all' (deny all others), but it can also be an earlier, more specific mechanism such as '-ip4:1.2.3.4/32'. If the domain has no SPF record, or the matching mechanism is unprefixed / '+' (pass), '~' (softfail, intended only for testing) or '?' (neutral), the test will pass and will not score.</div>

<div class="indented"> </div>

<div class="indented">See <a href="http://www.openspf.org/">www.openspf.org</a> for more detail about SPF. You can set SPF records for your own domains as part  of the DNS records.</div>

<div class="indented">
<h3>Check host in the HELO command</h3>
<div class="indented">Turn on this option if you want hMailServer to check the host name that clients has specified in the HELO command. According to the SMTP specification, the host given in the HELO command should match the IP of the client.  Enabling this may stop spam, but is also a violation of the SMTP RFC - if you have configured your server to delete spam messages. If you have configured your server to deliver spam messages but modifying the Subject header, it is not a violation of the SMTP RFC. Technically, hMailServer checks the A record for the given host to see if it matches the IP address of the connecting client.</div>
</div>

### Check that the sender has DNS-MX records

<div class="indented">If you enable this option, hMailServer will check that the senders domain has valid MX records in the DNS. If not, the spam score of this test will be added to the total spam score for the message. Please note that there is no requirement that domains should have MX records. It's perfectly valid for a domain not to have MX records and still send email messages. While most domain owners set up MX records, far from all do it. This means that you should expect quite many false positives using this spam test.</div>

<div class="indented">
<h3>Verify DKIM-Signature header</h3>
<div class="indented">If you enable this option, hMailServer will look for a DKIM-Signature header in every incoming message. If a header is found, hMailServer will verify that the message content matches the signature. If it does not, the spam score of this test will be added to the total spam score for the message.</div>
</div>

<div class="indented">
<h3>Use DMARC</h3>
<div class="indented">If you enable this option, hMailServer will check the DMARC policy of the domain in the message From header. DMARC requires that a message passes SPF or DKIM, and that the domain which passed matches the From header domain. If neither does, the spam score of this test is added to the total spam score for the message.</div>

<div class="indented">If the From domain does not publish a DMARC policy, or the message does not have exactly one From address, no DMARC check is made.</div>

<div class="indented">This test is run after the message has been transmitted, since the From header and any DKIM signatures are part of the message.</div>

<h3>Honor policy published by sender domain</h3>
<div class="indented">If you enable this option, hMailServer applies the policy the sender domain publishes in its DMARC record when the check fails, regardless of the spam score. A policy of <em>reject</em> makes hMailServer reject the message, and a policy of <em>quarantine</em> makes hMailServer mark the message as spam. A policy of <em>none</em> only adds the spam score. The <em>pct</em> tag of the DMARC record is respected, so a domain owner can roll a policy out gradually.</div>

<div class="indented">If this option is disabled, a failed DMARC check only adds the spam score.</div>
</div>

<div class="indented">
<h3>Check rDNS/PTR record</h3>
<div class="indented">If you enable this option, hMailServer will look up the PTR (reverse DNS) record for the connecting IP address, and check that one of the addresses that hostname resolves back to matches the connecting IP address. If no matching PTR record is found, the spam score of this test will be added to the total spam score for the message. This option is disabled by default.</div>
</div>

## SpamAssassin

<div class="indented">Use these options if you want hMailServer to integrate with an existing SpamAssassin installation.</div>

### Host name

<div class="indented">This is the host name of the SpamAssassin server hMailServer should connect to. If SpamAssasin is running on the same computer as hMailServer, the value should be "localhost" (without quotes).</div>

<div class="indented">
<h3>TCP/IP port</h3>
<div class="indented">Specify the TCP/IP port the SpamAssassin server is listening to. By default, SpamAssassin listens on port 783.</div>
<div class="indented">
<div class="indented">
<h3>Use score from SpamAssassin</h3>
<div class="indented">If this option is selected, hMailServer will use the spam score given by SpamAssassin and add it to the hMailServer spam score. If the option is de-selected, hMailServer will use the score specified in the Score text box. The score from SpamAssassin will only be used where the SpamAssassin 'mark' threshold is reached.</div>
<h3>Test</h3>
<div class="indented">Click this button if you want hMailServer to test to connect to SpamAssassin. After hMailServer has done this, the result of the test will be shown to you.</div>
</div>
</div>
</div>

## Related information

### IP ranges

It is possible to turn off hMailServer's spam protection functionality per IP range. For example, one normally does not have to check for spam on one's local network. Consult the IP range documentation for more information on this.

### White listing

It's possible to use [white listing records](?page=reference_whitelisting) to disable spam protection for email messages sent from specific email or IP addresses. This is a more flexible solution than to use IP ranges.

## Advanced

### Other methods

<div class="indented">
<ul>
    <li><a href="?page=reference_dnsblacklist">DNS blacklist reference</a></li>
    <li><a href="?page=reference_surblserver">SURBL reference</a></li>
    <li><a href="?page=reference_greylisting">Grey listing reference</a></li>
</ul>
</div>

### Related

<div class="indented">
<ul>
    <li><a href="?page=reference_whitelisting">White listing</a></li>
</ul>
</div>
