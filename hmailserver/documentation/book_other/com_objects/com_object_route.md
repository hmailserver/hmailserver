---
title: "Route object"
slug: com_object_route
parent: com_objects
index: 0
is_book: false
---

### Description

The route object represents one hMailServer route.

### Methods

<div class="api_method_name">Delete()</div>

<div class="api_description">Deletes the domain object the database.</div>

<div class="api_method_name">Save()</div>

<div class="api_description">Saves changes of the object in the database.</div>

<div class="api_method_name">SetRelayerAuthPassword(string newVal)</div>

<div class="api_description">Set SMTP server password</div>

### Properties

<div class="api_method_name"><a href="?page=com_object_routeaddresses">RouteAddresses</a> Addresses</div>

<div class="api_description">Valid addresses<br> <i>(read-only)</i></div>

<div class="api_method_name">bool AllAddresses</div>

<div class="api_description">Route to all addresses<br></div>

<div class="api_method_name">string Description</div>

<div class="api_description">Description for this route.<br></div>

<div class="api_method_name">string DomainName</div>

<div class="api_description">Domain name.<br></div>

<div class="api_method_name">long ID</div>

<div class="api_description">Database identifier.<br> <i>(read-only)</i></div>

<div class="api_method_name">long MinutesBetweenTry</div>

<div class="api_description">Number of minutes between every try<br></div>

<div class="api_method_name">long NumberOfTries</div>

<div class="api_description">Number of tries<br></div>

<div class="api_method_name">string RelayerAuthUsername</div>

<div class="api_description">SMTP server username<br></div>

<div class="api_method_name">bool RelayerRequiresAuth</div>

<div class="api_description">SMTP server requires authentication<br></div>

<div class="api_method_name">string TargetSMTPHost</div>

<div class="api_description">Target SMTP host<br></div>

<div class="api_method_name">long TargetSMTPPort</div>

<div class="api_description">Target SMTP host<br></div>

<div class="api_method_name">bool TreatSecurityAsLocalDomain</div>

<div class="api_description">Treat security as for local domains.<br></div>

<div class="api_method_name">bool UseSSL</div>

<div class="api_description">Use SSL for this route.<br></div>
