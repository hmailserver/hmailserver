---
id: 183
title: "Rule"
slug: reference_rule
parent: book_configuration
index: 0
is_book: false
updated: 2013-05-07
---

## Overview

Rules enable you to define actions to take based on the contents of an email. For example, you can use rules to delete mail based on a specific subject-line, or to forward email larger than a specific size.

Rules can be defined at two different levels: Global and Account. Global rules are applied to all messages delivered to the server, regardless of which recipient they are to be delivered to. Account rules apply only to email to a specific account.   

  

Every rule has a set of criteria and actions. When you create a rule, you add criteria that specify which email the rule should be applied to. For example, you may add a criterion that limits the rule to email containing a specific Message-ID-header. After you have added criteria, you add actions. Actions define what hMailServer should do if a message matches the criteria. For example, the action may be to forward the email, delete it or move it to a specific folder.

Rules are applied during the email delivery phase. This means that if you do content modification of an email message in a rule, or move the message to specific IMAP folders, this will only effect how the recipient of the email message see it. For example, if you have set up a global rule to move messages to the IMAP folder Spam, and User 1 sends a message to User 2, only User 2 will see the email message in his Spam mailbox. User 1 will not see the message in his Spam mailbox, since he's not the recipient of the message. The reason for this is that if User 1 sends an email message to User 2, the message should be delivered to User 2 - not to User 1.

### Match testing

In the rule criteria dialog, you can test whether specific values will match the criteria. To run a test, simply enter the value you want to test into the Test value field. If the value matches the criteria, you will see the next "Match" next to the text box. If not, you will see the text "No match". This makes it easier to create more advanced criterias, for example using regular expressions.

### Examples

Here are [some examples](?page=details_rules_examples) on how to implement rules.

### Different search types

hMailServer supports a number of different search types. They are: Is, Contains, Less than, Larger than and Regular Expression.

- *Is*: Used to specify an exact match.
- *Contains*: Used to specify a partial match. For example, you might want to apply the rule to email where the Body *Contains* a certain word.
- *Less than*: Used for numeric matches. For example, you may want to apply the rule to email where the message size is less than 1 MB.
- *Larger than*: Used for numeric matches. Opposite of Less than
- *Regular expression*: Using regular expression matching, you can specify more complex matchings. For example, you may apply the rule to all messages where the subject line begins with a letter and the rest of it is numeric. For more information about regular expressions, visit [http://www.regular-expressions.info/](http://www.regular-expressions.info/). The used regular expression should match the entire value it's being matched against. It is not possible to use partial matching. hMailServer rely on [Boost](http://www.boost.org)/Regex to do regular expression parsing and use the Perl syntax. More information about available options can be found on the [Boost site](http://www.boost.org/doc/libs/1_38_0/libs/regex/doc/html/boost_regex/syntax/perl_syntax.html).

### Actions

- Delete email - Select this rule to delete the email message. The message will be deleted after rule processing has finished.
- Forward email - Select this if you want to forward email to another recipient. It's possible to specify both local and external recipients.
- Reply - This option can be used if you want to automatically reply to messages.
- Run function - Use this option if you want to run a hMailServer script whenever a message matches the rule.
- Set header value - Using the "Set header value" option, you can add MIME-headers to email message.
- Moving to IMAP folder - It is possible, when moving messages to folder using a rule, to move messages to folders  as well as to sub folders. Use this syntax: *Folder.Subfolder1.Subfolder2*. If you have changed the default Hierarchy delimiter in hMailServer, you must use the new hierarchy delimiter rather than . (dot).
- Stop rule processing - Select this option if you want to cancel the remaining rule processing. Any action or rule specified after this will not be executed.

### Details

If you want to know details on how rule processing work, please see the [details document](?page=details_rules).

### Troubleshooting

If you are experiencing a problem with rules, please read the [troubleshooting tips](?page=ts_rules).

## Settings

### Name

<div class="indented">The name of the rule. This can be anything you like.</div>

<div class="indented">
<h3>Criteria</h3>
<div class="indented">The criteria for this rule. Messages matching the criteria will be affected by the rule actions.</div>
<div class="indented">
<ul>
    <li>Use AND - All of the criterias must match for the message to be effected by the rule.</li>
    <li>Use OR - If one criteria matches, the message will be effected by the rule.</li>
</ul>
<h4>Predefined field</h4>
<ul>
    <li>From - The From MIME-header in the email message.</li>
    <li>To - The To MIME-header in the email message. This should not be confused with the <em>Recipient list</em> setting. See notes (3) below for details.</li>
    <li>CC - The CC MIME-header in the email message.</li>
    <li>Recipient list - A list of all actual message recipients. This list is taken from the SMTP envelope, not from the MIME headers. See notes (3) below for details.</li>
    <li>Subject - The Subject MIME-header of the email message.</li>
    <li>Body - The Body of the email message. This includes both the plain text body and the HTML body.</li>
    <li>Message size - The size of the message</li>
    <li>Delivery attempts - 1 the first time the message is delivered, 2 ond the second attempt and so on.</li>
</ul>
<h4>Custom header field</h4>
<p>If the header you want to filter on does not exist in the list of predefined fields, you can enter the name of the MIME-header in this field.</p>
<h4>Search types</h4>
<ul>
    <li>Equals - The value / string must match exactly</li>
    <li>Not equals - The value / string must not match.</li>
    <li>Contains - Partial match</li>
    <li>Not contains - The value must not exist in the Predefined field / custom header field.</li>
    <li>Less than - Can only be used for values</li>
    <li>Greater than - Can only be used for values.</li>
    <li>Regular expression - Use a regular expression to match the value.</li>
    <li>Wildcard - Use a search string with wild cards to search for a value. <br>
    When wildcars are used, * (star) matches any sequence of characters, ? matches any single character.</li>
</ul>
<h4>Value</h4>
<p>Enter the value to search for, or a regular expression to use.</p>
<h4>Test</h4>
<p>Using the Test section in the Criteria dialog, you can test whether different values will match your criteria. If the value you have typed in matches the criteria, you will see the text Match next to the entered value. If not, you will see the text No match.</p>
<h3>Actions</h3>
<div class="indented">The action hMailServer should take when a message matches the rule criteria.</div>
<ul>
    <li>Delete email - The message will be deleted and not delivered to the recipient</li>
    <li>Forward email - Forward the message to the specified address. The message will still be delivered to the original recipient</li>
    <li>Move to IMAP folder - Move the message to a specific IMAP folder. This only applies when the delivery is local. If the folder does not already exist, it is created.</li>
    <li>Reply - Reply to the sender with the specified message</li>
    <li>Run function - Runs a function in the hMailServer script file. <br>
    This function should take a hMailServer.Message object as parameter, for example <span class="Code">Sub OnSomething(oMessage)</span>.</li>
    <li>Set  header value - This action lets you create or modify an existing header value.</li>
    <li>Stop rule processing - This action will cancel the remaining rules.</li>
    <li>Send using route - Normally hMailServer uses recipient addresses to determine whether or not a route should be used. Using this rule action, you can override the default behaviour. As an example, you can set up a rule which instructs hMailServer to send all email being sent from a specific domain on to another server.</li>
</ul>
<div class="indented">
<h3>Notes</h3>
</div>
<div class="indented">
<ol>
    <li>It's possible to specify the macros %YEAR%, %MONTH% and %DAY% in the folder name parameter if Move to IMAP folder action has been selected.</li>
    <li>When searching for values in header fields, an empty string is treated in the same way as an nonexistent header. So if you set up a rule which will delete messages if the subject line is empty, it will delete lines where the Subject header does not exist as well.</li>
    <li>An email message normally have two lists of recipients. Knowledge about this is of importance when setting up rules to filter on the To header and Recipient list. <br>
    <br>
    The first list is the MIME recipients list. This list of recipient is the list you normally see in your email client, in the To and CC headers. The second list is the list in the MIME envelope - the MIME envelope contains the addresses where the email will actually be delivered. It's normally not possible to view this list in an email client. This means that an email can be sent to one address, but have other recipients in the To and CC headers. A common example on this is if you send an email and put someone in the BCC field. The address you enter in the BCC field will be added to the address list in the SMTP envelope, but will not be added to any MIME headers.<br>
    <br>
    When hMailServer forwards an email, the Recipient list (in the SMTP envelope) is updated. The recipient list in the MIME headers is not. This means that the recipient who receives the forwarded email message will see the original recipients in his email client.<br>
    <br>
    Also, when a message is placed into an account, the recipient list is cleared to save disk space. So when account-level rules are executed, the recipient list will be empty and cannot be checked.</li>
</ol>
</div>
</div>
</div>
