---
title: "Database object"
slug: com_object_database
parent: com_objects
index: 0
is_book: false
---

### Description

The database object contains information on current database set-up and basic methods to run SQL towards the database.

### Methods

<div class="api_method_name">BeginTransaction()</div>

<div class="api_description">Starts a new SQL transaction. Only affects ExecuteSQL().</div>

<div class="api_method_name">CommitTransaction()</div>

<div class="api_description">Commits a SQL transaction. Only affects ExecuteSQL().</div>

<div class="api_method_name">CreateExternalDatabase(eDBtype ServerType, string ServerName, long lPort, string DatabaseName, string Username, string Password)</div>

<div class="api_description">Create external database</div>

<div class="api_method_name">CreateInternalDatabase()</div>

<div class="api_description">Create internal database</div>

<div class="api_method_name">ExecuteSQL(string sSQLStatement)</div>

<div class="api_description">Executes an SQL statement.</div>

<div class="api_method_name">ExecuteSQLScript(string sFilename)</div>

<div class="api_description">Executes an script file containing SQL statements.</div>

<div class="api_method_name">RollbackTransaction()</div>

<div class="api_description">Rollbacks a SQL transaction. Only affects ExecuteSQL().</div>

<div class="api_method_name">SetDefaultDatabase(eDBtype ServerType, string ServerName, long lPort, string DatabaseName, string Username, string Password)</div>

<div class="api_description">Set default database</div>

<div class="api_method_name">UtilGetFileNameByMessageID(long MessageID)</div>

<div class="api_description">Returns the filename for a given message id.</div>

### Properties

<div class="api_method_name">long CurrentVersion</div>

<div class="api_description">Current databaseversion.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool DatabaseExists</div>

<div class="api_description">Checks whether an existing database exists.<br> <i>(read-only)</i></div>

<div class="api_method_name">string DatabaseName</div>

<div class="api_description">Database name.<br> <i>(read-only)</i></div>

<div class="api_method_name">eDBtype DatabaseType</div>

<div class="api_description">The type of the database.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool IsConnected</div>

<div class="api_description">Returns true if there is a database available.<br> <i>(read-only)</i></div>

<div class="api_method_name">long RequiredVersion</div>

<div class="api_description">Required database version.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool RequiresUpgrade</div>

<div class="api_description">Database needs to be upgraded.<br> <i>(read-only)</i></div>

<div class="api_method_name">string ServerName</div>

<div class="api_description">Server name.<br> <i>(read-only)</i></div>
