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

<div class="api_description"> Deletes the object from the database.</div>

<div class="api_description"> </div>

<div class="api_method_name">Save()</div>

<div class="api_description"> Saves the security range to the database.</div>

### Properties

<div class="api_method_name">bool AllowDeliveryFromLocalToLocal</div>

<div class="api_description"> Allow deliveries from local to local.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool AllowDeliveryFromLocalToRemote</div>

<div class="api_description"> Allow deliveries from local users to external users.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool AllowDeliveryFromRemoteToLocal</div>

<div class="api_description"> Allow deliveries from external users to local users.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool AllowDeliveryFromRemoteToRemote</div>

<div class="api_description"> Allow deliveries from external users to external users.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool AllowIMAPConnections</div>

<div class="api_description"> AllowIMAPConnections</div>

<div class="api_description"> </div>

<div class="api_method_name">bool AllowPOP3Connections</div>

<div class="api_description"> Allow POP3 connections.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool AllowSMTPConnections</div>

<div class="api_description"> Allow SMTP connections.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool EnableAntiVirus</div>

<div class="api_description"> Enable virus protection for this IP range.</div>

<div class="api_description"> </div>

<div class="api_method_name"><em>bool EnableSpamProtection</em></div>

<div class="api_description"> Enable spam protection for this IP range.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool Expires</div>

<div class="api_description"> If true then this IP range will expire. Used by Autoban.  </div>

<div class="api_description"> </div>

<div class="api_method_name">Date/Time ExpiresTime</div>

<div class="api_description"> Gets or sets the time the IP range expires (if expiration is enabled).</div>

<div class="api_description"> </div>

<div class="api_method_name">long ID</div>

<div class="api_description"> Database identifier<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">bool IsForwardingRelay</div>

<div class="api_description"> True if this is a forwarding relay.</div>

<div class="api_description"> </div>

<div class="api_method_name">string LowerIP</div>

<div class="api_description"> Lower IP</div>

<div class="api_description"> </div>

<div class="api_method_name">string Name</div>

<div class="api_description"> The name of this object.</div>

<div class="api_description"> </div>

<div class="api_method_name"><span class="Code">long Priority</span></div>

<div class="api_description"> Priority of this security range</div>

<div class="api_description"> </div>

<div class="api_method_name">bool RequireSMTPAuthExternalToLocal</div>

<div class="api_description"> Require SMTP Auth for deliveries to local users from external users.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool RequireSMTPAuthExternalToExternal</div>

<div class="api_description"> Require SMTP Auth for deliveries to external users from external users.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool RequireSMTPAuthLocalToLocal</div>

<div class="api_description"> Require SMTP Auth for deliveries to local users from local users.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool RequireSMTPAuthLocalToExternal</div>

<div class="api_description"> Require SMTP Auth for deliveries to external users from local users.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool RequireSSLTLSForAuth</div>

<div class="api_description"> Require users to use SSL/TLS when authenticating.</div>

<div class="api_description"> </div>

<div class="api_method_name">string UpperIP</div>

<div class="api_description"> Upper IP</div>

<div class="api_description"> </div>

<div class="api_method_name"><span style="font-weight: bold;">NOTE:</span></div>

<div class="api_method_name">Local Users are considered by hMaislerver to be users that have an email account hosted locally on this hMailserver, or if a route is considered 'Local' then additionally users with an email address at the domain of the route will be considered to be local users.</div>

<div class="api_method_name">Remote / External users are anyone else.</div>

<div class="api_method_name">These definitions have nothing to with originiating IP, ie someone who connects from a LAN may well be an 'external' user, as much as a local user may connect and be required to authenticate from the internet IP range (think mobile phone user).</div>

 
