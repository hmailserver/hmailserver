---
id: 349
title: "Messages object"
slug: com_object_messages
parent: com_objects
index: 0
is_book: false
updated: 2008-10-31
---

### Description

The messages object is a collection of messages, typically connected to a specific IMAP folder.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description">Adds a new message to this collection.</div>

<div class="api_method_name">Clear()</div>

<div class="api_description">Removes all messages in the collection.</div>

<div class="api_method_name">DeleteByDBID(long lDBID)</div>

<div class="api_description">Deletes a message.</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">Returns the number of messages in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_message">Message</a> Item(long Index)</div>

<div class="api_description">Returns the message in the collection with the given index.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_message">Message</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns the message in the collection with the given database index.<br> <i>(read-only)</i></div>
