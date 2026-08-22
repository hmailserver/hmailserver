---
id: 412
title: "SSLCertificates object"
slug: com_object_sslcertificates
parent: com_objects
index: 0
is_book: false
updated: 2008-10-31
---

### Description

The SSLCertificates object is a collection of all SSL certificates set up in hMailServer.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description">Adds a new object to the collection.</div>

<div class="api_method_name">Clear()</div>

<div class="api_description">Clears the collection.</div>

<div class="api_method_name">DeleteByDBID(long DBID)</div>

<div class="api_description">Deletes an object in the database by its given database identifier.</div>

<div class="api_method_name">Refresh()</div>

<div class="api_description">Refreshes the collection.</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">The number of objects in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_sslcertificate">SSLCertificate</a> Item(long Index)</div>

<div class="api_description">Returns a SSL certificate by its given index.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_sslcertificate">SSLCertificate</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns a SSL certificate by its database identifier<br> <i>(read-only)</i></div>
