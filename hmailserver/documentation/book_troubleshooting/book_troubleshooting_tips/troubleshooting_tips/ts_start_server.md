---
id: 47
title: "Starting the server"
slug: ts_start_server
parent: troubleshooting_tips
index: 0
is_book: false
updated: 2005-11-22
---

## Terminal services

The most common reason for hMailServer's failure to start is that Terminal Services are used. The hMailServer installation software creates a file in the Windows directory called hMailServer.ini. The file
is created in the *real* windows directory, C:\Windows. However, when the service is started, Windows pretends that the Windows directory is located under C:\Documents And Settings\*Username*\Windows. As a result, hMailServer
tries to read from that file. Since the file does not exist, the hMailServer service fails to start.
(hMailServer 4.0 and forward will store hMailServer.ini in the hMailServer Bin directory to avoid this problem.)

## Ports are in use

Another common reason for hMailServer not to start is that some other software might be using the TCP/IP ports that hMailServer needs. Some versions of Windows have, by default, a built-in mail server. It is extremely light-weight. In Windows 2000/XP, it only includes SMTP (not POP3), and in Windows 2003, it does not include IMAP. This built-in server may occupy the port that hMailServer requires.

If hMailServer stops immediately after you have started it, the first thing to do is to check the Windows Event Viewer. The Event Viewer is found in the Administrative Tools folder in the Control Panel. hMailServer reports any fatal errors there. If the server doesn't start or if it stops unexpectedly, that is the first place to look. 

If the Event Viewer does not help, try to telnet your computer on port 25. When you connect, there will be a message saying which mail server you are using. Locate this mail server in the Services applet in the Control Panel and stop/disable it. If no error message is shown, make sure you are not currently running an e-mail anti virus scanner.
