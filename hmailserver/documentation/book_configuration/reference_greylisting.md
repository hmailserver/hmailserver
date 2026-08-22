---
id: 186
title: "Grey listing"
slug: reference_greylisting
parent: book_configuration
index: 0
is_book: false
updated: 2017-01-08
---

## General

Grey listing allows you to prevent spam by temporarily rejecting email to your server. Grey listing benefits from the fact that properly configured email servers will try to resend messages later, while spammers normally will give up immediately if your server rejects an email.

When a sender tries to deliver a message for the first time to your server, hMailServer will save the senders IP adderss, the senders email address and the recipient email address. This information is called a *triplet*. hMailServer will reject the message and kindly ask the sending server to retry later. The next time the sending server tries to deliver an email which matches the triplet, hMailServer will accept the message.

Spam messages which are stopped by grey listing are not counted in the Status page in hMailServer Administrator. Also, even if you configure hMailServer to deliver spam messages but modify header, messages rejected by grey listing will not be delivered due to how the grey listing mechanism work.

PLEASE NOTE That greylisting needs to be enabled in Settings >> AntiSPAM >> Greylisting, AND ALSO enabled in Domains >> [Domain Name] >> Advanced.

This way greylisting can be enabled on a domain by domain basis.

### Minutes to defer deliver attempts

<div class="indented">Specify how many minutes hMailServer should wait before accepting a message.</div>

<div class="indented">
<h3>Days before removing unused records</h3>
If hMailServer temporarly rejects a message, but the sender does not try to resend the message, hMailServer will remove the triplet after the number of days specified.<br>
<h3>Days before removing used records</h3>
<div class="indented">Using this setting, you can specify how long triplets should exist in hMailServer before being removed. The number of days are counted from the date when the triplet was last used. For example, if a triplet is created on day 1, and re-used on day 5, and this setting is set to 10, the triplet will be removed 15 days after it was created.</div>
<div class="indented">
<h3>Bypass Greylisting on SPF Pass</h3>
<p>The downside with greylisting is that it causes delays for legitimate email messages. Even if the delay is not very long, it may sometimes cause end-user frustration. As a partial solution to this problem, you can enable "Bypass Greylisting on SPF Pass". Larger email providers such as Gmail and Hotmail publishes SPF records for their domain. If hMailServer receives an email from a domain which has published SPF records, and the IP address connecting to hMailServer is authorized to send from this domain, and this option is enabled, hMailServer will not perform grey listing.</p>
<h3>Bypass Greylisting when message arrives from A or MX record.</h3>
<p>Prior to running grey listing, hMailServer will do a DNS/A and DNS/MX lookup. If the connecting address is found in one of the records, the grey listing will be skipped if this option is enbabled.</p>
</div>
<div class="indented">
<h2>White listing</h2>
<p>E-mail servers which uses different IP addresses every time they try to send a message to hMailServer, and email servers which does not try to resend messages that has been temporarly rejected is not compatible with grey listing. You can add an IP address to such servers here. hMailServer will not use grey listing for the servers. Wildcards are supported in this list.</p>
</div>
</div>

 
