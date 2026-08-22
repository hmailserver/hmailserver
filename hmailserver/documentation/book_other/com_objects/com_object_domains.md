---
title: "Domains object"
slug: com_object_domains
parent: com_objects
index: 0
is_book: false
---

### Description

The Domains object is a collection of all domains set up in the hMailServer installation.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description">Adds a new item to the collection</div>

<div class="api_method_name">DeleteByDBID(long DBID)</div>

<div class="api_description">Deletes the domain with the given database identifier.</div>

<div class="api_method_name">Refresh()</div>

<div class="api_description">Refreshes the collection.</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">The number of objects in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_domain">Domain</a> Item(long Index)</div>

<div class="api_description">Returns a domain by its given index in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_domain">Domain</a> ItemByDBID(long DBID)</div>

<div class="api_description">ItemByDBID<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_domain">Domain</a> ItemByName(string ItemName)</div>

<div class="api_description">Returns a domain by its name<br> <i>(read-only)</i></div>

<div class="api_method_name">string Names</div>

<div class="api_description">Returns a list containing id	name	id	name<br> <i>(read-only)</i></div>
