// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#define PROPERTY_MAXPOP3CONNECTIONS             _T("maxpop3connections")
#define PROPERTY_MAXSMTPCONNECTIONS             _T("maxsmtpconnections")
#define PROPERTY_MIRROREMAILADDRESS             _T("mirroremailaddress")
#define PROPERTY_AUTHALLOWPLAINTEXT             _T("authallowplaintext")
#define PROPERTY_ALLOWMAILFROMNULL              _T("allowmailfromnull")
#define PROPERTY_LOGDEVICE                      _T("logdevice")
#define PROPERTY_LOGGING                        _T("logging")
#define PROPERTY_SMTPNOOFTRIES                  _T("smtpnoofretries")
#define PROPERTY_SMTPMINUTESBETWEEN             _T("smtpminutesbetweenretries")
#define PROPERTY_WELCOMEIMAP                    _T("welcomeimap")
#define PROPERTY_WELCOMEPOP3                    _T("welcomepop3")
#define PROPERTY_WELCOMESMTP                    _T("welcomesmtp")
#define PROPERTY_SMTPRELAYER                    _T("smtprelayer")
#define PROPERTY_PROTOCOLSMTP                   _T("protocolsmtp")
#define PROPERTY_PROTOCOLPOP3                   _T("protocolpop3")
#define PROPERTY_PROTOCOLIMAP                   _T("protocolimap")
#define PROPERTY_LOGFORMAT                      _T("logformat")
#define PROPERTY_MAXDELIVERYTHREADS             _T("maxdelivertythreads")
#define PROPERTY_AV_CLAMWIN_ENABLE              _T("avclamwinenable")
#define PROPERTY_AV_CLAMWIN_EXEC                _T("avclamwinexec")
#define PROPERTY_AV_CLAMWIN_DB                  _T("avclamwindb")
#define PROPERTY_AV_NOTIFY_SENDER               _T("avnotifysender")
#define PROPERTY_AV_NOTIFY_RECEIVER             _T("avnotifyreceiver")
#define PROPERTY_AV_DELETE_INFC_EMAIL           _T("avdeleteinfectedemail")
#define PROPERTY_AV_ACTION                      _T("avaction")
#define PROPERTY_SENDSTATISTICS                 _T("sendstatistics")  
#define PROPERTY_HOSTNAME                       _T("hostname")
#define PROPERTY_SMTPRELAYER_USERNAME           _T("smtprelayerusername")
#define PROPERTY_SMTPRELAYER_PASSWORD           _T("smtprelayerpassword")
#define PROPERTY_SMTPRELAYER_USEAUTH            _T("usesmtprelayerauthentication")
#define PROPERTY_SMTPRELAYER_PORT               _T("smtprelayerport")
#define PROPERTY_USESCRIPTSERVER                _T("usescriptserver")
#define PROPERTY_SCRIPTLANGUAGE                 _T("scriptlanguage")
#define PROPERTY_MAXMESSAGESIZE                 _T("maxmessagesize")
#define PROPERTY_USECACHE                       _T("usecache")
#define PROPERTY_DOMAINCACHETTL                 _T("domaincachettl")
#define PROPERTY_ACCOUNTCACHETTL                _T("accountcachettl")
#define PROPERTY_ALIASCACHETTL                  _T("aliascachettl")
#define PROPERTY_DISTRIBUTIONLISTCACHETTL       _T("distributionlistcachettl")
#define PROPERTY_GROUPCACHETTL                  _T("groupcachettl")
#define PROPERTY_AWSTATSENABLED                 _T("awstatsenabled")
#define PROPERTY_RULELOOPLIMIT                  _T("rulelooplimit")

#define PROPERTY_USECUSTOMSCANNER               _T("usecustomvirusscanner")
#define PROPERTY_CUSTOMSCANNEREXECUTABLE        _T("customvirusscannerexecutable")
#define PROPERTY_CUSTOMSCANNERRETURNVALUE       _T("customviursscannerreturnvalue")

#define PROPERTY_USESPF                         _T("usespf")
#define PROPERTY_USESPFSCORE                    _T("usespfscore")
#define PROPERTY_USEMXCHECK                     _T("usemxchecks")
#define PROPERTY_USEMXCHECKSCORE                _T("usemxchecksscore")
#define PROPERTY_AS_CHECKHOSTINHELO             _T("ascheckhostinhelo")
#define PROPERTY_AS_CHECKHOSTINHELOSCORE        _T("ascheckhostinheloscore")

#define PROPERTY_BACKUPOPTIONS                  _T("backupoptions")
#define PROPERTY_BACKUPDESTINATION              _T("backupdestination")

#define PROPERTY_DEFAULTDOMAIN                  _T("defaultdomain")
#define PROPERTY_AV_MAXMSGSIZE                  _T("avmaxmsgsize")
#define PROPERTY_SMTPDELIVERYBINDTOIP           _T("smtpdeliverybindtoip")
#define PROPERTY_ENABLEIMAPQUOTA                _T("enableimapquota")
#define PROPERTY_ENABLEIMAPIDLE                 _T("enableimapidle")
#define PROPERTY_ENABLEIMAPSORT                 _T("enableimapsort")
#define PROPERTY_ENABLEIMAPACL                  _T("enableimapacl")
#define PROPERTY_MAXIMAPCONNECTIONS             _T("maximapconnections")
#define PROPERTY_WORKERTHREADPRIORITY           _T("workerthreadpriority")

#define PROPERTY_TCPIPTHREADS                   _T("tcpipthreads")
#define PROPERTY_ALLOWINCORRECTLINEENDINGS      _T("smtpallowincorrectlineendings")
#define PROPERTY_MAXSMTPRECIPIENTSINBATCH       _T("maxsmtprecipientsinbatch")
#define PROPERTY_IMAPPUBLICFOLDERNAME           _T("imappublicfoldername")

#define PROPERTY_USEGREYLISTING                 _T("usegreylisting")
#define PROPERTY_GL_INITIALDELAY                _T("greylistinginitialdelay")
#define PROPERTY_GL_INITIALDELETE               _T("greylistinginitialdelete")
#define PROPERTY_GL_FINALDELETE                 _T("greylistingfinaldelete")

#define PROPERTY_AS_ADDHEADERSPAM               _T("antispamaddheaderspam")
#define PROPERTY_AS_ADDHEADERREASON             _T("antispamaddheaderreason")
#define PROPERTY_AS_PREPENDSUBJECT              _T("antispamprependsubject")
#define PROPERTY_AS_PREPENDSUBJECTTEXT          _T("antispamprependsubjecttext")
#define PROPERTY_AS_MAX_SIZE                    _T("antispammaxsize")

#define PROPERTY_ENABLEATTACHMENTBLOCKING       _T("enableattachmentblocking")
#define PROPERTY_SMTPDISCONNECTINVALIDCLIENTS   _T("disconnectinvalidclients")
#define PROPERTY_SMTPMAXINCORRECTCOMMANDS       _T("maximumincorrectcommands")

#define PROPERTY_SMTPRELAYERCONNECTIONSECURITY  _T("smtprelayerconnectionsecurity")

#define PROPERTY_ADDDELIVEREDTOHEADER           _T("adddeliveredtoheader")

#define PROPERTY_SPAMDELETETHRESHOLD            _T("spamdeletethreshold")
#define PROPERTY_SPAMMARKTHRESHOLD              _T("spammarkthreshold")

#define PROPERTY_SPAMASSASSIN_ENABLED           _T("spamassassinenabled")
#define PROPERTY_SPAMASSASSIN_SCORE             _T("spamassassinscore")
#define PROPERTY_SPAMASSASSIN_MERGE_SCORE       _T("spamassassinmergescore")
#define PROPERTY_SPAMASSASSIN_HOST              _T("spamassassinhost")
#define PROPERTY_SPAMASSASSIN_PORT              _T("spamassassinport")

#define PROPERTY_AS_DKIM_VERIFICATION_ENABLE          _T("ASDKIMVerificationEnabled")
#define PROPERTY_AS_DKIM_VERIFICATION_FAILURE_SCORE   _T("ASDKIMVerificationFailureScore")

#define PROPERTY_AUTOBAN_ENABLED                _T("AutoBanOnLogonFailureEnabled")
#define PROPERTY_MAX_INVALID_LOGON_ATTEMPTS     _T("MaxInvalidLogonAttempts")
#define PROPERTY_LOGON_ATTEMPTS_WITHIN          _T("LogonAttemptsWithinMinutes")
#define PROPERTY_AUTOBAN_MINUTES                _T("AutoBanMinutes")

#define PROPERTY_IMAP_HIERARCHY_DELIMITER       _T("IMAPHierarchyDelimiter")
#define PROPERTY_MESSAGE_INDEXING               _T("MessageIndexing")
#define PROPERTY_MAX_NUMBER_OF_ASYNC_TASKS      _T("MaxNumberOfAsynchronousTasks")

#define PROPERTY_BYPASS_GREYLISTING_ON_SPFSUCCESS  _T("BypassGreylistingOnSPFSuccess")
#define PROPERTY_BYPASS_GREYLISTING_ON_MAILFROMMX  _T("BypassGreylistingOnMailFromMX")

#define PROPERTY_MAX_NUMBER_OF_MXHOSTS          _T("MaxNumberOfMXHosts")
#define PROPERTY_SMTPCONNECTIONSECURITY         _T("SmtpDeliveryConnectionSecurity")
#define PROPERTY_VERIFYREMOTESSLCERTIFICATE     _T("VerifyRemoteSslCertificate")

#define PROPERTY_SSLCIPHERLIST _T("SslCipherList")

#define PROPERTY_SSLVERSIONS _T("SslVersions")

#define PROPERTY_TLSOPTIONS _T("TlsOptions")

#define PROPERTY_CLAMAV_ENABLED              _T("ClamAVEnabled")
#define PROPERTY_CLAMAV_HOST                 _T("ClamAVHost")
#define PROPERTY_CLAMAV_PORT                 _T("ClamAVPort")


#define REQUIRED_DB_VERSION            5601
