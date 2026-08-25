---
title: "Utilities object"
slug: com_object_utilities
parent: com_objects
index: 0
is_book: false
---

### Description

The utilities object contains misc. functions used by hMailServer Administrator.

### Methods

<div class="api_method_name">BlowfishDecrypt(string Input)</div>

<div class="api_description">Decrypts a string from blowfish.</div>

<div class="api_method_name">BlowfishEncrypt(string Input)</div>

<div class="api_description">Encrypts a string to blowfish.</div>

<div class="api_method_name">CriteriaMatch(string MatchValue, eRuleMatchType, string TestValue)</div>

<div class="api_description">Returns true if a rule criteria matches a value.</div>

<div class="api_method_name">EmailAllAccounts(string sRecipientWildcard, string sFromAddress, string sFromName, string sSubject, string sBody)</div>

<div class="api_description">Sends an email message to accounts on the server.</div>

<div class="api_method_name">GenerateGUID()</div>

<div class="api_description">Returns a unique GUID</div>

<div class="api_method_name">GetMailServer(string EMailAddress)</div>

<div class="api_description">Returns the mail server handling e-mail for the specified email address.</div>

<div class="api_method_name">ImportMessageFromFile(string sFilename, long iAccountID)</div>

<div class="api_description">Imports a message from a file on disk for delivery or to a specific account.</div>

<div class="api_method_name">ImportMessageFromFileToIMAPFolder(string sFilename, long iAccountID, string sIMAPFolder)</div>

<div class="api_description">Imports a message from a file on disk for delivery or to a specific account.</div>

<div class="api_method_name">IsLocalHost(string sHostname)</div>

<div class="api_description">Checks if the given host name points at localhost.</div>

<div class="api_method_name">IsStrongPassword(string Username, string Password)</div>

<div class="api_description">Returns true if the given password is weak</div>

<div class="api_method_name">IsValidDomainName(string sDomainName)</div>

<div class="api_description">Returns true if the given domain name is valid.</div>

<div class="api_method_name">IsValidEmailAddress(string EMailAddress)</div>

<div class="api_description">Returns true if the given e-mail address is valid. false otherwise.</div>

<div class="api_method_name">IsValidIPAddress(string IPAddress)</div>

<div class="api_description">Returns true if the given IP-address is parseable</div>

<div class="api_method_name">MakeDependent(string OtherService)</div>

<div class="api_description">Makes the hMailServer service dependent on another service.</div>

<div class="api_method_name">MD5(string Input)</div>

<div class="api_description">Returns the MD5 hex string for a string.</div>

<div class="api_method_name">PerformMaintenance(eMaintenanceOperation operation)</div>

<div class="api_description">Performs maintenance operations in the database.</div>

<div class="api_method_name">RetrieveMessageID(string sFilename)</div>

<div class="api_description">Returns the ID of the message file</div>

<div class="api_method_name">RunTestSuite(string sTestPassword)</div>

<div class="api_description">Runs test suite. This will delete all objects on the server.</div>

<div class="api_method_name">SHA256(string Input)</div>

<div class="api_description">Returns a salted SHA256 hex string for a string.</div>
