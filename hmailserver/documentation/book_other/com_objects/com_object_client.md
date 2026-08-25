---
title: "Client object"
slug: com_object_client
parent: com_objects
index: 0
is_book: false
---

### Description

The client object is a temporary object used in hMailServer events which allows scripts to check the address and username of a connected client.

### Properties

<div class="api_method_name">bool Authenticated</div>

<div class="api_description">The logon status of the client.<br> <i>(read-only)</i></div>

<div class="api_method_name">long CipherBits</div>

<div class="api_description">The SSL/TLS cipher bits of the client connection.<br> <i>(read-only)</i></div>

<div class="api_method_name">string CipherName</div>

<div class="api_description">The SSL/TLS cipher name of the client connection.<br> <i>(read-only)</i></div>

<div class="api_method_name">string CipherVersion</div>

<div class="api_description">The SSL/TLS cipher version of the client connection.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool EncryptedConnection</div>

<div class="api_description">The SSL/TLS status of the client.<br> <i>(read-only)</i></div>

<div class="api_method_name">string HELO</div>

<div class="api_description">The HELO greeting sent by client.<br> <i>(read-only)</i></div>

<div class="api_method_name">string IPAddress</div>

<div class="api_description">The TCP/IP address the client is connected to.<br> <i>(read-only)</i></div>

<div class="api_method_name">long Port</div>

<div class="api_description">The TCP/IP port the client is connected to.<br> <i>(read-only)</i></div>

<div class="api_method_name">long SessionID</div>

<div class="api_description">The assigned SessionID for the TCP/IP connection.<br> <i>(read-only)</i></div>

<div class="api_method_name">string Username</div>

<div class="api_description">The username of the client.<br> <i>(read-only)</i></div>
