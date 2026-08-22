---
title: "Whitelisting"
slug: reference_whitelisting
parent: book_configuration
index: 0
is_book: false
---

# General

hMailServer includes a number of anti-spam features. In some cases, you want certain senders to bypass all these. For example, a specific IP address may have been blacklisted by mistake, but you still want to be able to receive email originating from this IP address. Another example is that you may expect email from a specific sender, and you don't want to risk to loose this email if it's classified as spam.

To do this, you can add white-list records to the configuration. If hMailServer receives an email from a source matching one of these records, hMailServer will not try to determine whether the email is spam. To add a whitelist record, start hMailServer Administrator, and navigate to Settings, Spam protection, White listing. For every white list record, you can specify a description, an lower and upper IP address and an email address.

Before performing spam protection, hMailServer determines the IP address of the sender. When this has been done, hMailServer goes through the list of white list records. If a record matching the IP address is found, hMailServer checks whether the email address specified in the white list record matches. If so, spam protection is bypassed for this email.

### Examples

Example 1: Whitelist all email from bill@example.com:

1. Click Add to add a new white list record
2. In the description field, specify "Whitelist of bill@example.com"
3. In the email address field, specify bill@example.com.
4. Click Save

Now a record is added. Note that the IP range for this record is set to 0.0.0.0 - 255.255.255.255. This means that email coming from bill@example.com will bypass spam protection, regardless of what IP address it arrives from.

Example 2: Whitelist all email from the domain example.com.

In this example you will use a wildcard to whitelist all senders on a specific domain.

1. Click Add to add a new white list record
2. In the description field, specify "Whitelist of all at example.com"
3. In the email address field, specify *@example.com.
4. Click Save

Example 3: Whitelist all email sent from the domain example.com, from the local network (192.168.0.*)

1. Click Add to add a new white list record
2. In the description field, type "Local network"
3. In the Lower IP field, specify  192.168.0.1
4. In the Upper IP field, specify 192.168.0.255.
5. In the email address field, specify *@example.com.
6. Click Save

 

Example 4: Whitelist all email sent from the local network (192.168.0.*)

1. Click Add to add a new white list record
2. In the description field, type "Local network"
3. In the Lower IP field, specify  192.168.0.1
4. In the Upper IP field, specify 192.168.0.255.
5. Click Save

Now a record is added. Note that the E-mail address for the record has been set to *. This means that hMailServer will ignore spam protection for all email messages originating from the local network, regardless who the sender is.

### Notes

- You can use wildcards in the email address part of whitelisting records. You can use wildcards for example to whitelist
   - an entire domain - *@example.com
  - or a specific mailbox, for example*sales* at all domains - sales@*.
  - addresses containing specific words, such as "support" - *support*  

 In other words, you can use the * anywhere in the email address.
- It's not possible to use wildcards in the IP address.
- If you have selected "*Incoming relay*" in the IP range the sender is connecting from, hMailServer will use the *Received*-headers of the email to determine the originating email IP address.

## Settings

### Description

A textual description of the white list record.

### E-mail address

The e-mail address which should be white-listed. It is possible to use wildcards in this field. For example, you may whitelist all email from the domain example.com, by specifying the address *@example.com

### Lower and Upper IP

The IP range which the white list record should be applied on. If you leave these fields empty, hMailServer will assume that you mean 0.0.0.0-255.255.255.255 (the entire Internet).

 

 
