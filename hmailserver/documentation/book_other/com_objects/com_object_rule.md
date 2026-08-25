---
title: "Rule object"
slug: com_object_rule
parent: com_objects
index: 0
is_book: false
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

<div class="api_description">AccountID<br></div>

<div class="api_method_name"><a href="?page=com_object_ruleactions">RuleActions</a> Actions</div>

<div class="api_description">The actions for this rule.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool Active</div>

<div class="api_description">Active<br></div>

<div class="api_method_name"><a href="?page=com_object_rulecriterias">RuleCriterias</a> Criterias</div>

<div class="api_description">The criterias for this rule.<br> <i>(read-only)</i></div>

<div class="api_method_name">long ID</div>

<div class="api_description">ID<br> <i>(read-only)</i></div>

<div class="api_method_name">string Name</div>

<div class="api_description">Name<br></div>

<div class="api_method_name">bool UseAND</div>

<div class="api_description">UseAND<br></div>
