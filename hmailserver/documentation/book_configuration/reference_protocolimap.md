---
id: 75
title: "IMAP settings"
slug: reference_protocolimap
parent: book_configuration
index: 0
is_book: false
updated: 2009-07-20
---

<h3><span class="Title">General</span></h3>

#### Connections

<div class="indented">This setting defines the maximum number of simultaneous connections that will be allowed to the IMAP server. If zero is specified, an unlimited number of connections will be allowed.</div>

#### Other / Welcome message

<div class="indented">The welcome message is sent to IMAP clients directly after they connect to the server. One reason to change this message is if you don't want anyone to know what kind of server software you are using.</div>

<div class="indented">
<h3><span class="Title">Public folders<br>
</span></h3>
<h4>Public folder name</h4>
<p>The public folder name will be visible to all users who have access  to public folders.</p>
<h4>Edit folders</h4>
<p>Select this option to manage public folders.</p>
<p>Permissions are applied in the following manner:</p>
<ul>
    <li>If a permission matching the specific user is found, that permission is used.</li>
    <li>If not, hMailServer will check if the user is a member of a group. If that's the case, the first group is selected.</li>
    <li>If not, hMailServer will check whether an "Anyone" permission has been set up.</li>
</ul>
</div>

<h3><span class="Title">Advanced</span></h3>

#### Extensions

Use these settings to enable and disable IMAP extensions on the server.

- [The IMAP SORT extension](?page=details_imap_sort) allows email messages to be sorted on the server instead of in the email client. This increases the performance in web mail clients.
- IMAP Quota - The IMAP Quota extension makes it possible for IMAP clients to check the quota usage for the account.
- IMAP Idle - Using this extension, IMAP client can receive notifications from the server whenever a new email exist. This way the email client does not have to manually check for new messages every X minute.
- IMAP ACL - When this extension is enabled, you can set up public folders and permissions for these.

#### Hierarchy delimiter

Select which hierarchy delimiter you want hMailServer to use. The delimiter is used in the communication between IMAP clients and hMailServer. For example, in the hierarchy Inbox\Test\Sub the delimiter is \.

After a delimiter has been selected, this delimiter can not be used in folder names - since it is used to delimit folder levels.  It is not possible to change delimiter to a character which is only in use in a folder name on the server. Nor is it possible to change to a delimiter which is used in folder names in rule actions since this would create a conflict.

When you change the hierarchy delimiter, hMailServer will automatically update all rule actions containing this hierarchy delimiter to the new one.

 

 
