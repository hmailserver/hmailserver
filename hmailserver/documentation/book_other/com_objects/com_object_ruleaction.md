---
title: "RuleAction object"
slug: com_object_ruleaction
parent: com_objects
index: 0
is_book: false
---

### Description

The RuleAction object represents a single action in a specific rule.

### Methods

<div class="api_method_name">Delete()</div>

 

<div class="api_description">Deletes the object from the database.</div>

 

<div class="api_method_name">MoveDown()</div>

 

<div class="api_description">Moves the action up in the order.</div>

 

<div class="api_method_name">MoveUp()</div>

 

<div class="api_description">Moves the action up in the order.</div>

 

<div class="api_method_name">Save()</div>

 

<div class="api_description">Saves changes of the object in the database.</div>

 

### Properties

<div class="api_method_name">string Body</div>

 

<div class="api_description">When the action type is eRAReply, the Body property will contain the text to reply with.</div>

 

<div class="api_method_name">string Filename</div>

 

<div class="api_description">Currently not used.</div>

 

<div class="api_method_name">string FromAddress</div>

 

<div class="api_description">When the action type is eRAReply, the FromAddress property will contain the sender address.</div>

 

<div class="api_method_name">string FromName</div>

 

<div class="api_description">When the action type is eRAReply, the FromName property will contain the sender name.</div>

 

<div class="api_method_name">string HeaderName</div>

 

<div class="api_description">Header name</div>

 

<div class="api_method_name">long ID</div>

 

<div class="api_description">The database identifier for the rule action.<br>
<i>(read-only)</i></div>

 

<div class="api_method_name">string IMAPFolder</div>

 

<div class="api_description">When the action type is eRAMoveToImapFolder, this property will contain the name of the IMAP folder the message should be moved to.</div>

 

<div class="api_method_name">long RouteID</div>

 

<div class="api_description">Route ID to deliver via</div>

 

<div class="api_method_name">long RuleID</div>

 

<div class="api_description">The database identifier for the rule that this rule action is connected to.</div>

 

<div class="api_method_name">string ScriptFunction</div>

 

<div class="api_description">Script function to run</div>

 

<div class="api_method_name">string Subject</div>

 

<div class="api_description">When the action type is eRAReply, this property will contain the subject of the reply email message.</div>

 

<div class="api_method_name">string To</div>

 

<div class="api_description">When the action type is eRAForwardEmail, the message will be forwarded to the email address specified by this property.</div>

 

<div class="api_method_name">eRuleActionType Type</div>

 

<div class="api_description">The type of action:<br>
- Const eRAUnknown = 0, undefined behaviour (don't use)<br>
- Const eRADeleteEmail = 1, delete the email<br>
- Const eRAForwardEmail = 2, forward the email to another address<br>
- Const eRAReply = 3, reply to the sender of the email<br>
- Const eRAMoveToImapFolder = 4, move the message to an IMAP folder<br>
- Const eRARunScriptFunction = 5, run a sub or function contained in eventhandlers.vbs, with the required parameter passed being oMessage. Use the Sub or Function name as value.<br>
- Const eRAStopRuleProcessing = 6, stops further rule processing<br>
- Const eRASetHeaderValue = 7, sets a header value, will replace existing headers of same name or create new header. The header name is the value.<br>
- Const eRASendUsingRoute = 8, Send via a route named to match the value.<br>
- Const eRACreateCopy = 9, Creates an exact copy of the message, including recipients<br>
- Const eRABindToAddress = 10, Binds the message outgoing SMTP to the IP address listed as the value<br>
 </div>

 

<div class="api_method_name">string Value</div>

 

<div class="api_description">Various values, depending on Rule Action Type</div>

 
