---
id: 256
title: "hMailServer Administrator error messages"
slug: reference_error_messages_hmailadmin
parent: book_error_messages
index: 0
is_book: false
updated: 2017-02-14
---

## hMailServer Administrator errors

### The remote server machine does not exist or is unavailable

When you connect to a hMailServer instance using hMailServer Administrator, DCOM is used. hMailServer connects to the hMailServer service using the DCOM API and then manages the server via the API. hMailServer Administrator (and PHPWebAdmin) does not connect directly to the database server. Because of this, to be able to run hMailServer Administrator, the hMailServer service must be running.

If hMailServer Administrator is unable to connect to the hMailServer service, you may see the error above. By default, hMailServer Administrator tries to connect to the hMailServer instance running on localhost. If you get this error message, make sure that the host name you're trying to connect to in the connection dialog really is correct. If it is, you should ask a question in the [user forum](https://www.hmailserver.com/forum/)
