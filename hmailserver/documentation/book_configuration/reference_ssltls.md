---
title: "SSL/TLS"
slug: reference_ssltls
parent: book_configuration
index: 0
is_book: false
---

When hMailServer communicates with other clients and servers (called peers), it is possible to enable encryption of the TCP/IP connection. In hMailServer, this is called Connection security.

There are different types of connection security which can be enabled, and what to use depends on requirements and the functionality available in servers the hMailServer installation communicates with. Before selecting connection security, make sure that the remote peers supports it.

### Connection security types

- None  

 This option means that there is no connection security. If you select this option, the communication will not be encrypted.
- SSL/TLS  

 With this option enabled, the communication will be encrypted using either SSLv3 or TLS. Directly after hMailServer has established a connection with the remote peer, it will perform a SSL/TLS handshake. If the handshake fails, the connection will be terminated.
- STARTTLS (Optional)  

 With this option enabled, hMailServer will attempt to use STARTTLS with SSL/TLS. If the remote peer does not support STARTTLS, or if the peers can not agree on a cryptographic protocol and cipher, hMailServer will fallback to a connection with no security. If the peers agree on a cryptographic protocol and cipher, but the certificate verification fails, the connection will be used despite the failed certificate verification. In this case, a message will be logged in the debug log.
- STARTTLS (Required)  

 With this option enabled, hMailServer will attempt to use STARTTLS with SSL/TLS. If the remote peer does not support STARTTLS, or if the SSL/TLS handshake fails, the connection will be terminated. No fallback to a connection without security is made.

### SSLv3 or TLS

During SSL/TLS handshake, the peers will agree on what cryptographic protocol and cipher to use. This is based on the implemented support in the two peers, and the configuration of them. For example, if a client which only supports SSLv3 connects to a server which supports both SSLv3 and TLS1.1, then SSLv3 will be used. If there is no protocol support in common, the handshake will fail.

<p>In hMailServer, it's possible to <u>override</u> the default SSL/TLS cipher list. To do this, open hMailServer Administrator and navigate to Settings -&gt; Advanced -&gt; Security. In the SSL/TLS ciphers text box, you can enter an OpenSSL cipher list.</p>

### Certificate verification

As a part of the SSL/TLS handshake, hMailServer will verify that the server it connects to has a correct certificate. This includes several things, for example:  

- The remote peer has a certificate
- The certificate is currently valid
- The certificate is trusted, according to the Windows certificate store
- The certificate is issued for a host name which matches the host name hMailServer connected to.
- The certificate has not been revoked.

If one of these steps fail, the certificate check fails.

Note that hMailServer does not verify remote client certificates - it only verifies the certificates of servers it connects to. If a end-user connects to hMailServer using for example Thunderbird or Outlook, he does not have to provide a certificate. A remote server which delivers a mail to hMailServer does not have to provide a certificate either. In these scenarios - where a client is connecting to hMailServer, hMailServer will present a certificate which will be used to encrypt the session.

### Disabling verification

During testing, you may want to disable certificate verification completely. To do this, de-select this option. Keeping certificate verification enabled in production is strongly recommended.

### Certificate trust

hMailServer uses Windows functionality to verify peer certificates and will hence trust certificates which Windows is configured to trust. 

### MX-resolved deliveries

When a MX-resolved delivery is performed, hMailServer will use connection security *STARTTLS (Optional)*. This means that hMailServer will attempt to set up an encrypted communication channel but if this fails it will fall back to a connection with no security.
