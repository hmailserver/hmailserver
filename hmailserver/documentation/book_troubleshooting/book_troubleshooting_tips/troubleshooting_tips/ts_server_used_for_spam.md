---
title: "Server used for spam"
slug: ts_server_used_for_spam
parent: troubleshooting_tips
index: 0
is_book: false
---

## Background

It's a quite common problem that spammers use other peoples servers to deliver spam messages. hMailServer does not allow this by default, but if you have configured hMailServer incorrectly or used weak passwords for your e-mail accounts, it's rather easy for them to do this.

## How do I know for sure?

- First check the hMailServer delivery queue. If the queue contains a lot of messages from external to other external addresses, it's pretty safe to say that someone is abusing your server to send spam. So if you do not recgonize either the senders address or the recipients address, it's probably spam.

- Another method is to telnet relay-test.mail-abuse.org from the computer running hMailServer. The remote server will automatically connect back to the mail server running on your computer and execute some tests to check whether your computer can be used for spamming.

- Also check your data-folder. If this folder (not the sub folders) contains a lot of .eml files, say, more than 250, even though no one is currently sending email through your server, it's likely that someone is using your server to send spam.

- A fourth method is to use other open relay tests.

## What should I do?

First of all:

- Check your IP range settings and make sure that you require authentication. Typically, in the Internet IP range, hMailServer should be configured to require SMTP authentication for deliveries to external accounts.
- Never allow delivery from external to external accounts. Check all your IP ranges.

If you are sure that external to external is not permitted in any of your IP ranges, and you have ensured that you require authentication for deliveries to external accounts, it likely means that the spammer is using one of the accounts on the server to send spam. In most cases when this happens, the user has selected a weak password such as "test" or "secret". So you need to determine which accounts send you the email messages:

1. Enable Application and SMTP logging. Only enable these two. Enabling more will make it harder to analyze the logs
2. Wait a few minutes so that the spammer sends another email through your server. Monitor the queue of "Undelivered messages" and note the sender and recipient address of one of the spam messages.
3. Search for the senders address in the hMailServer log.

The log will now contain info on who is trying to send the spam message. You will see a section which looks like this: (the first columns are left out to make text easier to read)

> ```
> "<spammers IP address>"    "SENT: 220 <your-host-name> ESMTP"
> "<spammers IP address>"    "RECEIVED: EHLO <spammers-host-name>"
> "<spammers IP address>"    "SENT: 250-hmailserver[nl]250-SIZE[nl]250 AUTH LOGIN"
> "<spammers IP address>"    "RECEIVED: AUTH LOGIN"
> "<spammers IP address>"    "SENT: 334 VXNlcm5hbWU6"
> "<spammers IP address>"    "RECEIVED: dGVzdEB0ZXN0LmNvbQ=="
> "<spammers IP address>"    "SENT: 334 UGFzc3dvcmQ6"
> "<spammers IP address>"    "RECEIVED: ***"
> "<spammers IP address>"    "SENT: 235 authenticated."
> "<spammers IP address>"    "RECEIVED: MAIL FROM: <sender-address@example.com>"
> "<spammers IP address>"    "SENT: 250 OK"
> "<spammers IP address>"    "RECEIVED: RCPT TO: <recipient-address@example.com>"
> "<spammers IP address>"    "SENT: 250 OK"
> "<spammers IP address>"    "RECEIVED: DATA"
> "<spammers IP address>"    "SENT: 354 OK, send."
> "<spammers IP address>"    "SENT: 250 Queued (0.000 seconds)"
> "<spammers IP address>"    "RECEIVED: QUIT"
> "<spammers IP address>"    "SENT: 221 goodbye"
> ```

The interesting lines here are the following section:

> ```
> "<spammers IP address>"    "RECEIVED: AUTH LOGIN"
> "<spammers IP address>"    "SENT: 334 VXNlcm5hbWU6"
> "<spammers IP address>"    "RECEIVED: dGVzdEB0ZXN0LmNvbQ=="
> "<spammers IP address>"    "SENT: 334 UGFzc3dvcmQ6"
> "<spammers IP address>"    "RECEIVED: ***"
> ```

These lines contains an encoded version of the login information the spammer used. The text is encoded using Base64 and can be decoded using any [Base64 decoder](http://www.opinionatedgeek.com/dotnet/tools/Base64Decode/Default.aspx). In plain text, it reads

> ```
> "<spammers IP address>"    "RECEIVED: AUTH LOGIN"
> "<spammers IP address>"    "SENT: 334 Username:"
> "<spammers IP address>"    "RECEIVED: test@test.com"
> "<spammers IP address>"    "SENT: 334 Password:"
> "<spammers IP address>"    "RECEIVED: ***"
> ```

So in this example, the spammer authenticated using test@test.com, and then sent an email from sender-address@example.com. Now when you know what account the spammer abused, you can either disable the account, or choose a better password.
