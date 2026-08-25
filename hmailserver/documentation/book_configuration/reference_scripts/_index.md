---
title: "Scripts"
slug: reference_scripts
parent: book_configuration
index: 0
is_book: false
---

## Overview

hMailServer enable you to write your own scripts to extend the server's functionality. Support for Microsoft VBScript and Microsoft JScript currently exists in the server. You will find at hMailServer.com useful sample scripts written in VBScript. For general script syntax, you should consult the [Microsoft MSDN](http://msdn.microsoft.com/en-us/library/d1wf56tt%28VS.85%29.aspx) library.   

  

All hMailServer scripts should be placed in a file called EventHandlers.vbs. The file is found in the hMailServer Events directory, normally C:\Program Files\hMailServer\Events.   

  

hMailServer offers the following pre-defined events:

<table width="800" cellspacing="10">
    <tbody>
        <tr>
            <td><strong>Event</strong></td>
            <td><strong>Purpose</strong></td>
            <td><strong>Implemented in</strong></td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_onbackupcompleted">OnBackupCompleted</a></td>
            <td valign="top">Executed when a backup has completed.</td>
            <td valign="top">4.2</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_onbackupcompleted">OnBackupFailed</a></td>
            <td valign="top">Executed when a backup has failed.</td>
            <td valign="top">4.2</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_onclientconnect">OnClientConnect</a></td>
            <td valign="top">Executed when a client is connected.</td>
            <td valign="top">4.0</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_onhelo">OnHELO</a></td>
            <td valign="top">Executed when a client sends the SMTP HELO or EHLO command.</td>
            <td valign="top">5.7</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_onclientlogon">OnClientLogon</a></td>
            <td valign="top">Executed when a client has logged on using SMTP, POP3 or IMAP.</td>
            <td valign="top">5.7</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_onclientvalidatepassword">OnClientValidatePassword</a></td>
            <td valign="top">Executed when a password is about to be validated. Lets a script accept or reject the password.</td>
            <td valign="top">5.7</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_onacceptmessage">OnAcceptMessage</a></td>
            <td valign="top">Executed when an e-mail has been delivered to the server          using the SMTP protocol.</td>
            <td valign="top">4.0</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_ondeliverystart">OnDeliveryStart</a></td>
            <td valign="top">Executed directly when the delivery of an email has started, before any rules are executed.</td>
            <td valign="top">4.4</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_ondelivermessage">OnDeliverMessage</a></td>
            <td valign="top">Executed when an e-mail is beeing delivered. Executed after global rules are executed, but before account-level rules.</td>
            <td valign="top">4.0</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_ondeliveryfailed">OnDeliveryFailed</a></td>
            <td valign="top">Executed if delivery of a a message has failed</td>
            <td valign="top">5.0</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_onexternalaccountdownload%09">OnExternalAccountDownload</a></td>
            <td valign="top">Executed when a message has been downloaded from a remote POP3 account.</td>
            <td valign="top">5.3</td>
        </tr>
        <tr>
            <td valign="top">
            <p><a href="?page=scripting_onerror">OnError</a></p>
            </td>
            <td valign="top">Executed if a error occurs in hMailServer.</td>
            <td valign="top">5.0</td>
        </tr>
        <tr>
            <td valign="top"><a href="https://www.hmailserver.com/documentation/latest/?page=scripting_onsmtpdata">OnSMTPData</a></td>
            <td valign="top">Executed when SMTP Data is received</td>
            <td valign="top">5.4</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_onrecipientunknown">OnRecipientUnknown</a></td>
            <td valign="top">Executed when a client specifies a recipient in a local domain which does not exist.</td>
            <td valign="top">5.7</td>
        </tr>
        <tr>
            <td valign="top"><a href="?page=scripting_ontoomanyinvalidcommands">OnTooManyInvalidCommands</a></td>
            <td valign="top">Executed when an SMTP client has sent too many invalid commands and is about to be disconnected.</td>
            <td valign="top">5.7</td>
        </tr>
    </tbody>
</table>

## Order of Execution

The events are executed in the following order:

1. `OnClientConnect` (oClient)
2. `OnHELO` (oClient)
3. *Spam tests: DNS blacklists, HELO host, MX records, SPF*
4. `OnClientValidatePassword` (oAccount, sPassword) - if the client authenticates
5. `OnClientLogon` (oClient) - if the client authenticates
6. `OnRecipientUnknown` (oClient, oMessage) - if applicable
7. `OnSMTPData` (oClient, oMessage)
8. *Spam tests: SURBL, DKIM, greylisting, integrated SpamAssassin*
9. `OnAcceptMessage` (oClient, oMessage)
10. `OnDeliveryStart` (oMessage)
11. *Integrated virus check, global rules*
12. `OnDeliverMessage` (oMessage)
13. `OnDeliveryFailed` (oMessage, sRecipient, sErrorMessage) - if applicable
14. *Message delivered to recipient(s), account-level rules*

## Settings

Follow these steps to enable scripting:

<ul>
    <li>Start hMailServer Administrator</li>
    <li>Navigate to <em>Settings-&gt;Advanced-&gt;Scripts</em></li>
    <li>Select <span style="font-style: italic;">Enabled</span></li>
    <li>Click on <em>Save</em> to save your changes</li>
    Whenever you modify the script file you have to click on <em>Reload script</em> for hMailServer to refresh, recording the changes. hMailServer keeps a copy of the entire script in memory, which improves performance.</ul>

## Objects

See [COM API](https://www.hmailserver.com/documentation/latest/?page=com_objects) for object references

<p><strong><em>For Developers</em></strong><em> the prototypes are in file: ScriptServer.h<br>
    </em><a href="https://github.com/hmailserver/hmailserver/blob/master/hmailserver/source/Server/Common/Scripting/ScriptServer.h" class="postlink"><em>https://github.com/hmailserver/hmailser ... ptServer.h</em></a></p>
