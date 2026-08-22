---
id: 9
title: "Storing the message in the database"
slug: faq_storing_message_in_database
parent: FAQ
index: 0
is_book: false
updated: 2008-10-27
---

## Question: Why aren't entire e-mail messages stored in the database?

Storing e-mail messages could have some advantages. For example, it would be easier to backup hMailServer if all data was stored in the database. However, it would also have some serious drawbacks:

### Higher memory usage

When a message is being delivered by hMailServer to a client or to another email server, hMailServer delivers the email in 50kb-parts. hMailServer reads 50kb worth of data from the email message stored on disk to memory. It sends it to the client and then frees the 50KB from memory. So, when hMailServer delivers an email message, the memory usage never needs to exceed 50KB per message. There is no good way to read 50KB-parts of an email message from the database. If the email was stored in the database, hMailServer would have to read the *entire* email from database and cache that in memory, which would dramatically increase memory usage.

### Higher CPU usage

Storing the message in the database would involve several additional tasks, which would all consume CPU cycles:

<ul>
    <li>If the message was stored in the database, it would not be possible to scan it using an anti-virus scanner. Then hMailServer would have to extract the entire message from the database and store it on a hard disk, before scanning it, and then remove the file from disk again. You could use an in-memory virus scanner to solve this but far from all virus scanners has such API's.</li>
    <li>Older versions of MySQL have quite a low maximum query length limit. Storing a message when using these versions would require hMailServer to split up the data in several SQL-statements and run them one by one.</li>
    <li>It is not possible to run <code>insert into table (column) values ('the-puremessage-data')</code>. Before doing this, hMailServer 	would have to search through the entire message and "escape" special characters, for example, escape <u>'</u> to <u>\'</u>.To circumvent this, you could use stored procedures but not all database engines which hMailServer supports supports stored procedures.</li>
</ul>

### Harder administration

Storing email messages on the disk gives the administrator a good overview of the messages on the server. It is possible to view an email message just by doubleclicking it, and to delete it just by pressing Delete on the keyboard. If the messages were stored in the database, the administrator would have to know how to run SQL statements.
