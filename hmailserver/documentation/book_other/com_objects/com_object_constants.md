---
title: "Constants"
slug: com_object_constants
parent: com_objects
index: 0
is_book: false
---

### Description

The constants available in the hMailServer COM API.

### Constant Group and values

<div class="api_method_name">eACLPermission</div>

<div class="api_description"> Const ePermissionLookup = 1</div>

<div class="api_description"> Const ePermissionRead = 2</div>

<div class="api_description"> Const ePermissionWriteSeen = 4</div>

<div class="api_description"> Const ePermissionWriteOthers = 8</div>

<div class="api_description"> Const ePermissionInsert = 16</div>

<div class="api_description"> Const ePermissionPost = 32</div>

<div class="api_description"> Const ePermissionCreate = 64</div>

<div class="api_description"> Const ePermissionDeleteMailbox = 128</div>

<div class="api_description"> Const ePermissionWriteDeleted = 256</div>

<div class="api_description"> Const ePermissionExpunge = 512</div>

<div class="api_description"> Const ePermissionAdminister = 1024</div>

<div class="api_description"> </div>

<div class="api_method_name">eACLPermissionType</div>

<div class="api_description"> Const ePermissionTypeUser = 0</div>

<div class="api_description"> Const ePermissionTypeGroup = 1</div>

<div class="api_description"> Const ePermissionTypeAnyone = 2</div>

<div class="api_description"> </div>

<div class="api_method_name">eAdminLevel</div>

<div class="api_description"> Const hAdminLevelNormal = 0</div>

<div class="api_description"> Const hAdminLevelDomainAdmin = 1</div>

<div class="api_description"> Const hAdminLevelServerAdmin = 2</div>

<div class="api_description"> </div>

<div class="api_method_name">eAntivirusAction</div>

<div class="api_description"> Const hDeleteEmail = 0</div>

<div class="api_description"> Const hDeleteAttachments = 1</div>

<div class="api_description"> </div>

<div class="api_method_name">eConnectionSecurity</div>

<div class="api_description"> Const eCSNone = 0</div>

<div class="api_description"> Const eCSTLS = 1</div>

<div class="api_description"> Const eCSSTARTTLSOptional = 2</div>

<div class="api_description"> Const eCSSTARTTLSRequired = 3</div>

<div class="api_description"> </div>

<div class="api_method_name">eDBtype</div>

<div class="api_description"> Const hDBTypeMySQL = 1</div>

<div class="api_description"> Const hDBTypeMSSQL = 2</div>

<div class="api_description"> Const hDBTypePostgreSQL = 3</div>

<div class="api_description"> Const hDBTypeMSSQLCE = 4</div>

<div class="api_description"> </div>

<div class="api_method_name">eDistributionListMode</div>

<div class="api_description"> Const eLMPublic = 0</div>

<div class="api_description"> Const eLMMembership = 1</div>

<div class="api_description"> Const eLMAnnouncement = 2</div>

<div class="api_description"> Const eLMDomainMembers = 3</div>

<div class="api_description"> Const eLMServerMembers = 4</div>

<div class="api_description"> </div>

<div class="api_method_name">eDKIMAlgorithm</div>

<div class="api_description"> Const eSHA1 = 1</div>

<div class="api_description"> Const eSHA256 = 2</div>

<div class="api_description"> </div>

<div class="api_method_name">eDKIMCanonicalizationMethod</div>

<div class="api_description"> Const eCanonicalizationSimple = 1</div>

<div class="api_description"> Const eCanonicalizationRelaxed = 2</div>

<div class="api_description"> </div>

<div class="api_method_name">eDKIMResult</div>

<div class="api_description"> Const eDKNeutral = 0</div>

<div class="api_description"> Const eDKPass = 1</div>

<div class="api_description"> Const eDKTempFail = 2</div>

<div class="api_description"> Const eDKPermFail = 3</div>

<div class="api_description"> </div>

<div class="api_method_name">eDomainSignatureMethod</div>

<div class="api_description"> Const eSMUnknown = 0</div>

<div class="api_description"> Const eSMSetIfNotSpecifiedInAccount = 1</div>

<div class="api_description"> Const eSMOverwriteAccountSignature = 2</div>

<div class="api_description"> Const eSMAppendToAccountSignature = 3</div>

<div class="api_description"> </div>

<div class="api_method_name">eLogDevice</div>

<div class="api_description"> Const hLogDeviceSQL = 1</div>

<div class="api_description"> Const hLogDeviceFile = 2</div>

<div class="api_description"> </div>

<div class="api_method_name">eLogOutputFormat</div>

<div class="api_description"> Const hLogFormatDefault = 1</div>

<div class="api_description"> Const hLogFormatCSA = 2</div>

<div class="api_description"> </div>

<div class="api_method_name">eMaintenanceOperation</div>

<div class="api_description"> Const eUpdateIMAPFolderUID = 1</div>

<div class="api_description"> </div>

<div class="api_method_name">eMessageFlag</div>

<div class="api_description"> Const eMFSeen = 1</div>

<div class="api_description"> Const eMFDeleted = 2</div>

<div class="api_description"> Const eMFFlagged = 4</div>

<div class="api_description"> Const eMFAnswered = 8</div>

<div class="api_description"> Const eMFDraft = 16</div>

<div class="api_description"> Const eMFRecent = 32</div>

<div class="api_description"> Const eMFVirusScan = 64</div>

<div class="api_description"> Const eMFSpam = 128</div>

<div class="api_description"> </div>

<div class="api_method_name">eRuleActionType</div>

<div class="api_description"> Const eRAUnknown = 0</div>

<div class="api_description"> Const eRADeleteEmail = 1</div>

<div class="api_description"> Const eRAForwardEmail = 2</div>

<div class="api_description"> Const eRAReply = 3</div>

<div class="api_description"> Const eRAMoveToImapFolder = 4</div>

<div class="api_description"> Const eRARunScriptFunction = 5</div>

<div class="api_description"> Const eRAStopRuleProcessing = 6</div>

<div class="api_description"> Const eRASetHeaderValue = 7</div>

<div class="api_description"> Const eRASendUsingRoute = 8</div>

<div class="api_description"> Const eRACreateCopy = 9</div>

<div class="api_description"> Const eRABindToAddress = 10</div>

<div class="api_description"> </div>

<div class="api_method_name">eRuleMatchType</div>

<div class="api_description"> Const eMTUnknown = 0</div>

<div class="api_description"> Const eMTEquals = 1</div>

<div class="api_description"> Const eMTContains = 2</div>

<div class="api_description"> Const eMTLessThan = 3</div>

<div class="api_description"> Const eMTGreaterThan = 4</div>

<div class="api_description"> Const eMTRegExMatch = 5</div>

<div class="api_description"> Const eMTNotContains = 6</div>

<div class="api_description"> Const eMTNotEquals = 7</div>

<div class="api_description"> Const eMTWildcard = 8</div>

<div class="api_description"> </div>

<div class="api_method_name">eRulePredefinedField</div>

<div class="api_description"> Const eFTUnknown = 0</div>

<div class="api_description"> Const eFTFrom = 1</div>

<div class="api_description"> Const eFTTo = 2</div>

<div class="api_description"> Const eFTCC = 3</div>

<div class="api_description"> Const eFTSubject = 4</div>

<div class="api_description"> Const eFTBody = 5</div>

<div class="api_description"> Const eFTMessageSize = 6</div>

<div class="api_description"> Const eFTRecipientList = 7</div>

<div class="api_description"> Const eFTDeliveryAttempts = 8</div>

<div class="api_description"> </div>

<div class="api_method_name">eServerState</div>

<div class="api_description"> Const hStateStopped = 1</div>

<div class="api_description"> Const hStateStarting = 2</div>

<div class="api_description"> Const hStateRunning = 3</div>

<div class="api_description"> Const hStateStopping = 4</div>

<div class="api_description"> </div>

<div class="api_method_name">eSessionType</div>

<div class="api_description"> Const eSTUnknown = 0</div>

<div class="api_description"> Const eSTSMTP = 1</div>

<div class="api_description"> Const eSTSMTPClient = 2</div>

<div class="api_description"> Const eSTPOP3 = 3</div>

<div class="api_description"> Const eSTPOP3Client = 4</div>

<div class="api_description"> Const eSTIMAP = 5</div>

<div class="api_description"> </div>

<div class="api_method_name">eSpecialUse</div>

<div class="api_description"> Const eSUNone = 0</div>

<div class="api_description"> Const eSUArchive = 1</div>

<div class="api_description"> Const eSUDrafts = 2</div>

<div class="api_description"> Const eSUJunk = 4</div>

<div class="api_description"> Const eSUSent = 8</div>

<div class="api_description"> Const eSUTrash = 16</div>

<div class="api_description"> </div>
