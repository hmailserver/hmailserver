---
id: 108
title: "Clear the delivery queue"
slug: howto_clear_delivery_queue
parent: howtos
index: 0
is_book: false
updated: 2005-11-22
---

## Background

The hMailServer delivery queue consists of two parts, the queue message list in the database and the actual messages in the data folder. How you delete the delivery queue depends on which hMailServer version you use.

## hMailServer 4.1 and earlier

To delete the delivery queue, delete all the files in the data directory root. If you have installed hMailServer in *C:\Program Files\hMailServer* you should delete all files in the directory, *C:\Program Files\hMailServer\Data\*. For every file you delete, hMailServer will delete the message from the database and report an error to the error log that the message could not be delivered since the message file has been removed. Do not delete any sub folders in the data directory, as they might contain messages for the user accounts.

## hMailServer 4.2 and later

In the Status section in hMailAdmin, select the "Undelivered messages" tab and click *Clear queue*. hMailServer will then start to clear the entire queue.
