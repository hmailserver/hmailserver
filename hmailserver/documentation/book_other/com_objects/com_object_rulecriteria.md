---
id: 380
title: "RuleCriteria object"
slug: com_object_rulecriteria
parent: com_objects
index: 0
is_book: false
updated: 2022-09-12
---

### Description

The RuleCriteria object represents a single criteria in a rule.

### Methods

<div class="api_method_name">Delete()</div>

 

<div class="api_description">Deletes the object from the database.</div>

 

<div class="api_method_name">Save()</div>

 

<div class="api_description">Saves changes of the object in the database.</div>

 

### Properties

<div class="api_method_name">string HeaderField</div>

 

<div class="api_description">HeaderField</div>

 

<div class="api_method_name">long ID</div>

 

<div class="api_description">ID<br>
<i>(read-only)</i></div>

 

<div class="api_method_name">eRuleMatchType MatchType</div>

 

<div class="api_description">The type of match that should be made. Can be any of the following.<br>
- Const eMTContains = 2<br>
- Const eMTEquals = 1<br>
- Const eMTGreaterThan = 4<br>
- Const eMTLessThan = 3<br>
- Const eMTNotContains = 6<br>
- Const eMTNotEquals = 7<br>
- Const eMTRegExMatch = 5<br>
- Const eMTUnknown = 0<br>
 </div>

<div class="api_method_name">string MatchValue</div>

 

<div class="api_description">The value that should be searched for.</div>

 

<div class="api_method_name">eRulePredefinedField PredefinedField</div>

 

<div class="api_description">Rule criterias can be connected to a number of different pre defined fields. If UsePredefined is set to true, the predefined field defined by PredefinedField will be used. It can be any of the following:<br>
- Const eFTFrom = 1, the From MIME header field<br>
- Const eFTBody = 5, the text body part of the email message<br>
- Const eFTCC = 3, the CC MIME header field<br>
- Const eFTMessageSize = 6, the size of the message<br>
- Const eFTRecipientList = 7, a list of all recipients. This is taken from the SMTP envelope, and includes BCC recipients<br>
- Const eFTSubject = 4, the Subject MIME header field<br>
- Const eFTTo = 2, the To MIME header field<br>
- Const eFTUnknown = 0, (undefined behaviour, don't use)</div>

 

<div class="api_method_name">long RuleID</div>

 

<div class="api_description">The database identifier for the rule that this rule criteria is connected to.</div>

 

<div class="api_method_name">bool UsePredefined</div>

 

<div class="api_description">If this value is true, the field define by PredefinedField will be used. If the value is false, the field defined by HeaderValue will be used.</div>

 

 
