---
id: 116
title: "Distribution list"
slug: reference_distributionlist
parent: book_configuration
index: 0
is_book: false
updated: 2016-04-17
---

## General

### Address

<div class="indented">The address of the distribution list. Messages sent to this address will be forwarded to all recipients on the distribution list.</div>

### Mode

<div class="indented">
<ul>
    <li>Public - Anyone can send to the list</li>
    <li>Membership - Only members can send to the list</li>
    <li>Announcements - Only messages to the list from a specific address will be allowed.</li>
</ul>
</div>

### Require SMTP Authentication

<div class="indented">If this checkbox is selected, hMailServer will require SMTP authentication for deliveries made to the list. If you select this option, only users with accounts on the server will be able to send email to the distribution list.</div>

## Members

##  

### Add

<div class="indented">Click <em>Add</em> to add an address to the distribution list.</div>

### Select

Click *Select* to add an address that is hosted on this hMailserver to the distribution list.

### Import

Click to browse and point to a text file of email addresses (one per line). Once the file is selected, the addresses contained within the file will be displayed in the window - click 'Import'.

#### Delete Recipients Not in Import File

Checking this box will remove all current addresses from the distribution list prior to the import of the new addresses.

### Edit

Click *Edit* to edit an existing address in the distribution list.

### Remove

<div class="indented">Click <em>Remove</em> to remove the selected address from the distribution list.</div>
