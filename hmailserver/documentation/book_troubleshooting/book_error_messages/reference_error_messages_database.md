---
id: 254
title: "Database error messages"
slug: reference_error_messages_database
parent: book_error_messages
index: 0
is_book: false
updated: 2008-09-25
---

### MySQL server has gone away

If hMailServer looses the connection to the database server during a query, hMailServer tries to re-open the connection and re-run the query. If this fails, the error MySQL server has gone away is logged. This can happen if the MySQL server is run on another computer, and the network connection is dropped. It can also happen when the MySQL server is about to be stopped, for example just before a computer reboot. In most cases, this failure will not have any side effects.
