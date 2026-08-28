---
title: "About message recipients"
slug: information_recipients
parent: book_other
index: 0
is_book: false
---

## Background

For an email message, there are two separate lists of message recipients. There is one list which defines who the message is to be delivered to, and there's a list which is displayed to the recipients in their email clients. 

The list which defines who the message is going to be delivered to, the SMTP envelope recipients, does not have to match the recipients displayed in email clients (the MIME header recipients).

For example, if you send an email to userone@example.com and put usertwo@example.com in the BCC field, both users will be in the SMTP envelope recipient list, but only userone@example.com will be included in the recipient fields in the MIME header. This has the effect that the email server knows where to deliver the message, but the recipients won't see usertwo@example.com in their clients.

The above information applies in a very similar way to sender addresses. An email message has one SMTP envelope sender address and one address in the From field in the MIME header. Those two addresses does not have to match. You can send an email from your own email address but with someoneelse@example.com in the From header, hence making it appear to the recipient that the email was sent from someoneelse@example.com.

This needs to be taken into account when setting up rules, scripts and during troubleshooting.

Spammers sometimes send messages with MIME header recipients not matching the SMTP envelope recipients. This can result in the illusion that you received an email which was not being sent to you. You could be the SMTP envelope recipient - the actual recipient - even if you are not listed in the MIME headers.

## Examples

### Example 1:

The fact that you see your own email address in the From field in your email client does not mean that the email was sent from your account. It merely means that the sender of the email wanted your address to be in the From address.

### Example 2:

When you set up a rule to forward an email message, the forwarded message will have the same *To header* but a different *Recipient list*. This is because the recipients of the message changes, but not the actual content of the message.

### Example 3:

If you receive a spam message addressed to someone-else@example.com, this means that the spammer wanted to send an email to you, but he wanted it to appear as if the message was sent to someone-else@example.com.
