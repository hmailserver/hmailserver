---
id: 393
title: "IMAPFolder object"
slug: com_object_imapfolder
parent: com_objects
index: 0
is_book: false
updated: 2008-10-31
---

### Description

The IMAPFolder object represents a single IMAP folder.

### Methods

<div class="api_method_name">Delete()</div>

<div class="api_description">Deletes the object from the database.</div>

<div class="api_method_name">Save()</div>

<div class="api_description">Saves changes of the object in the database.</div>

### Properties

<div class="api_method_name">long ID</div>

<div class="api_description">The database identifier for the IMAP folder.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_messages">Messages</a> Messages</div>

<div class="api_description">The messages in this folder.<br> <i>(read-only)</i></div>

<div class="api_method_name">string Name</div>

<div class="api_description">The name of the IMAP folder.<br></div>

<div class="api_method_name">long ParentID</div>

<div class="api_description">The database identifier for the parent IMAP folder.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_imapfolderpermissions">IMAPFolderPermissions</a> Permissions</div>

<div class="api_description">The permissions for this IMAP folder<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_imapfolders">IMAPFolders</a> SubFolders</div>

<div class="api_description">The IMAP folders in this folder.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool Subscribed</div>

<div class="api_description">The subscribed state of the IMAP folder.<br></div>
