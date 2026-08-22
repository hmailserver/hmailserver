---
title: "COM API ChangeLog"
slug: com_changelog
parent: com_objects
index: 0
is_book: false
---

This document briefly lists the changes made to the COM API.

## hMailServer 5.0

### Changes breaking compatibility

hMailServer 5 includes a lot of new functionality. Some of the functionality in hMailServer 4 has been redesigned, which have had the effect that the corresponding APIs have been redesigned as well. Due to this, hMailServer 5 has API changes which breaks backwards compatibility.

The following objects no longer exists. The contents of these two have been migrated to the TCPIPPort object

- IPHomes
- IPHome

The following properties no longer exists:

- Alias
   - Type - This property has been removed since it was not used.
- AntiSpam
   - AntiSpamAction - This property no longer exists since the anti spam funcitonality in version 5 is score-based.
- Backup
   - OverwriteObjects - This property has been removed since it was no longer used - the corresponding functionality in the backup functionality has been removed.
  - DeleteObjectsNotInBackups - This property has been removed since it was no longer used - the corresponding functionality in the backup functionality has been removed.
- GlobalObjects
   - The Language property have been removed and replaced with a Languages property which returns a collection of languages.
- Language
   - The signature of the String property have been updated.
- Settings
   - POP3Port. This property is no longer relevant since hMailServer may listen on multiple ports.
  - SMTPPort. This property is no longer relevant since hMailServer may listen on multiple ports.
  - TarpitDelay - This property has been moved to the AntiSpam object.
  - TarpitCount - This property has been moved to the AntiSpam object.
  - IMAPPort - This property is no longer relevant since hMailServer may listen on multiple ports.
  - UseSPF - This property has been moved to the AntiSpam object.
  - UseMXChecks - This property has been moved to the AntiSpam object.
  - DNSBlackLists - This property has been moved to the AntiSpam object.
  - ListenOnAllAddresses - This property is no longer relevant since the configuraiton of how hMailServer listens on different ports have been changed.
  - IPHomes - This property no longer exists, since the IPHomes/IPHome objects no longer exists.

The following items have been changed:

- Domains
   - ItemByName have been changed from a method to a property

The following methods and properties have been added to existing objects:

- Account
   - PersonFirstName - Gets or sets the first name of the account holder.
  - PersonLastName - Gets or sets the last name of the account holder.
  - Delete - Deletes the account from the database.
- Alias
   - Delete - Deletess the alias from the database.
- AntiSpam
   - CheckHostInHeloScore - Gets or sets a value determening whether hMailServer should check the senders HELO host during spam protection.
  - ClearGreylistingTriplets - Clears all greylisting triplets.
  - DNSBlackLists - Gets a collection of DNS black lists.
  - MaximumMessageSize  - Gets or sets the maximum message size hMailServer will scan for spam.
  - SpamMarkThreshold - Gets or sets the spam mark threshold.
  - SpamDeleteThreshold - Gets or sets the spam delete threshold.
  - SpamAssassinEnabled - Gets or sets a value determening whether the SpamAssassin integration is enabled.
  - SpamAssassinScore - Gets or sets the SpamAssassin score.
  - SpamAssassinMergeScore - Gets or sets a value determening whether hMailServer should use the spam score from SpamAssassin.
  - SpamAssassinHost - Gets or sets the host name of the server running SpamAssassin.
  - SpamAssassinPort - Gets or sets the port of the SpamAssassin server.
  - TarpitDelay - Gets or sets the tarpit delay.
  - TarpitCount - Gets or sets the tarpit count.
  - UseSPF - Gets or sets a value determening whether hMailServer should run SPF checks during spam protection.
  - UseSPFScore - Gets or sets the SPF score.
  - UseMXChecks - Gets or sets a value determening whether hMailServer should do MX checks during spam protection.
  - UseMXChecksScore - Gets or sets the MX checks score.
- BlockedAttachment
   - Delete - Deletes the object from the database.
- Database
   - CreateInternalDatabase - Creates an internal database using MSSQL Compact Edition.
  - CreateExternalDatabase  - Creates an external database using the given connection information.
  - DatabaseExists - Gets a value which determines whether the database settings in hMailServer.ini appear to be valid.
  - BeginTransaction - Starts a new SQL transaction.
  - CommitTransaction - Commits a SQL transaction.
  - RollbackTransaction - Rollbacks a SQL transaction.
  - ExecuteSQLScript - Executes a SQL script
  - SetDefaultDatabase - Sets the default database settings in hMailServer.ini.
  - IsConnected - Gets a value which determines whether hMailServer has been able to connect to the database.
  - ServerName - Gets the database server name hMailServer is connected to.
  - DatabasName - Gets the database name hMailServer is connected to.
- DNSBlackLists
   - Score - Gets or sets the spam score for the DNS blacklist.
  - Delete - Deletes the object from the database.
- FetchAccount
   - UseSSL - Gets or sets a value which determines whether SSL should be used when connecting to the external account server.
  - Delete - Deletes the external account from hMailServer.
  - NextDownloadTime - Gets a string containing the date and time of the next download attempt.
- GreyListingWhiteAddress
   - Delete - Deletes the item from the database.
- IMAPFolder
   - Save - Saves the changes made to the IMAP folder to the database.
  - ParentID - Gets database identifier for the parent IMAP folder.
  - Permissions - Gets a collection containing ACL permissions.
  - Delete - Deletes the object from the database.
- Language
   - Name - Gets the name of the language.
- Logging
   - CurrentAwstatsLog - Gets the name of the current awstats log file.
  - CurrentEventLog - Gets the name of the current event log file.
  - CurrentErrorLog - Gets the name of the current error log file.
  - CurrentDefaultLog - Gets the name of the current default log.
  - KeepFilesOpen - Gets or sets a value which determines whether hMailServer should keep log files open inbetween writes.
- Message  

   - Charset - Gets or sets the character set of the email message.
  - Copy - Copies the message to the specified IMAP folder (must be same user account).
- Messages  

   - Add - Adds a new message to the collection
  - Clear - Deletes all messages in the collection.
- Recipient
   - OriginalAddress - Gets the original address of the recipient (as specified during SMTP conversation)
- Route
   - UseSSL - Gets or sets a value which determines whether hMailServer should use SSL when connecting to the route.
  - Description  - Gets or sets a description of the route.
  - Delete - Deletes the route from the database.
- RouteAddress
   - Delete - Deletes the route address from the database.
- Rule
   - Delete - Deletes the rule from the database.
- RuleAction
   - HeaderName - Gets or sets the name of the header to modify
  - Value - Gets or sets the value of the header to modify.
  - Delete - Deletes the object from the database.
- RuleCriteria
   - Delete - Deletes the rule criteria from the database.
- Scripting
   - CurrentScriptFile - The script file currently in use.
- SecurityRange
   - EnableAntiVirus - Gets or sets a value which determines whether anti virus is enabled for the IP range.
  - Delete - Delets the IP range from the database.
- SecurityRanges
   - SetDefault - Reverts all IP range settings to the hMailServer defaults. Any IP range added which is not a part of the default configuration is deleted.
- ServerMessages
   - ItemByName - Returns a server message by its name.
- Settings  

   - SMTPRelayerUseSSL - Gets or sets the value that determine whether hMailServer should use SSL when communicating with the SMTP relayer.
  - SSLCertificates - Gets a collection of SSL certificates which have been set up in hMailServer.
  - AddDeliveredToHeader - Gets or sets the value that determine whether hMailServer should add a Delivered-To header to received messages.
  - IMAPPublicFolderName - Gets or sets the name of the public folder.
  - IMAPACLEnabled - Gets or sets the value that determine whether the ACL extension in the IMAP protocol should be enabled.
  - SetAdministratorPassword - Sets the hMailServer Administrator password.
  - Directories  - Gets a Directories object which contains directory related settings.
  - PublicFolders - Gets a collection of public folders.
  - PublicFolderDiskName  - Gets the name of the directory folder where public folders are stored.
  - Groups - Gets a collection of server-wide groups.
- SURBLServer
   - Score - Gets or sets the spam score for the SURBL server.
  - Delete - Deletes the SURBL server from the database.
- TCPIPPort
   - Address - Gets or sets the local IP address hMailServer should listen on.
  - UseSSL - Gets or sets a value determening whether hMailServer should expect SSL connections on this port.
  - SSLCertificateID - Gets or sets a value determening the ID of the SSL certificate to use for this port.
  - Delete - Deletes the object from the databse.
- TCPIPPorts
   - SetDefault - Reverts the TCP/IP configuration to the default values.
- Utilities  

   - IsStrongPassword - Returns true if the given password appears to be fairly strong. Note that  the requirements for a password to be considered strong are weak.
  - SHA256 - Returns a SHA256 hash for the given string.
- WhiteListAddress
   - Delete - Deletes the item from the database.
- WhiteListAddresses
   - Clear - Clears the collection

## hMailServer 4.4

### Changes breaking compatibility

No changes have been made to the hMailServer COM API which breaks compatibility with previous versions.

### New events

- OnDeliveryStart - This event is executed directly after a delivery of an email is started, before the global rules are run.

### New objects

- [WhiteListAddresses](?page=com_whitelistaddresses) - Collection of all addresses to white-list.
- [WhiteListAddress](?page=com_whitelistaddress) - An address to white-list.

### Changes to existing objects

No properties or methods have been removed in hMailServer 4.4. Below is a list of added properties

- [AntiSpam](?page=com_antispam)
   - WhiteListAddresses - Returns a collection with addressees to whitelist.  

  
- [Account](?page=com_account)
   - LastLogonTime - The timestamp for the last usage of this account. (Read-only)
  - VacationMessageExpires - Enables automatic expiration of out of office-messages.
  - VacationMessageExpiresDate - The date the out of office message should expire.  

  
- [Cache](?page=com_cache)
   - AliasCacheTTL - Cache time for aliases (seconds)
  - AliasHitRate - The hit rate in the alias cache.
  - DistributionListCacheTTL - Cache time for distribution lists (seconds)
  - DistributionListHitRate - Hit rate in the distribution list cache.

- [Domain](?page=com_domain)
   - MaxNumberOfAccounts - Maximum number of accounts which can be added to the domain. *
  - MaxNumberOfAliases - Maximum number of aliases which can be added to the domain. *
  - MaxNumberOfDistributionLists - Maximum number of distribution lists which can be added to the domain. *
  - MaxNumberOfAccountsEnabled - Limit number of accounts have been enabled. *
  - MaxNumberOfAliasesEnabled - Limit number of aliases have been enabled. *
  - MaxNumberOfDistributionListsEnabled - Limit number of distribution lists have been enabled. *
  - MaxAccountSize - The maximum size of accounts added to this domain *  

   

 ** Please note that these properties does not actually enforce the limitations. At the moment, these limitations needs to be enforced in the user interface layer of the application. (They are enforced in hMailServer Administrator and PHPWebAdmin). Enforcing this in the server core would require a redesign of the internals. This has been planned for hMailServer 5.*  

  
- [IMAPFolders](?page=com_imapfolders)
   - Add - Adds a new IMAP folder to the collection
  - DeleteByDBID - Removes an IMAP folder with the given database identifier.  

  
- [Message](?page=com_message)
   - InternalDate - The internal date of the message. (The time the message was created in hMailServer)  

  
- [Utilities](?page=com_utilities)
   - ImportMessageFromFileToIMAPFolder - Imports a message file from a directory on the disk and places it in a specific IMAP folder.  

  
- [SecurityRange](?page=com_securityrange)
   - IsForwardingRelay - Specifies whether the IP range covers a forwarding relay, for example a backup MX.  

  
- [FetchAccount](?page=com_fetchaccount)
   - DownloadNow - Forces a download of messages from the external account now.
  - ProcessMIMEDate - Enable processing of receive date in Received header.  

  
- [Rule](?page=com_rule)
   - MoveUp - Moves the rule up in the processing order.
  - MoveDown - Moves the rule down in the processing order.  

  
- [RuleAction](?page=com_ruleaction)
   - MoveUp - Moves the rule action up in the processing order.
  - MoveDown - Moves the rule action down in the processing order.

## hMailServer 4.3

### Changes breaking compatibility

In hMailServer 4.3, COM clients must call the Authenticate() function on the hMailServer.Application object prior to accessing any other function in the API. This change was made to increase the security of the hMailServer COM API.

The exception for this is the Message object. COM clients can create and send messages using the hMailServer.Message object without calling the Authenticate method.

If you are in a situation where you need to upgrade to hMailServer 4.3, but am unable to upgrade your COM client, a workaround is to set AdministratorPassword in hMailServer.ini to empty. If you do this, you do not have to call the Authenticate method()

### New objects

- [SURBLServers](?page=com_surblservers) - Collection of all SURBL servers set up in the system
- [SURBLServer](?page=com_surblserver) - Contains properties for a single SURBL server
- [AntiSpam](?page=com_antispam) - AntiSpam related settings
- [BlockedAttachments](?page=com_blockedattachments) - Collection of all objects set up in virus scanning / block attachments.
- [BlockedAttachment](?page=com_blockedattachment) - Properties for a single attachment listed under virus scanning / blocked attachments
- [ServerMessages](?page=com_servermessages) - Collection of text messages used by the server (for example, virus notification)
- [ServerMessage](?page=com_servermessage) - A single text message used by the server
- [GreyListingWhiteAddresses](fckblank.html?page=com_greylistingwhiteaddresses) - A collection of all the white addresses specified in the greylisting options
- [GreyListingWhiteAddress](?page=com_greylistingwhiteaddress) - A single grey listing white address
- [TCPIPPorts](fckblank.html?page=com_tcpippors) - A collection of all the TCP/IP ports which should be used
- [TCPIPPort](?page=com_tcpipport) - Properties for a single TCP/IP port

### New enums

- eRuleActionType, Added value: eRARunScriptFunction, - Run script function
- eAntiSpamAction - Action to take when spam has been detected
- eDistributionListMode - Distribution list mode
- eSessionType - Session type (SMTP/POP3/IMAP)
- eDomainSignatureMethod - Specfies how signatures should be applied (append, overwrite and so on)
- eMessageFlag - Contains message flags (seen, deleted and so on)

### Changes to existing objects

No properties or methods have been removed in hMailServer 4.3. Below is a list of added properties

- Application
   - Authenticate - Authenticates a client  

  
- Account
   - ForwardEnabled - Forwarding for this account is enabled.
  - ForwardAddress - Address messages to this account should be forwarded to.
  - ForwardKeepOriginal - Keep original message after forwarding has been applied.
  - SignatureEnabled - Enable the signature
  - SignaturePlainText - The plain text version of the signature  

  
- AntiVirus
   - MaximumMessageSize - Maximum message size when running anti virus (KB).
  - BlockedAttachments - Collection with all blocked attachment types
  - EnableAttachmentBlocking - Enable attachment blocking  

  
- Database
   - RequiresUpgrade - Database needs to be upgraded.  

  
- DeliveryQueue
   - Remove - Removes the message from the delivery queue.  

  
- DistributionList
   - Mode - Distribution list mode  

  
- Domain
   - PlusAddressingEnabled - Use plus addressing for this domain.
  - PlusAddressingCharacter - Plus addressing character.
  - AntiSpamEnableGreylisting - Enable greylisting for this domain.
  - MaxSize - Maximum size of this domain in MB.
  - Size - Current size of this domain in MB.
  - AllocatedSize - Allocated space for this domain.
  - SignatureEnabled - Enable the signature
  - SignatureMethod - The domain signature method
  - SignaturePlainText - The plain text version of the signature
  - SignatureHTML - The HTML version of the signature
  - AddSignaturesToReplies - Add signatures to replies on email
  - AddSignaturesToLocalMail - Add signatures to local email  

  
- FetchAccount
   - ProcessMIMERecipients - Send email to recipients in Received/To/CC headers.  

  
- Logging
   - MaskPasswordsInLog - Mask passwords before writing them to the log file.  

  
- Message
   - Flag - Returns the status of a message flag (deleted, seen, etc)  

  
- RuleAction
   - ScriptFunction - When action type set to eRARunScriptFunction, this function is executed  

  
- Status
   - SessionCount - Number of sessions per session type  

  
- Settings
   - DefaultDomain - Domain to use if domain has not been specified when user logs on.
  - SMTPDeliveryBindToIP - TCP/IP address to bind to when delivering messages to remote hosts.
  - MaxIMAPConnections - Maximum number of simultaneous IMAP connections.
  - IMAPSortEnabled - Enable the SORT IMAP extension.
  - IMAPQuotaEnabled - Enable the QUOTA IMAP extension.
  - IMAPIdleEnabled - Enable the IDLE IMAP extension.
  - WorkerThreadPriority - Priority to give to worker threads
  - TCPIPThreads - Number of TCP/IP-threads (for communication) to run.
  - AllowIncorrectLineEndings - Allow mail with incorrect line endings (missing CR or LF).
  - MaxSMTPRecipientsInBatch - Maximum number of SMTP recipients in batch during delivery.
  - AntiSpam - Returns [AntiSpam](fckblank.html?page=com_antispam) object containing Anti spam related settings.
  - DisconnectInvalidClients - Disconnect clients not following the RFCs
  - MaxNumberOfInvalidCommands - Maximum number of invalid client commands before disconnect.
  - [ServerMessages](fckblank.html?page=com_servermessages) - Returns ServerMessage object containing text strings used by the server.
  - [TCPIPPorts](fckblank.html?page=com_tcpippors) - A collection of all the TCP/IP ports which should be used  

  
- Utilities
   - IsLocalHost - Checks if the given host name points at localhost.

 
