---
id: 343
title: "Settings object"
slug: com_object_settings
parent: com_objects
index: 0
is_book: false
updated: 2017-02-14
---

### Description

The Settings object is the root-object for all hMailServer settings which are not related to a specific domain.

### Methods

<div class="api_method_name">ClearLogonFailureList</div>

<div class="api_description"> Clears the list of logon failures</div>

<div class="api_description"> </div>

<div class="api_method_name">SetAdministratorPassword(string newVal)</div>

<div class="api_description"> Set main hMailServer Administration password</div>

<div class="api_description"> </div>

<div class="api_method_name">SetSMTPRelayerPassword(string newVal)</div>

<div class="api_description"> Set SMTP relayer password</div>

### Properties

<div class="api_method_name">bool AddDeliveredToHeader</div>

<div class="api_description"> Determines whether a Delivered-To header should be added to incoming messages.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool AllowIncorrectLineEndings</div>

<div class="api_description"> Allow mail with incorrect line endings (missing CR or LF).</div>

<div class="api_description"> </div>

<div class="api_method_name">bool AllowSMTPAuthPlain</div>

<div class="api_description"> Allow plain SMTP authentication.</div>

<div class="api_method_name"> </div>

<div class="api_method_name"><a href="?page=com_object_antispam">AntiSpam</a> AntiSpam</div>

<div class="api_description"> AntiSpam settings<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_antivirus">AntiVirus</a> AntiVirus</div>

<div class="api_description"> AntiVirus Settings.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">Long AutoBanMinutes</div>

<div class="api_description"> The number of minutes that a client is autobanned for</div>

<div class="api_description"> </div>

<div class="api_method_name">bool AutoBanOnLogonFailure</div>

<div class="api_description"> Gets or sets whether AutoBan is on or off</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_backupsettings">BackupSettings</a> Backup</div>

<div class="api_description"> Gets the backup settings object<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_cache">Cache</a> Cache</div>

<div class="api_description"> Cache settings<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">string DefaultDomain</div>

<div class="api_description"> Domain to use if domain has not been specified when user logs on.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool DenyMailFromNull</div>

<div class="api_description"> Deny mail with empty sender address.</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_directories">Directories</a> Directories</div>

<div class="api_description"> Directory settings<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">bool DisconnectInvalidClients</div>

<div class="api_description"> Disconnect clients not following the RFCs</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_groups">Groups</a> Groups</div>

<div class="api_description"> A collection of groups.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">string HostName</div>

<div class="api_description"> Gets or sets the host name hMailServer identifies itself with.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool IMAPACLEnabled</div>

<div class="api_description"> Enable the IDLE ACL extension.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool IMAPIdleEnabled</div>

<div class="api_description"> Enable the IDLE IMAP extension.</div>

<div class="api_description"> </div>

<div class="api_method_name">string IMAPPublicFolderName</div>

<div class="api_description"> The name of the public folder when IMAP is used.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool IMAPQuotaEnabled</div>

<div class="api_description"> Enable the QUOTA IMAP extension.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool IMAPSortEnabled</div>

<div class="api_description"> Enable the SORT IMAP extension.</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="https://www.hmailserver.com/documentation/latest/?page=com_object_incomingrelays">IncomingRelays</a> IncomingRelays</div>

<div class="api_description"> A collection of all incoming relays</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_logging">Logging</a> Logging</div>

<div class="api_description"> Gets the logging interface.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxAsynchronousThreads</div>

<div class="api_description"> Gets or sets the maximum number of asynchronous task threads.</div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxDeliveryThreads</div>

<div class="api_description"> The maximum number of SMTP delivery threads.</div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxIMAPConnections</div>

<div class="api_description"> Maximum number of simultaneous IMAP connections.</div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxInvalidLogonAttempts</div>

<div class="api_description"> Gets or sets the maximum number of invalid logon attempts before auto ban.</div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxInvalidLogonAttemptsWithin</div>

<div class="api_description"> Gets or sets the number of minutes before an invalid logon attempt record is removed.</div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxMessageSize</div>

<div class="api_description"> The maximum message size when sending from this server.</div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxNumberOfInvalidCommands</div>

<div class="api_description"> Maximum number of invalid client commands before disconnect.</div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxNumberOfMXHosts</div>

<div class="api_description"> Gets or sets the maximum number of hosts hMailServer should perform SMTP-delivery to.</div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxPOP3Connections</div>

<div class="api_description"> Gets or sets the maximum number of simultaneous POP3 connections.</div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxSMTPConnections</div>

<div class="api_description"> Gets or sets the maximum number of simultaneous SMTP connections.</div>

<div class="api_description"> </div>

<div class="api_method_name">long MaxSMTPRecipientsInBatch</div>

<div class="api_description"> Maximum number of SMTP recipients in batch during delivery.</div>

<div class="api_description"> </div>

<div class="api_method_name">string MirrorEMailAddress</div>

<div class="api_description"> A copy of all e-mails are sent to the address.</div>

<div class="api_description"> </div>

<div class="api_method_name">string PublicFolderDiskName</div>

<div class="api_description"> The name of the folder on disk in which public folders are stored.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_imapfolders">IMAPFolders</a> PublicFolders</div>

<div class="api_description"> Public folders<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_routes">Routes</a> Routes</div>

<div class="api_description"> Routes collection.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">long RuleLoopLimit</div>

<div class="api_description"> The maximum number of times a message can be processed by rules.</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_scripting">Scripting</a> Scripting</div>

<div class="api_description"> Script settings<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_securityranges">SecurityRanges</a> SecurityRanges</div>

<div class="api_description"> Security ranges.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">bool SendStatistics</div>

<div class="api_description"> Gets or sets whether statistics should be sent to hMailServer.com</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_servermessages">ServerMessages</a> ServerMessages</div>

<div class="api_description"> Server messages<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">bool ServiceIMAP</div>

<div class="api_description"> Enable IMAP service.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool ServicePOP3</div>

<div class="api_description"> Enable POP3 service.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool ServiceSMTP</div>

<div class="api_description"> Enable SMTP service.</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_constants">eConnectionSecurity</a> SMTPConnectionSecurity</div>

<div class="api_description"> Connection security to use when delivering after a MX lookup.</div>

<div class="api_description"> </div>

<div class="api_method_name">string SMTPDeliveryBindToIP</div>

<div class="api_description"> TCP/IP address to bind to when delivering messages to remote hosts.</div>

<div class="api_description"> </div>

<div class="api_method_name">long SMTPMinutesBetweenTry</div>

<div class="api_description"> Number of minutes between each try when delivering an e-mail.</div>

<div class="api_description"> </div>

<div class="api_method_name">long SMTPNoOfTries</div>

<div class="api_description"> Number of retries when SMTP server delivers an e-mail.</div>

<div class="api_description"> </div>

<div class="api_method_name">string SMTPRelayer</div>

<div class="api_description"> Fixed relaying server.</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_constants">eConnectionSecurity</a> SMTPRelayerConnectionSecurity</div>

<div class="api_description"> Connection security for the SMTP relayer.</div>

<div class="api_description"> </div>

<div class="api_method_name">long SMTPRelayerPort</div>

<div class="api_description"> Fixed relaying server port.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool SMTPRelayerRequiresAuthentication</div>

<div class="api_description"> SMTP relayer requires authentication</div>

<div class="api_description"> </div>

<div class="api_method_name">string SMTPRelayerUsername</div>

<div class="api_description"> SMTP relayer username</div>

<div class="api_description"> </div>

<div class="api_method_name">bool SMTPRelayerUseSSL</div>

<div class="api_description"> Determines whether SSL should be used when delivering to the relay server.</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_sslcertificates">SSLCertificates</a> SSLCertificates</div>

<div class="api_description"> SSL certificates.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">string SslCipherList</div>

<div class="api_description"> List of SSL/TLS ciphers which should be permitted (OpenSSL format).</div>

<div class="api_description"> </div>

<div class="api_method_name">bool SslVersion30Enabled</div>

<div class="api_description"> Enable SSL version 3.0</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_tcpipports">TCPIPPorts</a> TCPIPPorts</div>

<div class="api_description"> TCP/IP ports to be used.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">long TCPIPThreads</div>

<div class="api_description"> Number of TCP/IP-threads (for communication) to run.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool TlsVersion10Enabled</div>

<div class="api_description"> Enable TLS version 1.0</div>

<div class="api_description"> </div>

<div class="api_method_name">bool TlsVersion11Enabled</div>

<div class="api_description"> Enable TLS version 1.1</div>

<div class="api_description"> </div>

<div class="api_method_name">bool TlsVersion12Enabled</div>

<div class="api_description"> Enable TLS version 1.2</div>

<div class="api_description"> </div>

<div class="api_method_name">string UserInterfaceLanguage</div>

<div class="api_description"> User interface language</div>

<div class="api_description"> </div>

<div class="api_method_name">bool VerifyRemoteSslCertificate</div>

<div class="api_description"> Verify the SSL/TLS server certificate.</div>

<div class="api_description"> </div>

<div class="api_method_name">string WelcomeIMAP</div>

<div class="api_description"> Welcome message for IMAP server.</div>

<div class="api_description"> </div>

<div class="api_method_name">string WelcomePOP3</div>

<div class="api_description"> Welcome message for POP3 server.</div>

<div class="api_description"> </div>

<div class="api_method_name">string WelcomeSMTP</div>

<div class="api_description"> Welcome message for SMTP server.</div>

<div class="api_description"> </div>

<div class="api_method_name">long WorkerThreadPriority</div>

<div class="api_description"> Priority to give to worker threads.</div>
