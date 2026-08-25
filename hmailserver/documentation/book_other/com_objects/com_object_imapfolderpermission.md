---
title: "IMAPFolderPermission object"
slug: com_object_imapfolderpermission
parent: com_objects
index: 0
is_book: false
---

### Description

The IMAPFolderPermission represents a single permission attached to an IMAP folder.

### Methods

<div class="api_method_name">Delete()</div>

<div class="api_description">Deletes the object from the database.</div>

<div class="api_method_name">Save()</div>

<div class="api_description">Saves changes of the object in the database.</div>

### Properties

<div class="api_method_name"><a href="?page=com_object_account">Account</a> Account</div>

<div class="api_description">Gets the account referenced by this permission.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_group">Group</a> Group</div>

<div class="api_description">Gets the group referenced by this permission.<br> <i>(read-only)</i></div>

<div class="api_method_name">long ID</div>

<div class="api_description">The unique database identifier for the ACL permission.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool Permission(eACLPermission iType)</div>

<div class="api_description">ACL permission<br></div>

<div class="api_method_name">long PermissionAccountID</div>

<div class="api_description">The identifier of the account this ACL permission is connected to.<br></div>

<div class="api_method_name">long PermissionGroupID</div>

<div class="api_description">The identifier of the group this ACL permission is connected to.<br></div>

<div class="api_method_name">eACLPermissionType PermissionType</div>

<div class="api_description">The type of object this ACL permission is connected to.<br></div>

<div class="api_method_name">long ShareFolderID</div>

<div class="api_description">The folder which has been shared (read-only)<br> <i>(read-only)</i></div>

<div class="api_method_name">long Value</div>

<div class="api_description">The ACL permission value.<br></div>
