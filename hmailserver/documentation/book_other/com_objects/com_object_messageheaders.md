---
title: "MessageHeaders object"
slug: com_object_messageheaders
parent: com_objects
index: 0
is_book: false
---

### Description

The MessageHeaders object is a collection of all MIME headers in a single email message.

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">The number of objects in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_messageheader">MessageHeader</a> Item(long Index)</div>

<div class="api_description">Returns an object by its given index.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_messageheader">MessageHeader</a> ItemByName(string sName)</div>

<div class="api_description">Returns an object by it's name. If multiple matching, first is returned.<br> <i>(read-only)</i></div>
