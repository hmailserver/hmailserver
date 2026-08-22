---
title: "DistributionLists object"
slug: com_object_distributionlists
parent: com_objects
index: 0
is_book: false
---

### Description

The DistributionLists object is a collection of all distribution lists connected to a specific domain.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description">Adds a new list to the collection.</div>

<div class="api_method_name">DeleteByDBID(long lDBID)</div>

<div class="api_description">Deletes a list from the collection.</div>

<div class="api_method_name">Refresh()</div>

<div class="api_description">Refreshes the collection from the database.</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">Returns the number of lists in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_distributionlist">DistributionList</a> Item(long Index)</div>

<div class="api_description">Returns the distribution list by the given index.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_distributionlist">DistributionList</a> ItemByAddress(string sAddress)</div>

<div class="api_description">Returns the list in the collection by the given address.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_distributionlist">DistributionList</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns the list in the collection by the given database identifier.<br> <i>(read-only)</i></div>
