---
title: "Settings object"
slug: com_object_settings
parent: com_objects
index: 0
is_book: false
---

### Description

The Settings object is the root-object for all hMailServer settings which are not related to a specific domain.

### Methods

<div class="api_method_name">ClearLogonFailureList()</div>

<div class="api_description">Clears the list of logon failures.</div>

<div class="api_method_name">SetAdministratorPassword(string newVal)</div>

<div class="api_description">Set main hMailServer Administration password</div>

<div class="api_method_name">SetSMTPRelayerPassword(string newVal)</div>

<div class="api_description">Set SMTP relayer password</div>

### Properties

<div class="api_method_name">bool AddDeliveredToHeader</div>

<div class="api_description">Determines whether a Delivered-To header should be added to incoming messages.<br></div>

<div class="api_method_name">bool AllowIncorrectLineEndings</div>

<div class="api_description">Allow mail with incorrect line endings (missing CR or LF).<br></div>

<div class="api_method_name">bool AllowSMTPAuthPlain</div>

<div class="api_description">Allow plain SMTP authentication.<br></div>

<div class="api_method_name"><a href="?page=com_object_antispam">AntiSpam</a> AntiSpam</div>

<div class="api_description">AntiSpam settings<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_antivirus">AntiVirus</a> AntiVirus</div>

<div class="api_description">AntiVirus Settings.<br> <i>(read-only)</i></div>

<div class="api_method_name">long AutoBanMinutes</div>

<div class="api_description">Gets or sets the number of minutes a client is auto banned.<br></div>

<div class="api_method_name">bool AutoBanOnLogonFailure</div>

<div class="api_description">Gets or sets whether auto ban on logon failure is enabled.<br></div>

<div class="api_method_name"><a href="?page=com_object_backupsettings">BackupSettings</a> Backup</div>

<div class="api_description">Gets the backup settings object<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_cache">Cache</a> Cache</div>

<div class="api_description">Cache settings<br> <i>(read-only)</i></div>

<div class="api_method_name">long CrashSimulationMode</div>

<div class="api_description">Crash simulation mode<br></div>

<div class="api_method_name">bool CreateDefaultSpecialUseFoldersEnabled</div>

<div class="api_description">Create default special-use folders (Sent, Drafts, Trash, Junk) for new accounts.<br></div>

<div class="api_method_name">string DefaultDomain</div>

<div class="api_description">Domain to use if domain has not been specified when user logs on.<br></div>

<div class="api_method_name">bool DenyMailFromNull</div>

<div class="api_description">Deny mail with empty sender address.<br></div>

<div class="api_method_name"><a href="?page=com_object_directories">Directories</a> Directories</div>

<div class="api_description">Gets an object containing hMailServer directory settings.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool DisconnectInvalidClients</div>

<div class="api_description">Disconnect clients not following the RFCs<br></div>

<div class="api_method_name"><a href="?page=com_object_groups">Groups</a> Groups</div>

<div class="api_description">A collection of groups.<br> <i>(read-only)</i></div>

<div class="api_method_name">string HostName</div>

<div class="api_description">Gets or sets the host name hMailServer identifies itself with.<br></div>

<div class="api_method_name">bool IMAPACLEnabled</div>

<div class="api_description">Enable the IDLE ACL extension.<br></div>

<div class="api_method_name">string IMAPHierarchyDelimiter</div>

<div class="api_description">Gets or sets the hierarchy delimiter to use in the IMAP protocol.<br></div>

<div class="api_method_name">bool IMAPIdleEnabled</div>

<div class="api_description">Enable the IDLE IMAP extension.<br></div>

<div class="api_method_name">string IMAPMasterUser</div>

<div class="api_description">IMAP Master user<br></div>

<div class="api_method_name">string IMAPPublicFolderName</div>

<div class="api_description">The name of the public folder when IMAP is used.<br></div>

<div class="api_method_name">bool IMAPQuotaEnabled</div>

<div class="api_description">Enable the QUOTA IMAP extension.<br></div>

<div class="api_method_name">bool IMAPSASLInitialResponseEnabled</div>

<div class="api_description">IMAP Enable SASL Plain Initial Response<br></div>

<div class="api_method_name">bool IMAPSASLPlainEnabled</div>

<div class="api_description">IMAP Enable SASL Plain<br></div>

<div class="api_method_name">bool IMAPSortEnabled</div>

<div class="api_description">Enable the SORT IMAP extension.<br></div>

<div class="api_method_name"><a href="?page=com_object_incomingrelays">IncomingRelays</a> IncomingRelays</div>

<div class="api_description">A collection of all incoming relays.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool IPv6PreferredEnabled</div>

<div class="api_description">Prefer IPv6 over IPv4.<br></div>

<div class="api_method_name"><a href="?page=com_object_logging">Logging</a> Logging</div>

<div class="api_description">Gets the logging interface.<br> <i>(read-only)</i></div>

<div class="api_method_name">long MaxAsynchronousThreads</div>

<div class="api_description">Gets or sets the maximum number of asynchronous task threads.<br></div>

<div class="api_method_name">long MaxDeliveryThreads</div>

<div class="api_description">The maximum number of SMTP delivery threads.<br></div>

<div class="api_method_name">long MaxIMAPConnections</div>

<div class="api_description">Maximum number of simultaneous IMAP connections.<br></div>

<div class="api_method_name">long MaxInvalidLogonAttempts</div>

<div class="api_description">Gets or sets the maximum number of invalid logon attempts before auto ban.<br></div>

<div class="api_method_name">long MaxInvalidLogonAttemptsWithin</div>

<div class="api_description">Gets or sets the number of minutes before an invalid logon attempt record is removed.<br></div>

<div class="api_method_name">long MaxMessageSize</div>

<div class="api_description">The maximum message size when sending from this server.<br></div>

<div class="api_method_name">long MaxNumberOfInvalidCommands</div>

<div class="api_description">Maximum number of invalid client commands before disconnect.<br></div>

<div class="api_method_name">long MaxNumberOfMXHosts</div>

<div class="api_description">Gets or sets the maximum number of hosts hMailServer should perform SMTP-delivery to.<br></div>

<div class="api_method_name">long MaxPOP3Connections</div>

<div class="api_description">Gets or sets the maximum number of simultaneous POP3 connections.<br></div>

<div class="api_method_name">long MaxSMTPConnections</div>

<div class="api_description">Gets or sets the maximum number of simultaneous SMTP connections.<br></div>

<div class="api_method_name">long MaxSMTPRecipientsInBatch</div>

<div class="api_description">Maximum number of SMTP recipients in batch during delivery.<br></div>

<div class="api_method_name"><a href="?page=com_object_messageindexing">MessageIndexing</a> MessageIndexing</div>

<div class="api_description">Gets an object containing settings related to message indexing.<br> <i>(read-only)</i></div>

<div class="api_method_name">string MirrorEMailAddress</div>

<div class="api_description">A copy of all e-mails are sent to the address.<br></div>

<div class="api_method_name">long PasswordHashAlgorithm</div>

<div class="api_description"> Algorithm used to hash account passwords. 1 = Argon2id, 2 = PBKDF2-SHA256.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool PasswordHashAutoUpgradeEnabled</div>

<div class="api_description"> Re-hash account passwords during logon when they use an older algorithm or a lower cost.</div>

<div class="api_description"> </div>

<div class="api_method_name">long PasswordHashIterations</div>

<div class="api_description"> Iteration count used when hashing account passwords. 0 means the recommended default of the selected algorithm: 2 for Argon2id, 600000 for PBKDF2-SHA256.</div>

<div class="api_description"> </div>

<div class="api_method_name">long PasswordHashMemoryCost</div>

<div class="api_description"> Memory cost in kilobytes used when hashing account passwords with Argon2id. 0 means the recommended default of 19456 KiB (19 MiB).</div>

<div class="api_description"> </div>

<div class="api_method_name">string PublicFolderDiskName</div>

<div class="api_description">The name of the folder on disk in which public folders are stored.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_imapfolders">IMAPFolders</a> PublicFolders</div>

<div class="api_description">Gets a collection all public folders.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool RewriteEnvelopeFromWhenForwarding</div>

<div class="api_description">Rewrite envelope From address to the forwarding account address when forwarding.<br></div>

<div class="api_method_name"><a href="?page=com_object_routes">Routes</a> Routes</div>

<div class="api_description">Routes collection.<br> <i>(read-only)</i></div>

<div class="api_method_name">long RuleLoopLimit</div>

<div class="api_description">The maximum number of times a message can be processed by rules.<br></div>

<div class="api_method_name"><a href="?page=com_object_scripting">Scripting</a> Scripting</div>

<div class="api_description">Script settings<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_securityranges">SecurityRanges</a> SecurityRanges</div>

<div class="api_description">Security ranges.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_servermessages">ServerMessages</a> ServerMessages</div>

<div class="api_description">Server messages<br> <i>(read-only)</i></div>

<div class="api_method_name">bool ServiceIMAP</div>

<div class="api_description">Enable IMAP service.<br></div>

<div class="api_method_name">bool ServicePOP3</div>

<div class="api_description">Enable POP3 service.<br></div>

<div class="api_method_name">bool ServiceSMTP</div>

<div class="api_description">Enable SMTP service.<br></div>

<div class="api_method_name">eConnectionSecurity SMTPConnectionSecurity</div>

<div class="api_description">Connection security to use when delivering after a MX lookup<br></div>

<div class="api_method_name">string SMTPDeliveryBindToIP</div>

<div class="api_description">TCP/IP address to bind to when delivering messages to remote hosts.<br></div>

<div class="api_method_name">long SMTPMinutesBetweenTry</div>

<div class="api_description">Number of minutes between each try when delivering an e-mail.<br></div>

<div class="api_method_name">long SMTPNoOfTries</div>

<div class="api_description">Number of retries when SMTP server delivers an e-mail.<br></div>

<div class="api_method_name">string SMTPRelayer</div>

<div class="api_description">Fixed relaying server.<br></div>

<div class="api_method_name">eConnectionSecurity SMTPRelayerConnectionSecurity</div>

<div class="api_description">Connection security for the SMTP relayer.<br></div>

<div class="api_method_name">long SMTPRelayerPort</div>

<div class="api_description">Fixed relaying server port.<br></div>

<div class="api_method_name">bool SMTPRelayerRequiresAuthentication</div>

<div class="api_description">SMTP relayer requires authentication<br></div>

<div class="api_method_name">string SMTPRelayerUsername</div>

<div class="api_description">SMTP relayer username<br></div>

<div class="api_method_name">bool SMTPRelayerUseSSL</div>

<div class="api_description">Determines whether SSL should be used when delivering to the relay server.<br></div>

<div class="api_method_name">bool SRSEnabled</div>

<div class="api_description">Rewrite the envelope sender using the Sender Rewriting Scheme (SRS) when forwarding messages to other servers.<br></div>

<div class="api_method_name">long SRSHashLength</div>

<div class="api_description">The number of hash characters in an SRS address. Longer is harder to forge, shorter makes for a shorter address. Valid values are 4 to 20; 0 selects the default of 8.<br></div>

<div class="api_method_name">long SRSMaxAgeDays</div>

<div class="api_description">The number of days an SRS address stays valid, after which a bounce sent to it is rejected. Valid values are 1 to 512; 0 selects the default of 21.<br></div>

<div class="api_method_name">string SRSSecret</div>

<div class="api_description">The secret SRS addresses are signed with. The server generates one on first start, so this normally does not need to be set. Setting it to an empty string generates a new one, which makes every address already handed out impossible to reverse.<br></div>

<div class="api_method_name"><a href="?page=com_object_sslcertificates">SSLCertificates</a> SSLCertificates</div>

<div class="api_description">SSL certificates.<br> <i>(read-only)</i></div>

<div class="api_method_name">string SslCipherList</div>

<div class="api_description">List of SSL/TLS ciphers which should be permitted (OpenSSL format).<br></div>

<div class="api_method_name"><a href="?page=com_object_tcpipports">TCPIPPorts</a> TCPIPPorts</div>

<div class="api_description">TCP/IP ports to be used.<br> <i>(read-only)</i></div>

<div class="api_method_name">long TCPIPThreads</div>

<div class="api_description">Number of TCP/IP-threads (for communication) to run.<br></div>

<div class="api_method_name">bool TlsOptionPreferServerCiphersEnabled</div>

<div class="api_description">Prefer server cipher order.<br></div>

<div class="api_method_name">bool TlsOptionPrioritizeChaChaEnabled</div>

<div class="api_description">Prioritize ChaCha20 ciphers when client prefers them.<br></div>

<div class="api_method_name">bool TlsVersion10Enabled</div>

<div class="api_description">Enable TLS version 1.0.<br></div>

<div class="api_method_name">bool TlsVersion11Enabled</div>

<div class="api_description">Enable TLS version 1.1.<br></div>

<div class="api_method_name">bool TlsVersion12Enabled</div>

<div class="api_description">Enable TLS version 1.2.<br></div>

<div class="api_method_name">bool TlsVersion13Enabled</div>

<div class="api_description">Enable TLS version 1.3<br></div>

<div class="api_method_name">string UserInterfaceLanguage</div>

<div class="api_description">User interface language<br></div>

<div class="api_method_name">bool VerifyRemoteSslCertificate</div>

<div class="api_description">Verify the SSL/TLS server certificate.<br></div>

<div class="api_method_name">string WelcomeIMAP</div>

<div class="api_description">Welcome message for IMAP server.<br></div>

<div class="api_method_name">string WelcomePOP3</div>

<div class="api_description">Welcome message for POP3 server.<br></div>

<div class="api_method_name">string WelcomeSMTP</div>

<div class="api_description">Welcome message for SMTP server.<br></div>

<div class="api_method_name">long WorkerThreadPriority</div>

<div class="api_description">Priority to give to worker threads.<br></div>
