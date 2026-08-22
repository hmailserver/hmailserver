---
title: "Logging"
slug: reference_logging
parent: book_configuration
index: 0
is_book: false
---

## General

### Enable logging

<div class="indented">This option enables the logging. If logging is disabled, nothing will be logged except for errors.</div>

### Application

<div class="indented">The application log contain major server events, such as server start, stop and message delivery information.</div>

### SMTP conversation

<div class="indented">The SMTP conversation log contains SMTP communication that hMailServer makes. Both when hMailServer acts as a SMTP client (when delivering) and when hMailServer acts as a SMTP server (when receiving).</div>

### POP3 conversation

<div class="indented">The POP3 conversation log contains POP3 communication that hMailServer makes. Both when hMailServer acts as a POP3 client (when downloading messages from external servers) and when hMailServer acts as a POP3 server (when email clients downloads messages from hMailServer).</div>

### IMAP conversation

<div class="indented">The IMAP conversation log contains IMAP communication that hMailServer makes.</div>

### TCP/IP

<div class="indented">The TCP/IP log contains TCP/IP events, such as DNS queries, connection opening and closing etc.</div>

### Debug messages

<div class="indented">The Debug messages log contains low level messages. Debug logging should only be turned on when troubleshooting.</div>

### AWStats

<div class="indented">The AWStats log (saved in hmailserver_awstats.log) contains SMTP delivery events formatteded for <a href="http://awstats.sourceforge.net">AWstats</a>.</div>

<div class="indented"><br>
In the AWstats configuration, the following settings should be used:<br>
<font class="Apple-style-span" face="'Courier New'"><br>
</font> <span class="Code">LogFormat="%time2 %email %email_r %host %host_r %method %url %code %bytesd"<br>
LogSeparator="\t"</span></div>

<div class="indented"><span class="Apple-style-span" style="font-family: 'Courier New'; ">LogType=M</span></div>

### Keep files open

<div class="indented">This setting lets you specify whether hMailServer should keep log files open inbetween writes. The default log writing behavior in hMailServer is that whenever hMailServer should append something to a log file, the file is open, written to and after that closed.  This means that any data hMailServer writes to the file is flushed immediately. If there are a lot of small writes to the log files, this may be bad for performance. </div>

<div class="indented"> </div>

<div class="indented">If you configure hMailServer to keep log files open, Windows will buffer data being sent to the log file. This may dramatically improve log file performance. The downside is that the log files can not be deleted while they are open. When hMailServer has finished writing to a log file (in the case of the normal log files, this happens at midnight when a new file name is generated), hMailServer will close the file and the file can be deleted.</div>

<div class="indented"> </div>

<div class="indented"><br>
 </div>

 
