---
title: "SecurityRange object"
slug: com_object_securityrange
parent: com_objects
index: 0
is_book: false
---

### Description

The security range object represents one hMailServer IP range.

### Methods

<div class="api_method_name">Delete()</div>

<div class="api_description">Deletes the object from the database.</div>

<div class="api_method_name">Save(void)</div>

<div class="api_description">Saves the security range to the database.</div>

### Properties

<div class="api_method_name">bool AllowDeliveryFromLocalToLocal</div>

<div class="api_description">Allow deliveries from local to local.<br></div>

<div class="api_method_name">bool AllowDeliveryFromLocalToRemote</div>

<div class="api_description">Allow deliveries from local to remote.<br></div>

<div class="api_method_name">bool AllowDeliveryFromRemoteToLocal</div>

<div class="api_description">Allow deliveries from remote to local.<br></div>

<div class="api_method_name">bool AllowDeliveryFromRemoteToRemote</div>

<div class="api_description">Allow deliveries from remote to remote.<br></div>

<div class="api_method_name">bool AllowIMAPConnections</div>

<div class="api_description">AllowIMAPConnections<br></div>

<div class="api_method_name">bool AllowPOP3Connections</div>

<div class="api_description">Allow POP3 connections.<br></div>

<div class="api_method_name">bool AllowSMTPConnections</div>

<div class="api_description">Allow SMTP connections.<br></div>

<div class="api_method_name">bool EnableAntiVirus</div>

<div class="api_description">Enable virus protection for this IP range.<br></div>

<div class="api_method_name">bool EnableSpamProtection</div>

<div class="api_description">Enable spam protection for this IP range.<br></div>

<div class="api_method_name">bool Expires</div>

<div class="api_description">Gets or sets whether this IP range expires.<br></div>

<div class="api_method_name">VARIANT ExpiresTime</div>

<div class="api_description">Gets or sets the time the IP range expires (if expiration is enabled).<br></div>

<div class="api_method_name">long ID</div>

<div class="api_description">Database identifier<br> <i>(read-only)</i></div>

<div class="api_method_name">bool IsForwardingRelay</div>

<div class="api_description">Obsolete: Used IncomingRelays collection instead.<br></div>

<div class="api_method_name">string LowerIP</div>

<div class="api_description">Lower IP<br></div>

<div class="api_method_name">string Name</div>

<div class="api_description">The name of this object.<br></div>

<div class="api_method_name">long Priority</div>

<div class="api_description">Priority of this security range.<br></div>

<div class="api_method_name">bool RequireAuthForDeliveryToLocal</div>

<div class="api_description">Obsolete: Use RequireSMTPAuth* instead.<br></div>

<div class="api_method_name">bool RequireAuthForDeliveryToRemote</div>

<div class="api_description">Obsolete: Use RequireSMTPAuth* instead.<br></div>

<div class="api_method_name">bool RequireSMTPAuthExternalToExternal</div>

<div class="api_description">Require SMTP authentication for deliveries from external to external.<br></div>

<div class="api_method_name">bool RequireSMTPAuthExternalToLocal</div>

<div class="api_description">Require SMTP authentication for deliveries from external to local.<br></div>

<div class="api_method_name">bool RequireSMTPAuthLocalToExternal</div>

<div class="api_description">Require SMTP authentication for deliveries from local to external.<br></div>

<div class="api_method_name">bool RequireSMTPAuthLocalToLocal</div>

<div class="api_description">Require SMTP authentication for deliveries from local to local.<br></div>

<div class="api_method_name">bool RequireSSLTLSForAuth</div>

<div class="api_description">Require users to use SSL/TLS when authenticating.<br></div>

<div class="api_method_name">string UpperIP</div>

<div class="api_description">Upper IP<br></div>
