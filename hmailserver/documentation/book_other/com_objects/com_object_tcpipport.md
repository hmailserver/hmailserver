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

<div class="api_method_name">Save()</div>

<div class="api_description">Saves changes of the object in the database.</div>

### Properties

<div class="api_method_name">string Address</div>

<div class="api_description">The TCP/IP address to bind to.<br></div>

<div class="api_method_name">eConnectionSecurity ConnectionSecurity</div>

<div class="api_description">Connection security for this TCP/IP port.<br></div>

<div class="api_method_name">long ID</div>

<div class="api_description">The unique database identifier for the object.<br> <i>(read-only)</i></div>

<div class="api_method_name">long PortNumber</div>

<div class="api_description">The TCP/IP port number.<br></div>

<div class="api_method_name">eSessionType Protocol</div>

<div class="api_description">The type of protocol connected to this port.<br></div>

<div class="api_method_name">long SSLCertificateID</div>

<div class="api_description">The id of the SSL certificate to use.<br></div>

<div class="api_method_name">bool UseSSL</div>

<div class="api_description">Use SSL for this TCP/IP port.<br></div>
