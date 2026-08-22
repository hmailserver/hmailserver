---
title: "Act as a MX backup"
slug: howto_act_as_mx_backup
parent: howtos
index: 0
is_book: false
---

## Overview

This document describes how to configure hMailServer to act as a MXÂ backup server for a domain hosted on another server.

### Background

In a DNS it is possible to configure a domain to have multiple mail exchange (MX) records. The different MX records have different preferences (also called priority). When a server tries to deliver an email to a domain on another server, it will first try to deliver to the host name in the MX record with the lowest preference / priority.Â If this fails, it will attempt to deliver to the second. If this fails, the third, and so on.

The e-mail servers running on the MX records with high preference can therefore be seen as backup mail servers - if the primary mail server fails to receive a message, the secondary /Â backup server may.

Please note that the term used here, backup mail server, only affects SMTP. If a delivery is made to the backup mail server and the backup mail server forwards the message to the primary server, the message will be deleted from the backup mail server when it has been successfully received by the primary server. So to download messages using POP3 or IMAP, users would always have to access the primary mail server.

### Example

Here's an example on how to set up a mail backup server for the domain hMailServer.com. There are two things which need to be made:

1. A route needs to be added to hMailServer on the backup server which instructs hMailServer to forward email to the primary domain.
2. The DNS records needsÂ  to be updated so that the secondary server receives email if the primary server does not respond.**

In this scenario, there are two mail hosts and servers involved:

- mail.hmailserver.com
- mail-backup.hmailserver.com

*Configuring the DNS*

In the DNS, two records should be added. One for the primary server and one for the secondary. It's important that the preference value for the primary is lower than the value for the secondary. If a lower preference value is given to the backup server, **all** email will go through the abackup server.

<table width="350">
    <tbody>
        <tr>
            <td><b>Host</b></td>
            <td><b>Preference</b></td>
        </tr>
        <tr>
            <td>mail.hmailserver.com</td>
            <td>25</td>
        </tr>
        <tr>
            <td>mail-backup.hmailserver.com</td>
            <td>50</td>
        </tr>
    </tbody>
</table>

*Configuring hMailServer*

The sender of an email will first try to deliver to the primary server, in this case mail.hmailserver.com. If the delivery to this host fails, the sender will try to deliver to the backup server, mail-backup.hmailserver.com. The server mail-backup.hmailserver.com will then have to forward the email to mail.hmailserver.com whenever that server goes online. If the primary server does not come online within a specified amount of time, a bounce message will be delivered to the original sender saying that the message could nto be delivered.

The steps below show how to configure the hMailServer installation running on mail-backup.hmailserver.com to forward all email for the domain hmailserver.com to the host mail.hmailserver.com.

1. Start hMailServer Administrator.
2. Expand the Settings node in the tree to the left.
3. Expand the Protocols node.
4. Expand the SMTP node
5. Select Routes and click Add.  

   

 In a route, you can specify where email should be delivered depending on the recipient's domain name.  

 Â 
6. Specify the following settings:   

   

   - *Domain* - The name of the domain you wish to provide backup for. In this example, this is hmailserver.com.Â Note that you should not add this domain to your hMailServer installation.Â You only need to set up a route to provide backup functionality.  

 Â 
  - *Target SMTP Host* - The host name or the IP address of the computer all e-mails to the domain should be forwarded to. In this example, this is mail.hmailserver.com.  

 Â 
  - *TCP Port* - The TCP/IP port that the Target SMTP host is listening on. In our case, this is port 25 since mail.hmailserver.com is listening on this port.  

 Â 
  - Select that both sender and recipient should be treated as local if they are matching the route.
  - *Address page* - If you have a list of the existing e-mail addresses on the target SMTP host, you can specify them here. If you do this, hMailServer will reject e-mail sent to non-existing addresses.  

 Â 
  - *Delivery page* - Specify the number of times hMailServer should try to deliver to this server.
