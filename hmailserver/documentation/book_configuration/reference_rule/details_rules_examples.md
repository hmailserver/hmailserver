---
id: 106
title: "Rule examples"
slug: details_rules_examples
parent: reference_rule
index: 0
is_book: false
updated: 2009-10-16
---

## Example 1, Delete email

Automatically delete all email messages whose subject-line contains the word *SPAM*

1. Start hMailAdmin, and select *Rules* in the tree to the left, under *Domains*
2. Click on *Add*. Enter the name `MyRule`
3. Under *Criteria*, click on *Add*.
4. Under *Pre-defined field*, select *Subject*
5. Under *Search type*, select *Contains*.
6. In the *Value* textbox, enter the word `SPAM`
7. Click on *Save* to add the criteria to the rule.
8. Under *Actions*, click on Add.
9. Select *Delete email* from the drop-down list
10. Click on *Save* to save the Action
11. Click again on *Save* to save the rule

## Example 2, forward messages

Forward all messages addressed to an external email address. The original message will still be delivered normally. Don't set up a rule that forwards email to yourself, since that would cause an endless loop.

1. Start hMailAdmin and navigate to the account settings.
2. Select the *Rules* tab.
3. Click on *Add*. Enter the name `ForwardRule`
4. Under *Criteria*, click on *Add*.
5. Under *Pre-defined field*, select *Message size*
6. Under *Search type*, select *Greater than*.
7. In the *Value* textbox, enter the value `0`. This will cause all messages larger than 0 bytes to be affected by the rule.
8. Click on *Save* to add the criteria to the rule.
9. Under *Actions*, click on Add.
10. Select *Forward email* from the drop down. Enter the external address to forward to.
11. Click on *Save* to save the Action
12. Click again on *Save* to save the entire rule

## Example 3, move messages to IMAP folder

Move all messages from a specific sender to a specific IMAP folder

1. Start hMailAdmin and navigate to the account settings.
2. Select the *Rules* tab.
3. Click on *Add*. Enter the name `MoveRule`
4. Under *Criteria*, click on *Add*.
5. Under *Pre-defined field*, select *From*
6. Under *Search type*, select *Contains*.
7. In the *Value* textbox, enter the value `someone@somedomain.com`. This will cause all messages that contain someone@somedomain.com in the From header field to be affected by the rule.
8. Click on *Save* to add the criteria to the rule.
9. Under *Actions*, click on Add.
10. Select *Move to IMAP folder* from the drop down. Enter the name of the IMAP folder to move it to.
11. Click on *Save* to save the Action
12. Click again on *Save* to save the entire rule

## Example 4, update subject of email message

The following tutorial will guide you through how to call a VBA-script depending on the contents of a message. The below example will show how you update the subject of a message depending on the size of the message.

### Setting up the rule

1. Start hMailServer Administrator
2. Select *Rules*
3. Click on *Add*. Enter the name `SizeRule`
4. Under *Criteria*, click on *Add*.
5. Under *Pre-defined field*, select *MessageSize*
6. Under *Search type*, select Greater than.
7. In the *Value* textbox, enter the value 4000. This will cause all messages that is larger than 4000 bytes to be effected by this rule.
8. Click on *Save* to add the criteria to the rule.
9. Under *Actions*, click on Add.
10. Select *Run function* from the drop down. Enter the name *OnSizeRule*
11. Click on *Save* to save the Action
12. Click again on *Save* to save the entire rule

### Creating the script

1. Open the file Events\EventHandlers.vbs.
2. Add the following lines to it and save the changes:  

 **Sub OnSizeRule(oMessage)  

 Â Â  oMessage.Subject = "[4000] " & oMessage.Subject  

 Â Â  oMessage.Save  

 End Sub**

### Enable scripting in hMailServer

1. Start hMailServer Administrator
2. Navigate to Settings, Advanced, Scripts
3. Click Check syntax to confirm that the syntax is correct
4. Select Active
5. Click Save to enable scripting.
