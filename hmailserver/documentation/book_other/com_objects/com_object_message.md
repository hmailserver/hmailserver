---
title: "Message object"
slug: com_object_message
parent: com_objects
index: 0
is_book: false
---

### Description

The message object represents a single e-mail message.

### Methods

<div class="api_method_name">AddRecipient(string sName, string sAddress)</div>

<div class="api_description">Adds a recipient to the e-mail.</div>

<div class="api_method_name">ClearRecipients()</div>

<div class="api_description">Clears the recipient of the email (including the MIME-headers).</div>

<div class="api_method_name">Copy(long iDestinationFolderID)</div>

<div class="api_description">Copies the message to the specified IMAP folder</div>

<div class="api_method_name">HasBodyType(string BodyType)</div>

<div class="api_description">Check whether an email contains a specific content-type.</div>

<div class="api_method_name">RefreshContent()</div>

<div class="api_description">Refreshes the message content</div>

<div class="api_method_name">Save()</div>

<div class="api_description">Saves the email. If this is a new email, it will be delivered after save.</div>

### Properties

<div class="api_method_name"><a href="?page=com_object_attachments">Attachments</a> Attachments</div>

<div class="api_description">The attachments of the message.<br> <i>(read-only)</i></div>

<div class="api_method_name">string Body</div>

<div class="api_description">The plain text contents of the e-mail.<br></div>

<div class="api_method_name">string CC</div>

<div class="api_description">The CC-header of the email.<br> <i>(read-only)</i></div>

<div class="api_method_name">string Charset</div>

<div class="api_description">The main character set of the email.<br></div>

<div class="api_method_name">string Date</div>

<div class="api_description">The contents of the Date message header.<br></div>

<div class="api_method_name">long DeliveryAttempt</div>

<div class="api_description">The number of this delivery attempt.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool EncodeFields</div>

<div class="api_description">Encode fields.<br></div>

<div class="api_method_name">string Filename</div>

<div class="api_description">The name of the file in witch the message is stored.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool Flag(eMessageFlag iType)</div>

<div class="api_description">Message flags<br></div>

<div class="api_method_name">string From</div>

<div class="api_description">The contents of the From message header.<br></div>

<div class="api_method_name">string FromAddress</div>

<div class="api_description">The From address specified in the message envelope.<br></div>

<div class="api_method_name"><a href="?page=com_object_messageheaders">MessageHeaders</a> Headers</div>

<div class="api_description">Collection of message heades.<br> <i>(read-only)</i></div>

<div class="api_method_name">string HeaderValue(string FieldName)</div>

<div class="api_description">Value of the given header field.<br></div>

<div class="api_method_name">string HTMLBody</div>

<div class="api_description">The HTML contents of the e-mail.<br></div>

<div class="api_method_name">long ID</div>

<div class="api_description">The unique database identifier for the e-mail.<br> <i>(read-only)</i></div>

<div class="api_method_name">VARIANT InternalDate</div>

<div class="api_description">The internal date and time of this message.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_recipients">Recipients</a> Recipients</div>

<div class="api_description">An list of recipients of this email<br> <i>(read-only)</i></div>

<div class="api_method_name">long Size</div>

<div class="api_description">The size of the message, counted in kilobytes.<br> <i>(read-only)</i></div>

<div class="api_method_name">long State</div>

<div class="api_description">The state of the message.<br> <i>(read-only)</i></div>

<div class="api_method_name">string Subject</div>

<div class="api_description">The subject of the e-mail.<br></div>

<div class="api_method_name">string To</div>

<div class="api_description">The To-header of the email.<br> <i>(read-only)</i></div>

<div class="api_method_name">long UID</div>

<div class="api_description">The mailbox-unique identifier for the e-mail.<br> <i>(read-only)</i></div>
