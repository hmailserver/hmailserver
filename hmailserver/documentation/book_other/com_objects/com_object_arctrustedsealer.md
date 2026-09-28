---
title: "ARCTrustedSealer object"
slug: com_object_arctrustedsealer
parent: com_objects
index: 0
is_book: false
---

### Description

The ARCTrustedSealer object contains a domain whose ARC seal may override a DMARC failure.

### Methods

<div class="api_method_name">Delete()</div>

<div class="api_description">Deletes the object from the database.</div>

<div class="api_method_name">Save()</div>

<div class="api_description">Saves changes of the object in the database.</div>

### Properties

<div class="api_method_name">string Description</div>

<div class="api_description">Description of the sealer.<br></div>

<div class="api_method_name">long ID</div>

<div class="api_description">The unique database identifier for the object.<br> <i>(read-only)</i></div>

<div class="api_method_name">string Domain</div>

<div class="api_description">The domain in the d= tag of the ARC-Seal, for example google.com.<br></div>
