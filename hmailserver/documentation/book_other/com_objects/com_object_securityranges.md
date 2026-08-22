---
id: 361
title: "SecurityRanges object"
slug: com_object_securityranges
parent: com_objects
index: 0
is_book: false
updated: 2008-10-31
---

### Description

The security ranges object is a collection of all IP ranges set up in the hMailServer installation.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description">Adds a new security range to the collection.</div>

<div class="api_method_name">Delete(long Index)</div>

<div class="api_description">Deletes the security range with the specified index.</div>

<div class="api_method_name">DeleteByDBID(long DBID)</div>

<div class="api_description">Deletes the security range with the specified database identifier.</div>

<div class="api_method_name">Refresh()</div>

<div class="api_description">Refreshes the collection.</div>

<div class="api_method_name">SetDefault()</div>

<div class="api_description">Reverts IP range settings back to their default values.</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">The number of objects in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_securityrange">SecurityRange</a> Item(long Index)</div>

<div class="api_description">Returns a securityrange by a given index.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_securityrange">SecurityRange</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns a securityrange by a given database identifier.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_securityrange">SecurityRange</a> ItemByName(string sName)</div>

<div class="api_description">Returns a securityrange by a given name.<br> <i>(read-only)</i></div>
