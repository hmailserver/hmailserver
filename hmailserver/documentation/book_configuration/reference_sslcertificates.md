---
title: "SSL certificate"
slug: reference_sslcertificates
parent: book_configuration
index: 0
is_book: false
---

<h2><span style="color: rgb(119, 119, 119); font-size: 18px;">Settings</span></h2>

### Name

<div class="indented">The name of the SSL certificate. The name is only used for displaying and must not have any connection to the SSL certificate.</div>

### Certificate file

The certificate file to use. This certificate should conatin the trust chain (Root CA and any Intermediary Certificates), in addition to the Server Certificate. Please see the user forum for some 'How Tos' for various types of SSL Certificates.

### Private key file

The private key file to use.

hMailServer will be unable to read the private key if it has a password. Be sure to strip the password from the key before configuring hMailServer to use the file.

### Other

For general information about how hMailServer uses SSL certificates, see [Connection security](?page=reference_ssltls).

## Configuring hMailServer to use a SSL certificate

There are two tasks involved with configuring hMailServer to use an SSL certificate:

### Adding the SSL certificate to hMailServer

1. Start hMailServer Administrator
2. Navigate to Settings->Advanced->SSL certificate
3. Click Add
4. Type in a SSL certificate name. This can be anything you like, but it's suggested that you set it to the host name in the SSL certificate.
5. Select the certificate file and private key filed
6. Save the changes

After following these steps, hMailServer knows about the SSL certificate, but you also need to tell hMailServer when to use it.

### Configuring hMailServer to use the SSL certificate

1. Start hMailServer Administrator
2. Navigate to Settings->Advanced->TCP/IP ports
3. Select a port 
4. Select "Use SSL" or STARTTLS and the certificate. For more info about these options, please see [Connection security](?page=reference_ssltls).
5. Save the changes
6. Restart hMailServer

 

###  

 
