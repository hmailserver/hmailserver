---
title: "DistributionListRecipients object"
slug: com_object_distributionlistrecipients
parent: com_objects
index: 0
is_book: false
---

### Description

The DistributionListRecipients object is a collection of all recipients in a single distribution list.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description">Adds a new recipient to the collection.</div>

<div class="api_method_name">DeleteByDBID(long lDBID)</div>

<div class="api_description">Deletes a recipient from the collection.</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">Returns the number of recipients in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_distributionlistrecipient">DistributionListRecipient</a> Item(long Index)</div>

<div class="api_description">Returns the recipient in the collection with the given index.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_distributionlistrecipient">DistributionListRecipient</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns the recipient in the collection with the given database index.<br> <i>(read-only)</i></div>
