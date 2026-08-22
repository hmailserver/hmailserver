---
id: 222
title: "Command handlers"
slug: internals_commandhandlers
parent: book_internals
index: 0
is_book: false
updated: 2007-03-15
---

# Overview

The communication between an email server and an email client consists of a large number of different commands and transferring of data. Typically, an email client connects to an email server over TCP/IP and transfers data via a socket. When the email client has sent all its commands, it disconnects and closes the TCP/IP connection. Examples on commands a client can send are login commands, message listing, message retrieval, mailbox creation and so on.

## The old thread pool

In hMailServer 4.2 and earlier version, hMailServer started a new operating system thread for every new TCP/IP connection. (Simply put, threads are a way for software to run multiple tasks at the same time). Every thread was responsible for monitoring a single TCP/IP connection, and handling any commands the client sent. Starting a new thread is a quite CPU intensive operation. To solve this, hMailServer had a so-called *thread pool*. During start-up, it estimated how many threads it would need, and started them immediately even though no client had connected. When a client then connected, hMailServer gave one of the threads the responsibility to handle this email client.

The hMailServer 4.2 thread pool rarely contained enough threads, so new threads had to be started when clients connected. As mentioned earlier, starting threads is a CPU intensive operation so this wasn't good for performance. The workaround to this was to start a large number of threads during service start up. But running many threads at the same time is bad for performance as well. A single-core CPU can only execute one task at a given time. This means that if software wants to execute 2 things at the same time, the CPU will first do a small part of task 1, then a small part of task 2, then a small part of task 1 and then task 2 again and so on. This switching back and forth (*context switching*) leads to *thread trashing* - the operating system has to spend more time on switching task than to actually performing the tasks.

## The new command handler

In hMailServer 4.3, a new method based on *command handlers* is used. During service start-up, a fixed number of commands threads are started (defined in *Advanced->Performance->Threading* settings). hMailServer tells Windows that these threads are ready to process commands from TCP/IP clients. When an email client connects, hMailServer tells Windows that it is interested in any data this client sends.

When an email client sends a command to the server, the Windows kernel gives the task of processing the command to one of the command threads. The command thread will process the command and then tell Windows that it's ready to process another command. This means that a single command thread can run commands for many different email clients. Most of the time, email clients are idle (does not send any commands) so this leads to more efficient thread usage.

This also means that hMailServer cannot process more simultaneous client commands than the number of command threads. This is normally not a problem since commands are processed very quickly. Since the number of commands are limited, every command can execute faster than if the number of parallel commands were unlimited. Client commands are queued until a command thread is available to do processing.

Since a single CPU core can never run more than one task at a given time, the command handlers in hMailServer 4.3 will give overall better performance than the thread-per-connection method in hMailServer 4.2.
