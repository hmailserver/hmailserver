---
title: "DistributionList object"
slug: com_object_distributionlist
parent: com_objects
index: 0
is_book: false
---

### Description

The DistributionList object represents a hMailServer distribution list.

### Methods

<div class="api_method_name">Delete()</div>

<div class="api_description">Deletes the distribution list from the database.</div>

<div class="api_method_name">Save()</div>

<div class="api_description">Saves the distribution list in the database.</div>

### Properties

<div class="api_method_name">bool Active</div>

<div class="api_description">True if the distribution list is active, false otherwise<br></div>

<div class="api_method_name">string Address</div>

<div class="api_description">The address of the distribution list.<br></div>

<div class="api_method_name">long ID</div>

<div class="api_description">The unique database identifier for the distribution list.<br> <i>(read-only)</i></div>

<div class="api_method_name">eDistributionListMode Mode</div>

<div class="api_description">Distribution list mode.<br></div>

<div class="api_method_name"><a href="?page=com_object_distributionlistrecipients">DistributionListRecipients</a> Recipients</div>

<div class="api_description">The recipients in the distribution list.<br> <i>(read-only)</i></div>

<div class="api_method_name">string RequireSenderAddress</div>

<div class="api_description">Require the user to send from the adress.<br></div>

<div class="api_method_name">bool RequireSMTPAuth</div>

<div class="api_description">Sets wether the list requires a user to SMTP Authenticate before sending mail to it.<br></div>
