---
title: "Connect to the database"
slug: ts_connect_to_database
parent: troubleshooting_tips
index: 0
is_book: false
---

Open the hMailServer Log directory in Windows Explorer. Open the file named ERROR_*.log in a text-editor
and look at the last lines in it.

- Error: Cannot connect to xx on host xx   

 Reason: Cannot connect to xx on host xx. hMailServer cannot connect to the database.
- Error: Cannot open requested database   

 Reason: Make sure that the database specified in hMailServer.ini actually exists and that the connecting user has
 the appropriate rights to use the database.
