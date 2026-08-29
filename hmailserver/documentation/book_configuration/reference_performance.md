---
title: "Performance"
slug: reference_performance
parent: book_configuration
index: 0
is_book: false
---

## Overview

These setting lets you fine tune hMailServer performance.

### Cache

Use these settings to configure the hMailServer cache. hMailServer can cache objects located in the database. If an object is cached, hMailServer does not have to contact the database to access it. Enabling the cache may dramatically increase the hMailSever performance. Please see [the user guide](?page=details_cache) for information regarding these settings.

### Threading

#### Max number of command threads

When an SMTP, POP3 or IMAP client sends a command to hMailServer, this is handled by something called a command thread. The command threads typically handles simple commands, such as "log in", "retrieve message" and "add recipient to message". This number specifies how many simultanoeus commands can be run by clients. If you increase this value, more commands can run at the same time, but all of them will run slower. If you decrease this value, less commands can run at the same time, but they will be executed faster.

#### Delivery threads

hMailServer can deliver several email messages at the same time. This setting lets you define how many messages hMailServer should deliver simultaneously. A higher value will require more CPU usage. A lower value may result in slower deliveries. The best setting depends on your hardware and on the volume of messages you wish your server to deliver. For small installations, 3 simultaneous deliveries should be enough. For larger installations, you may want to raise this value to 15 or 20. For very large installations it should be set to a value between 50 and 100. The actual values which are optimal vary depending on user patterns, messages sent per day and so on.

#### Max number of asynchronous task threads

This setting defines how many parallel connections hMailServer will make to external software while running anti-spam protection. The settings defines for example how many connections hMailServer will open up to SpamAssassin at most.

#### Worker thread priority

hMailServer is a multi-threaded application. This means that inside the hMailServer service, several different things are done at the exact same time. For example, email is being received, delivered and clients are downloading email at the same moment. These different tasks are performed by something called worker threads. You can change the priority of worker threads, to change the amount of time the operating system should spend on these threads. If you lower this value, hMailServer will run slower and more CPU resources will be available to other tasks in the operating system. If you increase the priority, hMailServer will run faster but other software on your computer may run slower.

### Message indexing

When message indexing is enabled, some additional message meta data is stored in the database. This can greatly improve browsing speed in large folders when using a webmail client in combination with server side sort. The performance is achieved since hMailServer can sort the messages by retrieving parsed data from the database, rather than having to read all files in the folder and parse the content one at a time.

The downside of this feature is that the database size will increase. It's not recommended to enable this feature unless you're experiencing performance problems related to large folders in web mail.
