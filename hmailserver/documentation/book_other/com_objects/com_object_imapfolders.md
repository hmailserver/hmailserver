---
title: "IMAPFolders object"
slug: com_object_imapfolders
parent: com_objects
index: 0
is_book: false
---

### Description

The IMAPFolders object is a collection of the root IMAP folders in an account, or the public folders.

### Methods

<div class="api_method_name">Add(string sName)</div>

<div class="api_description">Adds an IMAP folder with the given name to the collection. It is saved immediately.</div>

<div class="api_method_name">DeleteByDBID(long lDBID)</div>

<div class="api_description">Deletes an IMAP folder with the given database identifier.</div>

### Properties

<div class="api_method_name">long Count</div>

<div class="api_description">Returns the number of IMAP folders in the collection<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_imapfolder">IMAPFolder</a> Item(long Index)</div>

<div class="api_description">Returns an IMAP folder by it's index in the collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_imapfolder">IMAPFolder</a> ItemByDBID(long DBID)</div>

<div class="api_description">Returns an IMAP folder by it's database ID.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_imapfolder">IMAPFolder</a> ItemByName(string sName)</div>

<div class="api_description">Returns an IMAP folder by it's name.<br> <i>(read-only)</i></div>
