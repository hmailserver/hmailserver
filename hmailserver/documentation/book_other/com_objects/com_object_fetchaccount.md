---
id: 373
title: "FetchAccount object"
slug: com_object_fetchaccount
parent: com_objects
index: 0
is_book: false
updated: 2016-09-29
---

### Description

The fetch account object represents one external account which hMailServer downloads email messages from.

### Methods

<div class="api_method_name">Delete()</div>

 

<div class="api_description">Deletes the object from the database.</div>

 

<div class="api_method_name">DownloadNow()</div>

 

<div class="api_description">Performs an download now.</div>

 

<div class="api_method_name">Save()</div>

 

<div class="api_description">Saves the object in the database.</div>

 

### Properties

<div class="api_method_name">long AccountID</div>

 

<div class="api_description">The account that this fetch account is connected to.</div>

 

<div class="api_method_name">long DaysToKeepMessages</div>

 

<div class="api_description">The number of days to keep messages on the external server.</div>

<div class="api_description">Delete immediately: DaysToKeepMessages=-1<br>
Never delete: DaysToKeepMessages=0</div>

 

<div class="api_method_name">bool Enabled</div>

 

<div class="api_description">The fetch account is enabled.</div>

 

<div class="api_method_name">long ID</div>

 

<div class="api_description">ID<br>
<i>(read-only)</i></div>

 

<div class="api_method_name">long MinutesBetweenFetch</div>

 

<div class="api_description">The number of minutes to wait between every download.</div>

 

<div class="api_method_name">string Name</div>

 

<div class="api_description">The name of the fetch account.</div>

 

<div class="api_method_name">string NextDownloadTime</div>

 

<div class="api_description">Next download attempt<br>
<i>(read-only)</i></div>

 

<div class="api_method_name">string Password</div>

 

<div class="api_description">The password to use when logging on the external server.</div>

 

<div class="api_method_name">long Port</div>

 

<div class="api_description">The TCP/IP port hMailServer should connect to.</div>

 

<div class="api_method_name">bool ProcessMIMEDate</div>

 

<div class="api_description">Read receive date in Received header.</div>

 

<div class="api_method_name">bool ProcessMIMERecipients</div>

 

<div class="api_description">Read message recipients from To/CC headers.</div>

 

<div class="api_method_name">string ServerAddress</div>

 

<div class="api_description">The POP3 server address hMailServer should connect to.</div>

 

<div class="api_method_name">long ServerType</div>

 

<div class="api_description">The type of server, only POP3 at the moment.</div>

 

<div class="api_method_name">string Username</div>

 

<div class="api_description">The user name to use when logging on the external server.</div>

 

<div class="api_method_name">bool UseSSL</div>

 

<div class="api_description">Use SSL for external account.</div>

 
