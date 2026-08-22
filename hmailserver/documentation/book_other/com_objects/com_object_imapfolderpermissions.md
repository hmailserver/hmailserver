---
id: 418
title: "IMAPFolderPermissions object"
slug: com_object_imapfolderpermissions
parent: com_objects
index: 0
is_book: false
updated: 2017-02-14
---

### Description

The IMAPFolderPermissions object is a collection of all permissions connected to a specific public IMAP folder.

### Methods

<div class="api_method_name">Add()</div>

<div class="api_description"> Adds a new permission to the collection.</div>

<div class="api_description"> </div>

<div class="api_method_name">Delete(long Index)</div>

<div class="api_description"> Deletes the permission with the specified index.</div>

<div class="api_description"> </div>

<div class="api_method_name">DeleteByDBID(long DBID)</div>

<div class="api_description"> Deletes an permission by its given database identifier.</div>

<div class="api_description"> </div>

<div class="api_method_name">Refresh()</div>

<div class="api_description"> Refreshes the collection.</div>

<div class="api_description"> </div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description"> Returns the number of permissions in the collection.</div>

*(read-only)*

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_imapfolderpermission">IMAPFolderPermission</a> Item(long Index)</div>

<div class="api_description"> Returns a permission by a given index.</div>

*(read-only)*

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_imapfolderpermission">IMAPFolderPermission</a> ItemByDBID(long DBID)</div>

<div class="api_description"> Returns an permission by its given database identifier.</div>

*(read-only)*

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_imapfolderpermission">IMAPFolderPermission</a> ItemByName(string Name)</div>

<div class="api_description"> Returns an permission by it's name.</div>

*(read-only)*

<div class="api_description"> </div>
