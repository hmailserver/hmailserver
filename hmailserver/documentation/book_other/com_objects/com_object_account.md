---
title: "Account object"
slug: com_object_account
parent: com_objects
index: 0
is_book: false
---

### Description

The account object represents a single hMailServer account.

### Methods

<div class="api_method_name">Delete()</div>

<div class="api_description">Deletes the account from the database.</div>

<div class="api_method_name">DeleteMessages()</div>

<div class="api_description">Deletes all IMAP folders and messages that exists in the account.</div>

<div class="api_method_name">Save()</div>

<div class="api_description">Saves the account in the database.</div>

<div class="api_method_name">UnlockMailbox()</div>

<div class="api_description">Unlocks the POP3 mailbox.</div>

<div class="api_method_name">ValidatePassword(string Password)</div>

<div class="api_description">Validates the password. Returns true if the password is correct.</div>

### Properties

<div class="api_method_name">bool Active</div>

<div class="api_description">Active<br></div>

<div class="api_method_name">string ADDomain</div>

<div class="api_description">Name of the active directory domain.<br></div>

<div class="api_method_name">string Address</div>

<div class="api_description">The e-mail address of the account.<br></div>

<div class="api_method_name">eAdminLevel AdminLevel</div>

<div class="api_description">Administration level.<br></div>

<div class="api_method_name">string ADUsername</div>

<div class="api_description">Name of the active directory user account.<br></div>

<div class="api_method_name">long DomainID</div>

<div class="api_description">The database identifier of the domain in which the account belong.<br></div>

<div class="api_method_name"><a href="?page=com_object_fetchaccounts">FetchAccounts</a> FetchAccounts</div>

<div class="api_description">External fetch accounts connected to this account.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool ForwardAbortSpamFlagged</div>

<div class="api_description">Forwarding is set to be aborted on messages marked as spam.<br></div>

<div class="api_method_name">string ForwardAddress</div>

<div class="api_description">Address messages to this account should be forwarded to.<br></div>

<div class="api_method_name">bool ForwardEnabled</div>

<div class="api_description">Forwarding for this account is enabled.<br></div>

<div class="api_method_name">bool ForwardKeepOriginal</div>

<div class="api_description">Keep original message after forwarding has been applied.<br></div>

<div class="api_method_name">long ID</div>

<div class="api_description">The database identifier for the account.<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_imapfolders">IMAPFolders</a> IMAPFolders</div>

<div class="api_description">The IMAP folders belonging to this account.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool IsAD</div>

<div class="api_description">The account is connected to Active Directory.<br></div>

<div class="api_method_name">VARIANT LastLogonTime</div>

<div class="api_description">The timestamp for the last usage of this account.<br> <i>(read-only)</i></div>

<div class="api_method_name">long MaxSize</div>

<div class="api_description">Maximum size in of the account (MB).<br></div>

<div class="api_method_name"><a href="?page=com_object_messages">Messages</a> Messages</div>

<div class="api_description">Messages in the accounts mailbox.<br> <i>(read-only)</i></div>

<div class="api_method_name">string Password</div>

<div class="api_description">The password of the account.<br></div>

<div class="api_method_name">string PersonFirstName</div>

<div class="api_description">First name of the account holder.<br></div>

<div class="api_method_name">string PersonLastName</div>

<div class="api_description">Last name of the account holder.<br></div>

<div class="api_method_name">long QuotaUsed</div>

<div class="api_description">Percentage of quota used (0-100).<br> <i>(read-only)</i></div>

<div class="api_method_name"><a href="?page=com_object_rules">Rules</a> Rules</div>

<div class="api_description">The rules specified for this account.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool SignatureEnabled</div>

<div class="api_description">Enable the signature<br></div>

<div class="api_method_name">string SignatureHTML</div>

<div class="api_description">The HTML version of the signature<br></div>

<div class="api_method_name">string SignaturePlainText</div>

<div class="api_description">The plain text version of the signature<br></div>

<div class="api_method_name">float Size</div>

<div class="api_description">The size of the account.<br> <i>(read-only)</i></div>

<div class="api_method_name">string VacationMessage</div>

<div class="api_description">The vacation message<br></div>

<div class="api_method_name">bool VacationMessageAbortSpamFlagged</div>

<div class="api_description">Vacation message is set to be aborted on messages marked as spam.<br></div>

<div class="api_method_name">bool VacationMessageExpires</div>

<div class="api_description">Vacation message is set to expire<br></div>

<div class="api_method_name">string VacationMessageExpiresDate</div>

<div class="api_description">The vacation message expires this date<br></div>

<div class="api_method_name">bool VacationMessageIsOn</div>

<div class="api_description">Vacation message is turned on.<br></div>

<div class="api_method_name">string VacationSubject</div>

<div class="api_description">The vacation message subject<br></div>
