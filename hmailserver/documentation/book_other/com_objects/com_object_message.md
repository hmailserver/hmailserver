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

<div class="api_method_name"><br>
AddRecipient(STR sName, STR sAddress)</div>

<div class="api_description">Adds a recipient to the e-mail.</div>

<div class="api_method_name"><br>
ClearRecipients()</div>

<div class="api_description">Clears the recipient of the email (including the MIME-headers).</div>

<div class="api_method_name"><br>
Copy(long iDestinationFolderID)</div>

<div class="api_description">Copies the message to the specified IMAP folder</div>

<div class="api_method_name"><br>
HasBodyType(string BodyType)</div>

<div class="api_description">Check whether an email contains a specific content-type.</div>

<div class="api_method_name"><br>
RefreshContent()</div>

<div class="api_description">Refreshes the message content</div>

<div class="api_method_name"><br>
Save()</div>

<div class="api_description">Saves the email. If this is a new email, it will be delivered after save.</div>

### Properties

<div class="api_method_name"><br>
<a href="?page=com_object_attachments">Attachments</a> Attachments </div>

<div class="api_description">The attachments of the message<br>
<i>(read-only)</i></div>

<div class="api_method_name"><br>
string Body</div>

<div class="api_description">The plain text contents of the e-mail.</div>

<div class="api_method_name"><br>
string CC</div>

<div class="api_description">The CC-header of the email<br>
<i>(read-only)</i></div>

<div class="api_method_name"><br>
string Charset</div>

<div class="api_description">The main character set of the email.</div>

<div class="api_method_name"><br>
string Date</div>

<div class="api_description">The date header of the e-mail message.</div>

<div class="api_method_name"><br>
long DeliveryAttempt</div>

<div class="api_description">The number of this delivery attempt<br>
<i>(read-only)</i></div>

<div class="api_method_name"><br>
bool EncodeFields</div>

<div class="api_description">Encode fields.</div>

<div class="api_method_name"><br>
string Filename</div>

<div class="api_description">The filename in witch the message is stored<br>
<i>(read-only)</i></div>

<div class="api_method_name"><br>
bool Flag(eMessageFlag iType)</div>

<div class="api_description">Message flags (see bottom of this page for further detail)</div>

<div class="api_method_name"><br>
string From</div>

<div class="api_description">The sender of the e-mail.</div>

<div class="api_method_name"><br>
string FromAddress</div>

<div class="api_description">The sender address of the message, taken from "mail from" during the SMTP conversation and then inserted as the first occurrence of Return-Path.</div>

<div class="api_method_name"><br>
<a href="?page=com_object_messageheaders">MessageHeaders</a> Headers</div>

<div class="api_description">Collection of message headers<br>
<i>(read-only)</i></div>

<div class="api_method_name"><br>
string HeaderValue(string FieldName)</div>

<div class="api_description">Value of the first occurrence of the given header field. FieldName is not case sensitive.</div>

<div class="api_method_name"><br>
string HTMLBody</div>

<div class="api_description">The HTML contents of the e-mail.</div>

<div class="api_method_name"><br>
long ID</div>

<div class="api_description">The unique database identifier for the e-mail<br>
<i>(read-only)</i></div>

<div class="api_method_name"><br>
VARIANT InternalDate</div>

<div class="api_description">The internal date and time of this message<br>
<i>(read-only)</i></div>

<div class="api_method_name"><br>
<a href="?page=com_object_recipients">Recipients</a> Recipients</div>

<div class="api_description">An list of recipients of this email<br>
<i>(read-only)</i></div>

<div class="api_method_name"><br>
long Size</div>

<div class="api_description">The size of the message, counted in kilobytes<br>
<i>(read-only)</i></div>

<div class="api_method_name"><br>
long State</div>

<div class="api_description">The state of the message<br>
<i>(read-only)</i></div>

<div class="api_method_name"><br>
string Subject</div>

<div class="api_description">The subject of the e-mail.</div>

<div class="api_method_name"><br>
string To</div>

<div class="api_description">The To-header of the email<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_description"> </div>

<div class="api_description"> </div>

<div class="api_description"> </div>

<div class="api_description">eMessageFlags<span class="Title"><br>
<br>
</span>The message flag is a Binary Flag (Boolean) which can contain several set values at same time.</div>

<div class="api_description">1 = Seen (Read)<br>
2 = Deleted<br>
4 = Flagged<br>
8 = Answered<br>
16 = Draft<br>
32 = Recent<br>
64 =Virus Scanned</div>

<div class="api_description">Or various combinations of above values so 32 + 64 = 96 would mean Recent and Virus Scanned.</div>

 
