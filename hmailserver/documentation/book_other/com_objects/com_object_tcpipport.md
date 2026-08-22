---
title: "TCPIPPort object"
slug: com_object_tcpipport
parent: com_objects
index: 0
is_book: false
---

### Description

The TCPIPPort object represents a single address and port hMailServer is configured to listen on.

### Methods

<div class="api_method_name">Delete()</div>

<div class="api_description">Deletes the object from the database.</div>

<div class="api_method_name"> </div>

<div class="api_method_name">Save()</div>

<div class="api_description">Saves changes of the object in the database.</div>

<div class="api_description"> </div>

### Properties

<div class="api_method_name">string Address</div>

<div class="api_description">The TCP/IP address to bind to.</div>

<div class="api_description"> </div>

<div class="api_method_name">long ID</div>

<div class="api_description">The unique database identifier for the object.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_description"> </div>

<div class="api_method_name">long PortNumber</div>

<div class="api_description">The TCP/IP port number.</div>

<div class="api_description"> </div>

<div class="api_method_name">eConnectionSecurity ConnectionSecurity<br>
<div class="api_description">The type of security associated with this port. (numeric value  representing None = 0, SSL = 1, STARTTLS Optional = 2, STARTTLS Required  = 3</div>
</div>

<div class="api_method_name"> </div>

<div class="api_method_name">eSessionType Protocol</div>

<div class="api_description">The type of protocol connected to this port. (numeric value representing SMTP =1 POP3 = 3 IMAP =5)</div>

<div class="api_description"> </div>

<div class="api_method_name">long SSLCertificateID</div>

<div class="api_description">The id of the SSL certificate to use.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool UseSSL</div>

<div class="api_description">Use SSL for this TCP/IP port.</div>

<div class="api_description"> </div>

<div class="api_description"> </div>
