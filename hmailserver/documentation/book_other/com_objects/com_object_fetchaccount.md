---
title: "FetchAccount object"
slug: com_object_fetchaccount
parent: com_objects
index: 0
is_book: false
---

### Description

The fetch account object represents one external account which hMailServer downloads email messages from.

### Methods

<div class="api_method_name">Delete()</div>

<div class="api_description">Deletes the object from the database.</div>

<div class="api_method_name">DownloadNow(void)</div>

<div class="api_description">Performs an download now.</div>

<div class="api_method_name">Save(void)</div>

<div class="api_description">Saves the object in the database.</div>

### Properties

<div class="api_method_name">long AccountID</div>

<div class="api_description">The account that this fetch account is connected to.<br></div>

<div class="api_method_name">eConnectionSecurity ConnectionSecurity</div>

<div class="api_description">Connection security for this fetch account.<br></div>

<div class="api_method_name">long DaysToKeepMessages</div>

<div class="api_description">The number of days to keep messages on the external server.<br></div>

<div class="api_method_name">bool Enabled</div>

<div class="api_description">The fetch account is enabled.<br></div>

<div class="api_method_name">bool EnableRouteRecipients</div>

<div class="api_description">Gets or sets whether hMailServer should deliver to recipients in routes.<br></div>

<div class="api_method_name">long ID</div>

<div class="api_description">ID<br> <i>(read-only)</i></div>

<div class="api_method_name">bool IsLocked</div>

<div class="api_description">Gets whether the fetch account is currently locked<br> <i>(read-only)</i></div>

<div class="api_method_name">string MIMERecipientHeaders</div>

<div class="api_description">Read message recipients from these MIME headers.<br></div>

<div class="api_method_name">long MinutesBetweenFetch</div>

<div class="api_description">The number of minutes to wait between every download.<br></div>

<div class="api_method_name">string Name</div>

<div class="api_description">The name of the fetch account.<br></div>

<div class="api_method_name">string NextDownloadTime</div>

<div class="api_description">Next download attempt<br> <i>(read-only)</i></div>

<div class="api_method_name">string Password</div>

<div class="api_description">The password to use when logging on the external server.<br></div>

<div class="api_method_name">long Port</div>

<div class="api_description">The TCP/IP port hMailServer should connect to.<br></div>

<div class="api_method_name">bool ProcessMIMEDate</div>

<div class="api_description">Read receive date in Received header.<br></div>

<div class="api_method_name">bool ProcessMIMERecipients</div>

<div class="api_description">Read message recipients from To/CC headers.<br></div>

<div class="api_method_name">string ServerAddress</div>

<div class="api_description">The POP3 server address hMailServer should connect to.<br></div>

<div class="api_method_name">long ServerType</div>

<div class="api_description">The type of server, only POP3 at the moment.<br></div>

<div class="api_method_name">bool UseAntiSpam</div>

<div class="api_description">Gets or sets whether anti spam should be performed on messages downloaded from this account.<br></div>

<div class="api_method_name">bool UseAntiVirus</div>

<div class="api_description">Gets or sets whether anti virus should be performed on messages downloaded from this account.<br></div>

<div class="api_method_name">string Username</div>

<div class="api_description">The user name to use when logging on the external server.<br></div>

<div class="api_method_name">bool UseSSL</div>

<div class="api_description">Use SSL for external account.<br></div>
