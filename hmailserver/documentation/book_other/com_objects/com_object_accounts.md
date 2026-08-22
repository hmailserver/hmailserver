---
id: 345
title: "Accounts object"
slug: com_object_accounts
parent: com_objects
index: 0
is_book: false
updated: 2008-10-31
---

### Description

The accounts object is a collection of all accounts connected to a specific domain.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description">Adds a new item to the collection.</div>

<div class="api_method_name">Delete(long Index)</div>

<div class="api_description">Deletes an item by its given index in the collection.</div>

<div class="api_method_name">DeleteByDBID(long DBID)</div>

<div class="api_description">Deletes an account in the database by its given database identifier.</div>

<div class="api_method_name">Refresh()</div>

<div class="api_description">Refreshes the collection</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">Returns the number of items in the collection<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_account">Account</a> Item(long Index)</div>

<div class="api_description">Returns an account by its given index in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_account">Account</a> ItemByAddress(string Address)</div>

<div class="api_description">Returns an account by its given email address.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_account">Account</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns an account by its given database identifier.<br> <i>(read-only)</i></div>
