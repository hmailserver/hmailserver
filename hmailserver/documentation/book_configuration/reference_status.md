---
title: "Status"
slug: reference_status
parent: book_configuration
index: 0
is_book: false
---

## Overview

The status dialog gives you information on the current state of your hMailServer installation.

### Server

On the server page, you can start and stop the hMailServer server, see the current hMailServer version and information on database backend. When you start or stop the server from here, you do not actually start or stop the actual server *service*. Even if you click on Stop, the hMailServer service will continue running. That is, the server stops, not the service as a whole. The service is the executable file, hMailServer.exe, whereas the server is the sub-component in the executable, that accepts connections from clients.

Under configuration warnings, you can see a list of configuration problems which the self-test in hMailServer Administrators have detected. Most of these will be problems you want to resolve as soon as possible.

### Status

The status page shows information on when the server was started, the number of processed messages and the number of sessions currently active for the different protocols. Note that the message counters increase while messages are being processed (never decrease) while the number of open sessions decreases when a client disconnects from the server.

- Processed messages  

   

 This is the number of messages hMailServer has accepted for delivery and started to deliver. They have either been delivered to hMailServer by a SMTP client, or downloaded from an external POP3 account, or created directly in the hMailServer database or using the API.  

  
- Viruses detected (before: Messages containing Virus)  

   

 This is the total number of messages hMailServer has processed in which viruses have been found. This is expected to be same or lower as the number of processed messages.  

  
- Spam messages (before: Messages containing spam)  

   

 This is the total number of messages hMailServer has detected to contain spam. These messages may have been delivered to the recipients, depending on the server configuration. For example, if hMailServer is configured to drop all spam immediately, the count may be higher than the number of processed messages.

### Logging

Using the live log, you can see what is happening in your hMailServer installation without have to manually open the log. The live log is primarily used for troubleshooting. To use the log, you first go to the logging settings and enable which details you want to log. After that, you then open up this page and click Start. Whenever something is added to the log, it will automatically appear on this page.

### Delivery queue

The delivery queue page shows messages which are in the queue to be delivered. The following tasks can be performed:

- Refresh - The list you see is a snapshot from the time when you opened the page. If you wait a few minutes several of the messages will have been delivered. To see a new snapshot, click refresh.
- Clear queue - Select clear queue to remove all messages from the delivery queue. Messages currently  being delivered might still be delivered, depending on the exact state at the moment.
- Show (right click in list) - Choosing this will open up a new dialog which allows you to see the message content.
- Send now (right click in list) -This will queue the message for immediate delivery. This means that if there is an available delivery thread, thedelivery will start immediately. If not, the delivery will start when adelivery thread is available.
- Remove (right click in list) - this operation will remove a message from the delivery queue.

 
