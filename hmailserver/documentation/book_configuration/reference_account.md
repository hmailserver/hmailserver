---
id: 33
title: "Account"
slug: reference_account
parent: book_configuration
index: 0
is_book: false
updated: 2022-09-12
---

## General

### Account address

<div class="indented">The email address of the account. An account can only have one email address. You can use <a href="?page=reference_alias">aliases</a> to redirect email from many email addresses to one single account.</div>

<div class="indented"> </div>

<div class="indented">hMailServer does not support the following characters in accounts:</div>

- ? * |  \ / < >

<p><strong>IMPORTANT:</strong>  When EDITING (renaming) an <span style="text-decoration: underline">existing account</span>   be aware that the process can take some time to complete and you will   temporarily need double the disk space taken by the old account data   directory.  This is due to the process actually creating a data   directory for the new account and then <strong>copying all email data from the old account at operating system level</strong>   before then deleting the old account directory.  Depending on the old account data size and processor power, this copying could take many   minutes.  It is advised to restrict email use for the old account whilst   the copying of this data is taking place and to monitor for the process   completion by watching for the old account data directory to finally be   deleted.</p>

### Account password

<div class="indented">The password of the account. Passwords are encrypted and stored in the hMailServer database.</div>

### Maximum size (MB)

<div class="indented">The maximum disk space that the account may use. If the limit is reached, the account will not be able to receive any more email. In hMailServer 3.0, the account size is measured in KB. From hMailServer 4.0 onwards, it is measured in MB. If an e-mail is sent to this account when the quota has been used, hMailServer will deliver a notification to the sender containing the information that the quota for the recipient had been reached. After that, the email message will be dropped.</div>

<div class="indented"> </div>

<div class="indented">When an email is sent over the Internet, any binary data is encoded (because of limitations in the SMTP protocol). This encoding increases the size of the email message with an average of 50%. This means that if you create an account, set the quota to 10MB and send a 10MB attachment, the message will most likely to big for the account. When configuring a mailbox size, you may need to take this into consideration.</div>

### Administration level

<div class="indented">
<div class="indented">The administration level setting lets you define which parts of the server a user should have access to. This setting is primarily used by PHPWebAdmin.</div>
<div class="indented">
<ul>
    <li>User - The user can change settings which applies to his own account. For example, he can change his password, his out-of-office message and forwarding settings. The user can't increase the maximum size of his own account, and he can't modify the active directory settings</li>
    <li>Domain - The user can change settings which applies to his domain and the users in it. The user can change settings for all users in his domain, add new users, add aliases and distribution lists, delete objects, increase account max sizes and so on.</li>
    <li>Server - The user can change any setting and modify any object (such as domains and accounts) in the server. </li>
</ul>
</div>
<h3>Last logon time</h3>
This shows the date and time of the last logon on this account. If a user has never logged on the account, the date and time when the account was created will be shown.</div>

<div class="indented">
<h3>Enabled</h3>
This option lets you enable and disable the account.</div>

## Auto-reply

An Auto-reply is also known as a Vacation message or an Out-of-office Notification. An auto-reply is sent automatically when you go on vacation or are away from the office for some time. Before you leave, you enter a subject and a message. When someone sends you an email, hMailServer will automatically send your auto-reply message to the sender.

### Specifying an auto-reply message

1. Open up an account in hMailAdmin
2. Select the auto-reply tab
3. Select *Enable*
4. Enter a subject and a message
5. Click on *Save*

### Notes

- If you leave the subject field empty, the server will automatically set the subject to `Re: [subject-line of sender's original email]`
- To prevent message looping, auto-reply messages are not sent to accounts which have auto-reply enabled. Also, hMailServer only sends one auto-reply per sender.
- We recommend that you unsubscribe from any distribution lists before you turn vacation messages on.
- The macro %SUBJECT% can be used in the Subject and Body of the auto-reply message. The text %SUBJECT% will be replaced with the subject of the original e-mail message.

 

### Automatically expire

<div class="indented">By selecting Automatically expire, you can configure hMailServer to automatically disable the auto-reply at a given date. This may be good if you know that you will be out of office for 3 days. When you're back, you don't have to remember to manually disable the auto-reply again.</div>

<div class="indented">
<h2>Forwarding</h2>
<p>The forwarding functionality lets you forward email from this account to another. Select <em>enable forwarding </em>to start forwarding of messages. Enter the email address you want to forward messages to. If you want to forward messages without keeping copies of them, de-select <em>Keep original message</em>.</p>
<h2>Signature</h2>
<p>If you specify a signature, this will be appended to email messages sent from this account. It's possible to specify both a plain text signature and a HTML signature. If a plain text signature has been specified, but no HTML signature, hMailServer will use the plain text signature as HTML signature. hMailServer will convert the plain text line breaks to HTML line breaks. <br>
<br>
It is possible to use the macros %User.FirstName% and %User.LastName% in signatures. These macros will be replaced with the users first and last name as specified in the account settings.</p>
<p>Depending on the signature configuration in the domain settings, signatures can be applied to all email messages or just messages sent to external addresses.</p>
</div>

## Rules

Account rules work just like global rules. The difference is that local rules are only applied to messages that are delivered to a specific account. See the documentation on global rules for more information.

## External accounts

Using the external account functionality, you can configure hMailServer to download email from other e-mail servers, using the POP3 protocol. After the messages have been downloaded, global rules, virus scanning and etc are applied on the messages. After that, they are normally delivered to one or several local accounts.

### Scenario 1:

You have a hMailServer installation where you host email for your domain. You want to download email from another email server and put it in one of the local accounts.

### Steps

1. Open the account settings for the account you want to download email to
2. Select the *External accounts* tab
3. Click *Add* to add a new account.
4. Enter a name for the external account.
5. Enter the login information.
6. De-select "Deliver to recipients in MIME-headers" (if it is selected)
7. Specify how often you want hMailServer to download messages, and how long they should be stoerd on the remote POP3 server.
8. Save the account

After you have performed the steps above, hMailServer will download the messages and put them in the account in which you created the external account.

### Scenario 2:

Your domain is hosted by your ISP. They have created a "catch all" email account for you. Whenever anyone sends you an email to a recipient on the domain, it ends up in the catch-all account.

### Steps

1. Create the domain in hMailServer  

 The domain name should be your public domain name.
2. Create accounts for your users.
3. Open the settings for your own account
4. Select the *External accounts* tab
5. Click *Add* to add a new account.
6. Enter a name for the external account.
7. Enter the login information.
8. Select "Deliver to recipients in MIME-headers"
9. Specify how often you want hMailServer to download messages, and how long they should be stored on the remote POP3 server.
10. Save the account

After you have performed the steps above, hMailServer will download the messages and deliver them to the recipients in the MIME headers of the email message. If hMailServer cannot determine who the message should be delivered to (if no local recipients exists in the MIME headers), it will be delivered to your account (the account in which you added the external account)

## Active Directory connection

### Active Directory Account

<div class="indented">Check this checkbox if you want to connect the account to a Windows NT/2000 Active Directory Account. There are several advantages in using a connection to an Active Directory. For example, none of the account passwords are stored in the hMailServer database. Instead, the user must supply his/her Windows NT/2000 domain password when logging in to the POP3 server.</div>

### Active Directory Domain

<div class="indented">Active Directory Domain is the name of the Windows NT/2000 domain, in the case where the mail server account is connected to a Windows 2000/NT active directory account.</div>

### Active Directory User name

<div class="indented">Active Directory User name is the user name of the active directory domain that the mail server account is connected to.</div>

<h2 class="indented">Advanced</h2>

<h3 class="indented">Personal information</h3>

<div class="indented">Use this setting to specify the full name of the user holding this account. hMailServer does not use this information<span style='color: rgb(36, 41, 47); font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Helvetica, Arial, sans-serif, "Apple Color Emoji", "Segoe UI Emoji"; font-size: 14px; font-style: normal; font-variant-ligatures: normal; font-variant-caps: normal; font-weight: 400; letter-spacing: normal; orphans: 2; text-align: start; text-indent: 0px; text-transform: none; white-space: normal; widows: 2; word-spacing: 0px; -webkit-text-stroke-width: 0px; background-color: rgb(255, 255, 255); text-decoration-thickness: initial; text-decoration-style: initial; text-decoration-color: initial; display: inline !important; float: none;'>, except for signature macros</span></div>

<h3 class="indented">Other actions</h3>

<ul>
    <li>
    <div class="indented">Edit folders - This option allows you to craete and delete IMAP folders connected to this account</div>
    </li>
    <li>
    <div class="indented">Empty account - This option will delete all IMAP folders and their content (messages) from the account.</div>
    </li>
    <li>
    <div class="indented">Unlock - This option will remove the POP3 lock on this folder. This option should only be used if the account remains locked even though the client has disconnected.</div>
    </li>
</ul>

 
