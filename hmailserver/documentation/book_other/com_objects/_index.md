---
id: 53
title: "COM API"
slug: com_objects
parent: book_other
index: 0
is_book: false
updated: 2016-08-14
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

# Change log

For a list of changes in the COM API, see the [COM API Changelog](?page=com_changelog)

# .NET Class

The complete COM API is available as a .NET class in the hMailserver bin directory ...\hmailserver\bin\Interop.hMailServer.dll

# Reference

- ## Domain objects

   - [Domains](?page=com_object_domains)
  - [Domain](?page=com_object_domain)
     - [Accounts](?page=com_object_accounts)
    - [Account](?page=com_object_account)
       - [IMAPFolders](?page=com_object_imapfolders)
      - [IMAPFolder](?page=com_object_imapfolder)
         - [Messages](?page=com_object_messages)
        - [Message](?page=com_object_message)
           - [Attachments](?page=com_object_attachments)
          - [Attachment](?page=com_object_attachment)
          - [Recipients](?page=com_object_recipients)
          - [Recipient](?page=com_object_recipient)
      - [FetchAccounts](?page=com_object_fetchaccounts)
      - [FetchAccount](?page=com_object_fetchaccount)
    - [Aliases](?page=com_object_aliases)
    - [Alias](?page=com_object_alias)
    - [DistributionLists](?page=com_object_distributionlists)
    - [DistributionList](?page=com_object_distributionlist)
       - [DistributionListRecipients](?page=com_object_distributionlistrecipients)
      - [DistributionListRecipient](?page=com_object_distributionlistrecipient)
    - [DomainAliases](?page=com_object_domainaliases)
    - [DomainAlias](?page=com_object_domainalias)
- ## Settings

   - [AntiSpam](?page=com_object_antispam)
  - [AntiVirus](?page=com_object_antivirus)
  - [Cache](?page=com_object_cache)
  - [BackupSettings](?page=com_object_backupsettings)
  - [BlockedAttachments](?page=com_object_blockedattachments)
  - [BlockedAttachment](?page=com_object_blockedattachment)
  - [DNSBlackLists](?page=com_object_dnsblacklists)
  - [DNSBlackList](?page=com_object_dnsblacklist)
  - [GreyListingWhiteAddresses](?page=com_object_greylistingwhiteaddresses)
  - [GreyListingWhiteAddress](?page=com_object_greylistingwhiteaddress)
  - [Language](?page=com_object_language)
  - [Links](?page=com_object_links)
  - [Logging](?page=com_object_logging)
  - [Routes](?page=com_object_routes)
  - [Route](?page=com_object_route)
     - [RouteAddresses](?page=com_object_routeaddresses)
    - [RouteAddress](?page=com_object_routeaddress)
  - [Scripting](?page=com_object_scripting)
  - [SecurityRanges](?page=com_object_securityranges)
  - [SecurityRange](?page=com_object_securityrange)
  - [ServerMessages](?page=com_object_servermessages)
  - [ServerMessage](?page=com_object_servermessage)
  - [SURBLServers](?page=com_object_surblservers)
  - [SURBLServer](?page=com_object_surblserver)
  - [TCPIPPorts](?page=com_object_tcpipports)
  - [TCPIPPort](?page=com_object_tcpipport)
  - [Settings](?page=com_object_settings)
  - [Rules](?page=com_object_rules)
  - [Rule](?page=com_object_rule)
     - [RuleCriterias](?page=com_object_rulecriterias)
    - [RuleCriteria](?page=com_object_rulecriteria)
    - [RuleActions](?page=com_object_ruleactions)
    - [RuleAction](?page=com_object_ruleaction)
  - [WhiteListAddresses](?page=com_object_whitelistaddresses)
  - [WhiteListAddress](?page=com_object_whitelistaddress)
- ## Server-wide objects

   - [Application](?page=com_object_application)
  - [Backup](?page=com_object_backup)
  - [BackupManager](?page=com_object_backupmanager)
  - [Database](?page=com_object_database)
  - [DeliveryQueue](?page=com_object_deliveryqueue)
  - [GlobalObjects](?page=com_object_globalobjects)
  - [Status](?page=com_object_status)
  - [Utilities](?page=com_object_utilities)
- ## Scripting-wide objects

   - [Client](?page=com_object_client)
  - [Result](?page=com_object_result)

# Examples

Please see the [COM API examples page](?page=com_examples) .
