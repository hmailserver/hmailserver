---
id: 29
title: "Use ASSP"
slug: howto_use_assp
parent: howtos
index: 0
is_book: false
updated: 2005-11-22
---

## Background

How to configure hMailserver and ASSP to filter SPAM (Jason Weir: jason.weir@comcast.net, April 14, 2004)  
 
This configuration was tested with both hMailserver and ASSP running on the same Windows 2000 Professional box, but it should work for practically any system that will run hMailServer.

## Software Versions

- ASSP v1.0.9 -   

- hMailServer v2.0.x and 3.x  

- Activestate Perl v5.6.1

## Requirements

hMailserver up and running, configured correctly and is processing mail  

Perl installed with Win32  

Daemon module installed

## Configuring hMailServer

1. Open hMailServer Administrator
2. Under Settings->SMTP, change SMTP Port to 125, save settings and restart server

## ASSP Installation & Configuration

1. Installation is straightfoward, download and unzip files to c:\ASSP
2. Create the following directories:  

   - c:/assp/spam
  - c:/assp/notspam
  - c:/assp/errors
  - c:/assp/errors/spam
  - c:/assp/errors/notspam
3. Open a command prompt from c:\assp. Run: perl assp.pl
4. Point your browser to http://127.0.0.1:55555/ blank username, password is *nospam4me*
5. Check the "Show Advanced Configuration Options" checkbox and click the *Apply Changes* Button
6. Change the following Settings:  

   - SMTP Destination = 127.0.0.1:125
  - Check "As a Service" checkbox
  - Listen port = 25
  - Local Domains = list the domains you have hMailServer configured for
7. Click the *Apply Changes*
8. Press Ctrl-C to stop the assp.pl process
9. Run: perl addservice.pl -i c:\assp\assp.pl c:\assp
10. Start the Anti-Spam Smtp Proxy service
11. Test by trying to telnet to 127.0.0.1:25; you should see the mail server banner.

This gets ASSP working in Test Mode. Mail should now pass through ASSP to hMailServer. You will notice messages building up in the spam and notspam directories. If you see spam in the notspam folder or vice versa you must manually move those messages to the correct folder. Once you have a build-up of 100 or so messages in the spam and notspam folders, run the rebuildspamdb.pl script. This builds the spam database. Read the ASSP documentation on [turning off test mode](http://assp.sourceforge.net/cgi-bin/fom?_highlightWords=test%20mode&file=115) and on [automating rebuildspamdb.pl](http://assp.sourceforge.net/cgi-bin/fom?_highlightWords=test%20mode&file=115)
