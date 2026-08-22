---
id: 379
title: "Rule object"
slug: com_object_rule
parent: com_objects
index: 0
is_book: false
updated: 2022-09-12
---

### Description

The Rule object represents a single hMailServer rule.

### Methods

<div class="api_method_name">Delete()</div>

 

<div class="api_description">Deletes the object from the database.</div>

 

<div class="api_method_name">MoveDown()</div>

 

<div class="api_description">Moves the rule down in the priority order</div>

 

<div class="api_method_name">MoveUp()</div>

 

<div class="api_description">Moves the rule up in the priority order.</div>

 

<div class="api_method_name">Save()</div>

 

<div class="api_description">Saves changes of the object in the database.</div>

 

### Properties

<div class="api_method_name">long AccountID</div>

 

<div class="api_description">AccountID</div>

 

<div class="api_method_name"><a href="?page=com_object_ruleactions">RuleActions</a> Actions</div>

 

<div class="api_description">The actions for this rule.<br>
<i>(read-only)</i></div>

 

<div class="api_method_name">bool Active</div>

 

<div class="api_description">Active</div>

 

<div class="api_method_name"><a href="?page=com_object_rulecriterias">RuleCriterias</a> Criterias</div>

 

<div class="api_description">The criterias for this rule.<br>
<i>(read-only)</i></div>

 

<div class="api_method_name">long ID</div>

 

<div class="api_description">ID<br>
<i>(read-only)</i></div>

 

<div class="api_method_name">string Name</div>

 

<div class="api_description">Name</div>

 

<div class="api_method_name">bool UseAND</div>

 

<div class="api_description">UseAND</div>

 

### Sample Code

<div class="api_description">Creating a forward rule<br>
The following sample creates an account rule that forwards all messages larger than 0 bytes to someone@test.com.<br>
 </div>

<div class="api_description"> </div>

<p><span class="Code">    Dim obRule As Rule    <br>
    Set obRule = obAccount.Rules.Add        <br>
    obRule.Name = "My forward rule"        <br>
<br>
    ' Add a If MessageSize Is Greater Than 0 criteria    <br>
    Dim obCriteria    <br>
    Set obCriteria = obRule.Criterias.Add        <br>
    obCriteria.MatchType = eMTGreaterThan    <br>
    obCriteria.PredefinedField = eFTMessageSize    <br>
    obCriteria.MatchValue = 0    <br>
    obCriteria.Save        <br>
<br>
    ' Create a forward action    <br>
    Dim obAction    <br>
    Set obAction = obRule.Actions.Add    <br>
    obAction.Type = eRAForwardEmail    <br>
    obAction.To = "someone@test.com"    <br>
    obAction.Save        <br>
<br>
    ' Save the entire rule    <br>
    obRule.Save  </span></p>

 
