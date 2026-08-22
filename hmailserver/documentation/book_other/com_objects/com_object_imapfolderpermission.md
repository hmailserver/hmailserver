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

<div class="api_description"> Deletes the object from the database.</div>

<div class="api_description"> </div>

<div class="api_method_name">Save()</div>

<div class="api_description"> Saves changes of the object in the database.</div>

### Properties

<div class="api_method_name"><a href="?page=com_object_account">Account</a> Account</div>

<div class="api_description"> Gets the account referenced by this permission.</div>

*(read-only)*

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_group">Group</a> Group</div>

<div class="api_description"> Gets the group referenced by this permission.</div>

*(read-only)*

<div class="api_description"> </div>

<div class="api_method_name">long ID</div>

<div class="api_description"> The unique database identifier for the ACL permission.</div>

*(read-only)*

<div class="api_description"> </div>

<div class="api_method_name">bool Permission(eACLPermission iType)</div>

<div class="api_description"> ACL permission</div>

<div class="api_description"> </div>

<div class="api_method_name">long PermissionAccountID</div>

<div class="api_description"> The identifier of the account this ACL permission is connected to.</div>

<div class="api_description"> </div>

<div class="api_method_name">long PermissionGroupID</div>

<div class="api_description"> The identifier of the group this ACL permission is connected to.</div>

<div class="api_description"> </div>

<div class="api_method_name">eACLPermissionType PermissionType</div>

<div class="api_description"> The type of object this ACL permission is connected to.</div>

<div class="api_description"> </div>

<div class="api_method_name">long ShareFolderID</div>

<div class="api_description"> The folder which has been shared (read-only)</div>

*(read-only)*

<div class="api_description"> </div>

<div class="api_method_name">long Value</div>

<div class="api_description"> The ACL permission value.</div>

<div class="api_method_name"> </div>

<div class="api_method_name"><span style="font-weight: bold;">NOTE:</span></div>

<div class="api_method_name"><span class="Title">ACL permission value</span></div>

<div class="api_description">The ACL Permission values for the public IMAP folders is as follows</div>

<div class="api_description"><span class="Code"> Lookup = 1<br>
Read = 2<br>
Keep seen flag = 4<br>
Keep other flags = 8<br>
Insert Mail = 16<br>
Create Mailbox = 64<br>
Delete Mailbox = 128<br>
Keep deleted flag = 256<br>
Expunge = 512<br>
Administer = 1024<br>
<br>
</span></div>

The way this works.

<div class="api_description">If you want say 'Insert mail' and 'keep deleted flag' (this allows deletion) then you would set a value of 16+ 256 = 272<br>
If you want everything except 'delete mailbox' you would set a value of 1+2+4+8+16+64+256+512+1024 = 1887<br>
<br>
Code below sets 'Insert mail' permissions ONLY for public IMAP folder 'Another Subfolder' to the user 'account@domain.com'.<br>
<br>
Please note that if you want 'read' permissions for a subfolder, you will also need to provide 'read' permissions to parent folder(s). <br>
<br>
</div>

<div class="api_description">  <br>
<div class="api_description"> <span class="Code">Dim oApp, oPermission, oPublicFolder<br>
    Set oApp = CreateObject("hMailServer.Application")<br>
    Call oApp.Authenticate("Administrator", "Secret_Password")<br>
<br>
    'set Public Folder<br>
    set oPublicFolder = oApp.settings.Publicfolders.ItemByName("Main Folder").subfolders.itemByName("Subfolder").subfolders.add("Another Subfolder")<br>
    set oPermission = oPublicFolder.permissions.add()<br>
    with oPermission<br>
        .PermissionAccountID = oApp.Domains.ItemByName("domain.com").accounts.ItemByAddress(account@domain.com").id<br>
        .value = 16<br>
        .save()<br>
    end with</span><br>
<div class="api_description">  </div>
</div>
</div>

 
