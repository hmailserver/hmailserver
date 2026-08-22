---
id: 433
title: "Database maintenance"
slug: maintenance_database
parent: book_maintenance
index: 0
is_book: false
updated: 2009-02-10
---

## Background

There is a built-in limitation in hMailServer which occurs when 2 billion messages has passed the server.

Every message in a hMailServer installation has a unique numeric identifier.

- When the first message is received in a hMailServer installation it receives ID 1.
- When it is copied to the recipients inbox it receives ID 2.
- If it's copied to another IMAP folder by the user it receives ID 3.

This message ID is used in the communication with IMAP clients. The IMAP protocol specifies that this value must never be higher than about  4 billion. This means that if you receive 2 billion messages, you will reach this limit. If you're using IMAP, you'll reach the limit sooner since a message receives a new ID when it's copied to a new folder.

### Determine current situation

If you want to determine the current situation, you can run the following SQL statement in the hMailServer database. It will show you the currently highest assigned message id in your database.

<p style="margin-left: 40px;"><span class="Code">select max(messageid) from hm_messages</span></p>

If this value is higher than 4 000 000 000, you will soon run into this probem.
