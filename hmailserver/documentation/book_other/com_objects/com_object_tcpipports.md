---
title: "TCPIPPorts object"
slug: com_object_tcpipports
parent: com_objects
index: 0
is_book: false
---

### Description

The TCP/IP ports object contains a collection of all TCP/IP ports and addresses hMailServer is configured to listen on.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description">Adds a new object to the collection.</div>

<div class="api_method_name">DeleteByDBID(long DBID)</div>

<div class="api_description">Deletes an object in the database by its given database identifier.</div>

<div class="api_method_name">Refresh()</div>

<div class="api_description">Refreshes the collection.</div>

<div class="api_method_name">SetDefault()</div>

<div class="api_description">Generate default set of objects in this collection</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">The number of objects in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_tcpipport">TCPIPPort</a> Item(long Index)</div>

<div class="api_description">Returns an object by its given index.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_tcpipport">TCPIPPort</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns a object by its database identifier<br> <i>(read-only)</i></div>
