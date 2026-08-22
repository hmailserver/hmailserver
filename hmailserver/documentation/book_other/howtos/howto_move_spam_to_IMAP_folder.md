---
id: 82
title: "Move spam to IMAP folder"
slug: howto_move_spam_to_IMAP_folder
parent: howtos
index: 0
is_book: false
updated: 2005-11-22
---

These instructions assume you're using hMailServer version 4.0 or higher. They are applicable to mail marked as spam using ASSP, but should work equally well with any other spam filter. It is only necessary that the filter put a spam flag in the header, for example, simply by replacing steps 3 and 4 below with the appropriate Header and Value.

1. Open hMailAdmin
2. Select *Rules* (to create a rule that applies globally)
3. Click *Add*
4. Give the rule a name
5. Click *Add* next to Criteria
6. Select *Custom Header* and enter *X-Assp-Spam*
7. Enter *YES* in the value field
8. Click *Save*
9. Click *Add* next to Action
10. Select *Move to IMAP Folder*
11. Under *Folder* enter the folder you want to move it to (example: *Inbox\SPAM*)

The folder will automatically be created for users if it doesn't exist the first time they receive a spam message. 

*Contributed by Colin Murphy*
