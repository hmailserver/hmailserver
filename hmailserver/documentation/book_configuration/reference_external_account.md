---
id: 88
title: "External accounts"
slug: reference_external_account
parent: book_configuration
index: 0
is_book: false
updated: 2014-09-23
---

## Overview

hMailServer can download messages from POP3 accounts on other servers. Email downloads are delivered to a specific account, but it is possible to redirect them to an external account, using rules. External accounts are defined in the Account settings under the *External accounts* tab.

### Name

<div class="indented">The name of the external account. The name is in free text and can be anything you like.</div>

### Server type

<div class="indented">Currently only POP3. Support for other protocols, such as IMAP, may be added in future.</div>

### Server address & TCP Port

<div class="indented">The hostname and TCP/IP port of the server hMailServer should connect to when downloading messages.</div>

### User name & Password

<div class="indented">The user name and password hMailServer should use when logging on to the external server. This should be the same login information that you normally enter in your email client when logging on to that account.</div>

### Settings

<div class="indented"><strong>Minutes between downloads</strong> defines how often hMailServer should download messages from the external server.</div>

<div class="indented"> </div>

<div class="indented">If you select <strong><em>Delete messages immediately</em></strong>, hMailServer will delete the messages from the external server right after downloading them. The opposite, <em>Do not delete messages</em>, causes hMailServer not to delete messages on the external server. If you select <em>Delete messages after [x] days</em>, hMailServer will automatically delete messages from the POP3 server when they are [x] days old.</div>

<div class="indented"> </div>

<div class="indented"><strong>Deliver to recipients in MIME headers</strong> allows you to override who hMailServer deliveres the downloaded messages to. By default, hMailServer downloads the messages and puts them in the account in which you have created the external account. For example, if you have added the external account to an account named something@domain.com, all downloaded email will be put in something@domain.com's inbox. However, if you select this option, hMailServer will deliver to the recipients in the MIME headers instead. For example, if the To field contains someone@domain.com, hMailServer will check if there is an account named someone@domain.com. In that case, hMailServer will deliver the message to that account.</div>

<div class="indented"><br>
In some cases, all recipients may not exist in the MIME headers. For example, if you send an email where a recipient is on the BCC list, this recipient will not be available in the MIME headers, and hMailServer will not know that the email should be delivered to this recipient</div>

<div class="indented"> </div>

<div class="indented">
<div class="indented"><strong>Retrieve date from Received header</strong> allows you to configure hMailServer to use the date in the latest Received-header, instead of using the current date. When hMailServer has downloaded an email from an external server, it normally sets the internal date of the message to the current date and time. If you later on download the message from hMailServer using IMAP, the internal date may be shown as "Received"-date in your email client. If you have selected this option, hMailServer will try to determine when the external POP3 server received the date, and set the internal date to the same. If this fails, hMailServer will use the current date.</div>
<div class="indented"><br>
In other words: If you want the <em>Received</em> column in your email client to show the time when hMailServer downloaded the message, don't select this option. If you want the column to show the time when the external POP3 server received it, select this option.</div>
<div class="indented"> </div>
<div class="indented"><strong>Anti-spam &amp; Anti-virus</strong></div>
<div class="indented"> </div>
<div class="indented">Select these option if you want hMailServer to scan downloaded messages for spam and viruses. If you know that the server hosting the external account already performs anti-spam and anti-virus, you may want to de-select these options in the external account to improve perfromance.</div>
<div class="indented"> </div>
</div>

### Notes

- You must have SMTP enabled in hMailServer, for the external accounts feature to work.
- hMailServer 4.0 and 4.1 will download email from external accounts, even if the parent account/domain is disabled. However, if the message is deleted from the remote server, and the local account/domain had been disabled, the message will be lost. To prevent this in future, from version 4.2 onwards, hMailServer will not download email from external accounts if the parent account or domain is disabled.
- When you configure to deliver messages to recipients in MIME headers, hMailServer checks the following headers
   - To
  - CC
  - X-RCPT-TO
  - X-Envelope-To
  - Received (multiple)
- If you have selected to deliver messages to recipients in MIME headers, and no recipients have been found, hMailServer will put the email message in the account in which the external account was created.

### Common problems

**Recipients not in MIME headers**

When delivering email to recipients in MIME headers, there is a risk that email won't be delivered to the correct recipients or that some will receive duplicates. For example, it is possible to send an email to one address but still put another email address in the MIME headers. If hMailServer reads the recipients from the MIME headers in this case, the email will be delivered to the wrong recipient (the recipient in the header). It's also possible to put recipients of an email in the BCC header (which is not included in the email message). hMailServer will not deliver messages to recipients not listed in the To, CC, X-RCPT-TO, X-Envelope-To or Received headers.

**Duplicate messages are delivered**

If several copies of the same message are stored in the external account, hMailServer will deliver multiple copies of these messages. Some SMTP servers may deliver multiple copies of the same message to the same account, if a catch-all account is used and the message is being sent to several persons on the same server. The only workaround to this problem is to reconfigure the SMTP server not to store multiple copies of the same message. Note that it is not hMailServer which needs to be re-configured but the SMTP server which delivers messages to the POP3 account hMailServer is downloading from.
