---
title: "RouteAddresses object"
slug: com_object_routeaddresses
parent: com_objects
index: 0
is_book: false
---

### Description

The RouteAddresses object is a collection of all recipients in a single hMailServer route.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description">Adds a new object to the collection.</div>

<div class="api_method_name">DeleteByAddress(string sAddress)</div>

<div class="api_description">Deletes an object in the database by its given address.</div>

<div class="api_method_name">DeleteByDBID(long DBID)</div>

<div class="api_description">Deletes an object in the database by its given identifier.</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">The number of objects in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_routeaddress">RouteAddress</a> Item(long Index)</div>

<div class="api_description">Returns an attachment by its given index.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_routeaddress">RouteAddress</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns a route address by its database identifier<br> <i>(read-only)</i></div>
