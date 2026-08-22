---
id: 413
title: "Groups object"
slug: com_object_groups
parent: com_objects
index: 0
is_book: false
updated: 2008-10-31
---

### Description

The Groups object is a collection of all server-wide groups set up in hMailServer.

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

<div class="api_method_name"><a href="?page=com_object_group">Group</a> Item(long Index)</div>

<div class="api_description">Returns an object by it's index.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_group">Group</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns an object by its database identifier<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_group">Group</a> ItemByName(string Name)</div>

<div class="api_description">Returns an object by its name<br> <i>(read-only)</i></div>
