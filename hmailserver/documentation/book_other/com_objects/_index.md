---
title: "COM API"
slug: com_objects
parent: book_other
index: 0
is_book: false
---

# Overview

The hMailServer COM API lets you write your own scripts and applications that integrate with hMailServer. Almost all objects in hMailServer are accessible using the COM library. As an example, both hMailServer Administrator and the PHP WebAdmin is entirely based upon the COM library. Besides writing application like theese two, you can write scripts that are automatically run when certain events take place. For example, you can write a script that filters messages before they are delivered to the recipients.   

  

The [Application](?page=com_object_application) object is the root object in hMailServers COM model. Using this object, you can access all hMailServer objects and settings. Before accessing any of the properties and methods on the Application object, you must call Application.Authenticate with valid credentials.

The hMailServer API behaves as most other COM APIs, but there are some [special behavior](?page=com_api_structure) which relates to exception handling and permissions.

<p>It is possible to create triggers in hMailServer. Triggers are script which are executed when certain actions occur. Triggers are created by adding the script to the <span class="Code">EventHandlers</span>-file in the hMailServer <span class="Code">Events</span> folder.</p>

# Basic steps

These are the basic steps to perform when you want to access the hMailServer COM API.

1. Create an instance of the hMailServer Application object. How to do this depends on what programming language you use. In VBScript, you call CreateObject("hMailServer.Application").
2. Authenticate. Before you can call any other methods in the API, you need to authenticate. This is done using the Authenticate() method on the Application object. This is required to prevent unauthorized users to access your hMailServer installation.
3. Call the methods and use the properties you need.

# .NET Class

The complete COM API is available as a .NET class in the hMailserver bin directory ...\hmailserver\bin\Interop.hMailServer.dll

# Reference

- [Account](?page=com_object_account)
- [Accounts](?page=com_object_accounts)
- [Alias](?page=com_object_alias)
- [Aliases](?page=com_object_aliases)
- [AntiSpam](?page=com_object_antispam)
- [AntiVirus](?page=com_object_antivirus)
- [Application](?page=com_object_application)
- [Attachment](?page=com_object_attachment)
- [Attachments](?page=com_object_attachments)
- [Backup](?page=com_object_backup)
- [BackupManager](?page=com_object_backupmanager)
- [BackupSettings](?page=com_object_backupsettings)
- [BlockedAttachment](?page=com_object_blockedattachment)
- [BlockedAttachments](?page=com_object_blockedattachments)
- [Cache](?page=com_object_cache)
- [Client](?page=com_object_client)
- [Constants](?page=com_object_constants)
- [Database](?page=com_object_database)
- [DeliveryQueue](?page=com_object_deliveryqueue)
- [DiagnosticResult](?page=com_object_diagnosticresult)
- [DiagnosticResults](?page=com_object_diagnosticresults)
- [Diagnostics](?page=com_object_diagnostics)
- [Directories](?page=com_object_directories)
- [DistributionList](?page=com_object_distributionlist)
- [DistributionListRecipient](?page=com_object_distributionlistrecipient)
- [DistributionListRecipients](?page=com_object_distributionlistrecipients)
- [DistributionLists](?page=com_object_distributionlists)
- [DNSBlackList](?page=com_object_dnsblacklist)
- [DNSBlackLists](?page=com_object_dnsblacklists)
- [Domain](?page=com_object_domain)
- [DomainAlias](?page=com_object_domainalias)
- [DomainAliases](?page=com_object_domainaliases)
- [Domains](?page=com_object_domains)
- [EventLog](?page=com_object_eventlog)
- [FetchAccount](?page=com_object_fetchaccount)
- [FetchAccounts](?page=com_object_fetchaccounts)
- [GlobalObjects](?page=com_object_globalobjects)
- [GreyListingWhiteAddress](?page=com_object_greylistingwhiteaddress)
- [GreyListingWhiteAddresses](?page=com_object_greylistingwhiteaddresses)
- [Group](?page=com_object_group)
- [GroupMember](?page=com_object_groupmember)
- [GroupMembers](?page=com_object_groupmembers)
- [Groups](?page=com_object_groups)
- [IMAPFolder](?page=com_object_imapfolder)
- [IMAPFolderPermission](?page=com_object_imapfolderpermission)
- [IMAPFolderPermissions](?page=com_object_imapfolderpermissions)
- [IMAPFolders](?page=com_object_imapfolders)
- [IncomingRelay](?page=com_object_incomingrelay)
- [IncomingRelays](?page=com_object_incomingrelays)
- [Language](?page=com_object_language)
- [Languages](?page=com_object_languages)
- [Links](?page=com_object_links)
- [Logging](?page=com_object_logging)
- [Message](?page=com_object_message)
- [MessageHeader](?page=com_object_messageheader)
- [MessageHeaders](?page=com_object_messageheaders)
- [MessageIndexing](?page=com_object_messageindexing)
- [Messages](?page=com_object_messages)
- [Recipient](?page=com_object_recipient)
- [Recipients](?page=com_object_recipients)
- [Result](?page=com_object_result)
- [Route](?page=com_object_route)
- [RouteAddress](?page=com_object_routeaddress)
- [RouteAddresses](?page=com_object_routeaddresses)
- [Routes](?page=com_object_routes)
- [Rule](?page=com_object_rule)
- [RuleAction](?page=com_object_ruleaction)
- [RuleActions](?page=com_object_ruleactions)
- [RuleCriteria](?page=com_object_rulecriteria)
- [RuleCriterias](?page=com_object_rulecriterias)
- [Rules](?page=com_object_rules)
- [Scripting](?page=com_object_scripting)
- [SecurityRange](?page=com_object_securityrange)
- [SecurityRanges](?page=com_object_securityranges)
- [ServerMessage](?page=com_object_servermessage)
- [ServerMessages](?page=com_object_servermessages)
- [Settings](?page=com_object_settings)
- [SSLCertificate](?page=com_object_sslcertificate)
- [SSLCertificates](?page=com_object_sslcertificates)
- [Status](?page=com_object_status)
- [SURBLServer](?page=com_object_surblserver)
- [SURBLServers](?page=com_object_surblservers)
- [TCPIPPort](?page=com_object_tcpipport)
- [TCPIPPorts](?page=com_object_tcpipports)
- [Utilities](?page=com_object_utilities)
- [WhiteListAddress](?page=com_object_whitelistaddress)
- [WhiteListAddresses](?page=com_object_whitelistaddresses)

# Examples

Please see the [COM API examples page](?page=com_examples) .
