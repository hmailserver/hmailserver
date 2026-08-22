---
title: "BlockedAttachments object"
slug: com_object_blockedattachments
parent: com_objects
index: 0
is_book: false
---

### Description

The BlockedAttachments object is a collection of all blocked attachments set up in the hMailServer installation.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description">Adds a new object to the collection.</div>

<div class="api_method_name">DeleteByDBID(long DBID)</div>

<div class="api_description">Deletes an object in the database by its given database identifier.</div>

<div class="api_method_name">Refresh()</div>

<div class="api_description">Refreshes the collection.</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">The number of objects in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_blockedattachment">BlockedAttachment</a> Item(long Index)</div>

<div class="api_description">Returns a blocked attachment by its given index.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_blockedattachment">BlockedAttachment</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns a blocked attachment by its database identifier<br> <i>(read-only)</i></div>
