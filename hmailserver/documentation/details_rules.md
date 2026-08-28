---
title: "Rule details"
slug: details_rules
parent: reference_rule
index: 0
is_book: false
---

## Rule details

This document contains details about rule processing.

### Forwarding of forwarded messages

In general, messages forwarded by rules are also affected by rules. For example, if you set up a global rule which forwards all email to someone@example.com, a new message is created with someone@example.com as recipient address. When this new email is sent, this email is also affected by the rule and another copy of the message is created. This results in a loop which generates new messages.

hMailServer has built-in functionality to stop this rule (see *Rules loop count* below), but it is still a problem since the recipient receive multiple copies of the same email message. To resolve the problem, rules must be configured to prevent forwarded messages from being affected by the rule. In the example above, a rule criteria with the configuration "*If RecipientList Not Contains someone@example.com*" must be added, to prevent the rule from affecting messages where someone@example.com is already in the recipient list.

### Rules loop count

hMailServer 4.2 and later can limit the number of times a message is processed in a loop. The loop limit is found in the advanced SMTP settings. If two users set up rules that make an email to bounce back and forth between them, hMailServer will only handle the email the number of times defined by rule loop count.

### Global rules vs. Account rules

During message delivery, global rules are executed before any account rules. This means that if you set up a global rule to delete all messages for a specific account, the messages won't reach this account and they won't be effected by rules in this account.

In the same way, account level rules can override behaviour of global rules. If you set up a global rule to move messages a specific IMAP folder, and then set up an account rule to move the messages to a different IMAP folder, the account rule will have precedence.

### Order of rules and actions

Rules and actions are executed in the same way as they appear in hMailServer Administrator. Starting with hMailServer 4.4, you can change the order of rules and actions by selecting the rule or action and then click *Move down* or *Move up*.
