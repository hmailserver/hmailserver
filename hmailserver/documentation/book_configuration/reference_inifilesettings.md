---
title: "Ini-file settings"
slug: reference_inifilesettings
parent: book_configuration
index: 0
is_book: false
---

# Overview

<p>Most settings in an hMailServer installation is stored in the database. However, some settings are stored in the <span class="Code">hMailServer.ini</span> file. Examples of settings stored in the ini-file are paths and database connection information. This document lists all the available settings in hMailServer.ini.</p>

<p>If you want to use a setting and it's not available in the hMailServer.ini file in your system, you can add the setting yourself. For example, to add the setting <span class="Code">ConnectionAttempts</span> to the <span class="Code">Database</span> section, simply add the line <span class="Code">ConnectionAttempts=5</span> below the line <span class="Code">[Database]</span> in hMailServer.ini. In some cases, you may need to add the actual section ([SectionName]) as well. If the section already exists in the file, you should add the setting to that file. You cannot have two ini file sections with the same name in the same ini-file.</p>

 

## Sections

### Directories

- ProgramFolder - The path to the hMailServer directory. By default, C:\Program Files\hMailServer.
- DataFolder - The path to the hMailServer data directory. By default, C:\Program Files\hMailServer\Data.
- LogFolder - The path where hMailServer logs are stored. By default, C:\Program Files\hMailServer\Logs
- TempFolder - The path where hMailServer stores temporary files, such as attachments during virus scanning. By default C:\Program Files\hMailServer\Temp
- EventFolder - The path where the hMailServer event file is located. By default, C:\Program Files\hMailServer\Events
- DatabaseFolder - The path where the database files are stored, when hMailServer uses a database it manages itself. This is used by the internal database, and is empty when hMailServer connects to an external database server.

### GUILanguages

- ValidLanguages - A list of valid hMailServer user interface languages. hMailServer Administrator uses this list to determine which languages to display in the *Language* menu.

### Database

- Internal - 1 if the internal MySQL database is used, 0 otherwise. hMailServer uses this setting to determine whether scripts should be applied to the MySQL database on the first launch. For example, if a new version of MySQL is included with the installation program, hMailServer might run SQL scripts to patch it.
- Type - Type of database. Can be MySQL, MSSQL, PostgreSQL or MSSQLCE. hMailServer uses it to determine what method to use to connect to the database server, and which syntax to use for SQL statements.
- Username - hMailServer will use this username when connecting to the database server. If it's left empty, and MSSQL is used, hMailServer will try to use Windows Authentication.
- Password - The password hMailServer should use when connecting to the database server. If the passwordencryption is set to 1, the password is encrypted using blowfish.
- Passwordencryption - If set to 1, the database password is encrypted using blowfish. In this case, the hMailServer service decodes the password before connecting to the database.
- Port - The port hMailserver should connect to on the database server.
- Server - The database server host name hMailServer should connect to.
- Database - The name of the database hMailServer should try to use.
- NumberOfConnections - The number of connections should open to the database. The default value of this setting is 5, which means that hMailServer will open 5 connections to the database server. hMailServer often wants to execute several database queries at the same time. Since a specific database connection can only be used for one SQL statement at a time, multiple database connections improves performance.
- ConnectionAttempts - The number of times hMailServer should try to connect to the database before giving up on start-up. Default 6 times. (hMailServer 4.4 and later)
- ConnectionAttemptsDelay - The number of seconds hMailServer should pause between each connection attempt during start-up. Default 5 seconds. (hMailServer 4.4 and later)
- Provider - The OLE DB provider hMailServer should use when connecting to Microsoft SQL Server. If this setting is empty, hMailServer selects a provider itself, preferring MSOLEDBSQL version 18 or later when it is installed, since that version supports TLS 1.2.
- ServerFailoverPartner - The name of the failover partner server, when connecting to a Microsoft SQL Server which is set up for database mirroring.

### Security

- AdministratorPassword - The main hMailServer administration password. The user for example needs to enter this password when starting hMailServer Administrator. This password is encoded using MD5.

### Settings

The settings below are not available in hMailServer Administrator, and should be edited carefully. They all belong to the [Settings] section of hMailServer.ini. The sub headings below are only used to group the settings in this document; they are not ini-file sections.

#### User interface

- UseLanguage - The user interface language hMailServer Administrator uses. The value must be one of the languages listed in ValidLanguages. Default English. hMailServer Administrator writes this setting itself when you select a language from the *File* menu.

#### Message delivery

- RewriteEnvelopeFromWhenForwarding (from version 5.6.2 b 2223) - When performing forwarding, hMailServer now keeps the original From address rather than changing to that of the forwarding account. This change was made to reduce risk of message delivery failures. To force the previous behavior, set RewriteEnvelopeFromWhenForwarding=1
- QuickRetries - The number of delivery retries, at the beginning of a message's life, which should be made using the shorter QuickRetriesMinutes interval rather than the retry interval configured for the SMTP delivery. This is intended for remote servers which use greylisting, where a quick second attempt is often enough to get the message delivered. Default 0, which means that this feature is disabled.
- QuickRetriesMinutes - The number of minutes hMailServer should wait between the quick retries described above. Default 6 minutes.
- QueueRandomnessMinutes - If set to a value larger than 0, hMailServer adds a random delay of up to this many minutes to the time when a message should be retried. This spreads out the delivery queue rather than having many messages become due at the same time. Default 0, which means that no randomness is added.
- MXTriesFactor - Limits how many of a domain's mail servers hMailServer tries during a single delivery attempt. The limit is this value multiplied by the number of retries already made for the message, so later attempts try more servers. Default 0, which means that all mail servers are tried during every attempt.
- SMTPDMaxSizeDrop - If set to a value larger than 0, hMailServer stops receiving a message as soon as it has grown larger than this many KB, replies with a 552 error and disconnects the client. Compared to the maximum message size configured in hMailServer Administrator, which is checked after the message has been received, this avoids spending bandwidth on messages which are going to be rejected anyway. Default 0, which means that the message is always received in full.
- MaxNumberOfExternalFetchThreads - The number of external accounts hMailServer downloads from at the same time. Default 15.
- DaemonAddressDomain - The domain name hMailServer should use in the mailer-daemon@ address it sends delivery failure messages from. If this setting is empty, hMailServer uses the domain of the original recipient if it is local, and otherwise the computer name.

#### Message headers

- AddXAuthUserHeader - If set to 1, hMailSever will add a X-AuthUser header containing a username to messages received using SMTP, if the user has authenticated.
- AddXAuthUserIP - If set to 1, and AuthUserReplacementIP is used, hMailServer adds a X-AuthUserIP header containing the real IP address the authenticated user connected from. Default 1.
- AuthUserReplacementIP - If this setting contains an IP address, hMailServer writes that address instead of the real client address in the Received header of messages sent by authenticated users. This hides the IP addresses of your users from the recipients. The real address is still available in the X-AuthUserIP header, unless AddXAuthUserIP has been set to 0.
- AddXOriginalRcptTo - If set to 1, hMailServer adds a X-OriginalRcptTo header containing the original recipient address to messages. Default 0.

#### Anti-spam and anti-virus

- DNSBLChecksAfterMailFrom - By default, hMailServer runs DNS blacklists checks after SMTP/MAIL FROM. Some users prefer to have it running after the SMTP/RCPT TO command. In this case, set the value of this setting to 0.
- GreylistingEnabledDuringRecordExpiration - This setting lets you configure hMailServer to temporarily disable grey listing functionality while old grey listing records are cleaned away. This may be required if you have a large amount of greylisting records and are using SQL. When hMailServer deletes old records, the entire greylisting table will become locked for a long time. If other database connections tries to access the table meanwhile, they will have to wait for the deletion to complete. If this takes several minutes, this is likely to cause problems. Default value is 1, which means that hMailServer will continue to use grey listing when deleting records from the database.
- GreylistingRecordExpirationInterval - This setting defines how often hMailserver should delete expired greylisting records from the database. Deleting records may be a time consuming task. The default value is 240, which means that hMailServer will clear expired records every 240 minute (every 4th hour).
- SAMinTimeout / SAMaxTimeout - The timeout, in seconds, hMailServer uses when communicating with SpamAssassin. Default 30 and 90 seconds. See *Connection timeouts* below for how the two values are used.
- SAMoveVsCopy - Controls how hMailServer replaces the message file with the copy SpamAssassin has processed. If set to 1, the file is moved, which is faster. Default 0, which means that the file is copied and the temporary file then deleted.
- ClamMinTimeout / ClamMaxTimeout - The timeout, in seconds, hMailServer uses when communicating with ClamAV. Default 15 and 90 seconds. See *Connection timeouts* below for how the two values are used.

#### Connection timeouts

For each of the settings below, hMailServer normally uses the Max value. When the server is under high load, the timeout is lowered to 60 seconds, but never below the Min value. Lowering the timeout under load makes hMailServer drop idle sessions sooner, so that connections become available to other clients.

- SMTPDMinTimeout / SMTPDMaxTimeout - Timeout, in seconds, for SMTP sessions where a client is delivering to hMailServer. Default 10 and 1800 seconds.
- SMTPCMinTimeout / SMTPCMaxTimeout - Timeout, in seconds, for SMTP sessions where hMailServer is delivering to another server. Default 30 and 600 seconds.
- POP3DMinTimeout / POP3DMaxTimeout - Timeout, in seconds, for POP3 sessions where a client is downloading from hMailServer. Default 10 and 600 seconds.
- POP3CMinTimeout / POP3CMaxTimeout - Timeout, in seconds, for POP3 sessions where hMailServer is downloading from an external account. Default 30 and 900 seconds.
- BlockedIPHoldSeconds - The number of seconds hMailServer should keep a refused connection open before dropping it. Connections are refused when the client does not match any IP range, or when a connection limit has been reached. Holding the connection slows down clients which reconnect over and over. Default 0, which means that refused connections are dropped immediately.

#### Logging

- SepSvcLogs - If set to 1, hMailServer writes SMTP, POP3 and IMAP logging to one log file per protocol, instead of writing all of them to the same file. Default 0.
- LogLevel - Controls how much protocol logging hMailServer writes. With the default value 9, everything is logged. With a value of 2 or lower, IMAP FETCH and STATUS responses, which make up the bulk of IMAP logging, are left out.
- MaxLogLineLen - The maximum length of a logged line. Longer lines are shortened, with the middle part replaced by ... . Values below 100 are treated as 100. Default 500.

#### DNS

- UseDNSCache - If set to 0, hMailServer bypasses the DNS cache of the operating system and asks the DNS server directly for every lookup. Default 1.
- DNSServer - The IPv4 address of a DNS server hMailServer should send its DNS queries to, instead of the DNS servers configured in Windows.

#### Performance and storage

- IndexerFullMinutes - The number of minutes between full runs of the message indexer. The indexer runs once a minute; most runs only look at recently added messages, and every IndexerFullMinutes:th run looks at all messages which have not been indexed yet. Default 720 minutes (12 hours).
- IndexerFullLimit - The maximum number of messages a full indexer run indexes. Default 25000.
- IndexerQuickLimit - How many of the most recent messages a quick indexer run looks at. Default 1000.
- LoadHeaderReadSize - The number of bytes hMailServer reads at a time from a message file when it loads the message headers. Default 4000 bytes. The maximum value is 50000.
- LoadBodyReadSize - The number of bytes hMailServer reads at a time from a message file when it loads the message body. Default 4000 bytes. The maximum value is 50000.
- BackupMessagesDBOnly - If set to 1, hMailServer includes the message database entries, but not the message files, in backups. This is useful when the message files are backed up by some other software. The same setting is used during restore. Default 0.
- ArchiveDir - If this setting contains a path, hMailServer stores an extra copy of every message it receives over SMTP below that directory. Messages from local senders are stored per domain and user, and messages from external senders in a common Inbound directory. If the setting is empty, no archiving is done.
- ArchiveHardLinks - If set to 1, hMailServer creates the additional archive copies of a message as hard links to the first copy, instead of copying the file once per recipient. This saves disk space, but requires that the archive directory is on an NTFS volume. Default 0.

#### Other

- DisableAUTHList (from 5.6.3) -  Setting DisableAUTHList allows you to specify a comma-separated list of SMTP ports which authentication should not be enabled for. This is useful when working with legacy systems with malfunctioning SMTP support.
- PreferredHashAlgorithm  - This setting allows you to specify which hashing algorithm hMailServer should use for passwords in the hMailServer database. In old versions of hMailServer, passwords were stored in plain text. In hMailServer 4, passwords were stored in MD5. In hMailServer 5, the default preferred hash is now salted SHA256. The following values are valid for this setting:  

     0 - None. Store passwords in clear text. This is not recommended.  

     1 - Blowfish. Store passwords encrypted using Blowfish. This is not recommended, since the password used for encryption is known. Hence, this is no more safe than option 0.  

     2 - MD5. Store passwords in MD5 hash. This is only recommended to preserve backwards compatibility if you have application which integrates with the hMailServer database.  

     3 - SHA256 - Store passwords in SHA256 hashes. This is currently the recommended option which gives the highest level of security.
