---
title: "Application object"
slug: com_object_application
parent: com_objects
index: 0
is_book: false
---

### Description

The application object is the root object in the hMailServer API. It's the first object you should create when you want to access the hMailServer API

### Methods

<div class="api_method_name">Authenticate(string Username, string Password)</div>

<div class="api_description">Authenticate the client.</div>

<div class="api_method_name">Connect()</div>

<div class="api_description">Opens a connection to the hMailServer database.</div>

<div class="api_method_name">Reinitialize()</div>

<div class="api_description">Reloads all the server settinsg</div>

<div class="api_method_name">Start()</div>

<div class="api_description">Starts an hMailServer server.</div>

<div class="api_method_name">Stop()</div>

<div class="api_description">Stops an hMailServer server.</div>

<div class="api_method_name">SubmitEMail()</div>

<div class="api_description">Submits pending email messages. May be triggered if an email has been manually inserted into the database.</div>

### Properties

<div class="api_method_name"><a href="?page=com_object_backupmanager">BackupManager</a> BackupManager</div>

<div class="api_description">Gets the backup manager.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_database">Database</a> Database</div>

<div class="api_description">Gets the database interface.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_diagnostics">Diagnostics</a> Diagnostics</div>

<div class="api_description">Gets a diagnostics object.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_domains">Domains</a> Domains</div>

<div class="api_description">Gets the Domains collection.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_globalobjects">GlobalObjects</a> GlobalObjects</div>

<div class="api_description">Gets the global objects object.<br> <i>(read-only)</i></div>

<div class="api_method_name">string InitializationFile</div>

<div class="api_description">The initialization file currently in use.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_links">Links</a> Links</div>

<div class="api_description">Quick links to access objects<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_rules">Rules</a> Rules</div>

<div class="api_description">Gets a collection of global rules.<br> <i>(read-only)</i></div>

<div class="api_method_name">eServerState ServerState</div>

<div class="api_description">Gets the current state of the server.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_settings">Settings</a> Settings</div>

<div class="api_description">Gets the Settings object.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_status">Status</a> Status</div>

<div class="api_description">Returns the Status object.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_utilities">Utilities</a> Utilities</div>

<div class="api_description">Gets the Utilities object.<br> <i>(read-only)</i></div>

<div class="api_method_name">string Version</div>

<div class="api_description">Gets the current hMailServer version<br> <i>(read-only)</i></div>

<div class="api_method_name">string VersionArchitecture</div>

<div class="api_description">Gets the current hMailServer architecture (x86/x64)<br> <i>(read-only)</i></div>
