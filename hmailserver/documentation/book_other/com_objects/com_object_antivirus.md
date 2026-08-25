---
title: "AntiVirus object"
slug: com_object_antivirus
parent: com_objects
index: 0
is_book: false
---

### Description

The anti virus object settings related to virus scanning.

### Methods

<div class="api_method_name">TestClamAVScanner(string ClamAVHostName, long ClamAVPort, string ResultText)</div>

<div class="api_description">Tests to connect to ClamAV</div>

<div class="api_method_name">TestClamWinScanner(string clamWinExecutable, string clamWinDatabase, string ResultText)</div>

<div class="api_description">Tests to use the ClamWin</div>

<div class="api_method_name">TestCustomerScanner(string customExecutable, long virusReturnCode, string ResultText)</div>

<div class="api_description">Tests to use the custom virus scanner.</div>

### Properties

<div class="api_method_name">eAntivirusAction Action</div>

<div class="api_description">Action.<br></div>

<div class="api_method_name"><a href="?page=com_object_blockedattachments">BlockedAttachments</a> BlockedAttachments</div>

<div class="api_description">Blocked attachments<br> <i>(read-only)</i></div>

<div class="api_method_name">bool ClamAVEnabled</div>

<div class="api_description">ClamAV enabled.<br></div>

<div class="api_method_name">string ClamAVHost</div>

<div class="api_description">ClamAV host.<br></div>

<div class="api_method_name">long ClamAVPort</div>

<div class="api_description">ClamAV port.<br></div>

<div class="api_method_name">string ClamWinDBFolder</div>

<div class="api_description">ClamWIn database folder.<br></div>

<div class="api_method_name">bool ClamWinEnabled</div>

<div class="api_description">ClamWin enabled.<br></div>

<div class="api_method_name">string ClamWinExecutable</div>

<div class="api_description">ClamWin Executable path.<br></div>

<div class="api_method_name">bool CustomScannerEnabled</div>

<div class="api_description">Custom scanner enabled.<br></div>

<div class="api_method_name">string CustomScannerExecutable</div>

<div class="api_description">Custom executable path.<br></div>

<div class="api_method_name">long CustomScannerReturnValue</div>

<div class="api_description">Custom scanner return value.<br></div>

<div class="api_method_name">bool EnableAttachmentBlocking</div>

<div class="api_description">Enable attachment blocking.<br></div>

<div class="api_method_name">long MaximumMessageSize</div>

<div class="api_description">Maximum message size when running anti virus (KB).<br></div>

<div class="api_method_name">bool NotifyReceiver</div>

<div class="api_description">Notify receiver that email has been deleted.<br></div>

<div class="api_method_name">bool NotifySender</div>

<div class="api_description">Notify sender that email has been deleted.<br></div>
