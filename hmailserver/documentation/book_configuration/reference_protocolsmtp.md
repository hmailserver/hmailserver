---
title: "SMTP settings"
slug: reference_protocolsmtp
parent: book_configuration
index: 0
is_book: false
---

## General

### Connection

<div class="indented">The maximum number of simultaneous SMTP connections to the server. If this value is set to zero, an unlimited number of simultaneous connections will be allowed. By default, the value is set to zero. This is server wide, not per client.</div>

### Welcome message

<div class="indented">The welcome message is sent to SMTP clients directly after they have connected to the server. This message is normally never seen by the sender or receiver. One reason to change the welcome message is to make it harder for other people to determine what server software you are running.</div>

### Max message size

<div class="indented">If a Max Message Size is specified, hMailServer will reject messages larger than that size. If you wish to allow messages of unlimited size, set the value to zero. The size is specified in KB.</div>

<div class="indented"><br>
This value can be overridden in the <a href="?page=reference_domain">domain settings</a>. If a value is specified in the domain settings, this will be used instead of the value specified in the SMTP settings.</div>

<div class="indented"> </div>

<div class="indented">It is strongly recommended that you use a max message size limit. Having no message size limits will leave your server open to different types of attack. For example, users could send a message so big that it fills the server hard drive, which will cause unpredictable behavior. The default maximum message size is 20MB.</div>

## Delivery of email

### Number of retries

<div class="indented">This setting defines the number of times hMailServer should try to deliver an email. Deliveries may fail for a number of reasons. For example, the recipient's email server may be rebooting or your network may be temporarily unavailable. The default value is 4 retries, which means hMailServer will try a total of 5 times before giving up and returning an error message to the sender.</div>

### Minutes between every retry

<div class="indented">This setting defines how many minutes hMailServer should wait before every retry, when delivering emails to other servers. The default value is 60 minutes.</div>

### Host name

<div class="indented">When an SMTP server connects to another server to send a message, the first thing that happens is that the sending server identifies itself using the host name. Since there is no way to safely auto-detect the host name of a computer, you have to specify this setting manually. The host name must resolve to the IP address of the computer which is running hMailServer. Some servers will validate this and classify your email as spam if it does not resolve properly.</div>

<div class="indented"><br>
It does not matter what host name you enter, as long as it resolves to the IP address where hMailServer is running. You may have 15 different host names which resolves to the IP address hMailServer is running on. If this is the case, you can enter any of these 15 different host names in the Host name field.<br>
<br>
Example: If hMailServer is running on a machine whose host name is mail.domain.com, you should specify mail.domain.com as host name. If your machine has several public host names, such as mail.domain.com and mail.domain2.com, you may specify any of them as host name.</div>

### SMTP relayer

<div class="indented">The SMTP relayer setting lets you specify which email server email messages should be delivered to. You should never set the value to "localhost" or to the hostname of your own email server. That would cause hMailServer to try to connect to itself.  <br>
<br>
When one SMTP server delivers email to another, DNS-MX lookup is normally used. This means that if you send an email to me, at someone@hmailserver.com, your email server will do an MX lookup for my domain, hmailserver.com. The MX response will tell your server that it should deliver the message to mail.hmailserver.com. That communication occurs via port 25. However, it can happen that your ISP blocks outgoing traffic on the SMTP port (25) to all computers except their own email server. You can therefore not connect to mail.hmailserver.com. In that case, you should configure hMailServer to send all email through your ISP's email server. Your ISP's email server is then your relayer. The value to enter in the relayer field is the name of your ISP's email server. For example, if you happen to use the Swedish broadband provider Bredbandsbolaget, you should specify smtp.bredband.net as SMTP relayer.  <br>
<br>
If you don't want to relay all outgoing messages through a specific SMTP server, this field should be left empty.</div>

### SMTP relayer TCP port

<div class="indented">The TCP/IP port hMailServer should connect to when delivering to the SMTP relayer.</div>

### Server requires authentication

<div class="indented">Select this if the server you have specified as SMTP relayer requires authentication.</div>

### Connection security

<div class="indented">If the SMTP relay server supports SSL or STARTTLS, you can configure hMailServer to use this when delivering the message. More information about this topic can be found on <a href="?page=reference_ssltls">the connection security page</a>.</div>

<div class="indented"> </div>

<div class="indented">
<h2>RFC compliance</h2>
<h3>Allow plain text authentication</h3>
<div class="indented">This option tells the SMTP server in hMailServer whether or not plain authentication should be allowed.</div>
<div class="indented">
<h3>Allow empty sender address</h3>
<div class="indented">Some spammers send email with empty sender address. If you disable thisoption, hMailServer will treat these messages as spam. However, some legitimate email also has empty sender address, so it's strongly recommended that you do not disable this option.</div>
<div class="indented">
<div class="indented">
<h3>Allow incorrectly formatted line endings</h3>
<div class="indented">According to the SMTP specification, every line in an email message should be separated by the ASCII-codes 13 and 10. Some spammers sends messages which are not correctly formatted. Use this setting to reject these messages. Please note that legitimate email might have incorrectly formatted line endings, if the sending software contains bugs.</div>
<div class="indented">
<h3>Disconnect client after too many invalid commands</h3>
Using this setting you can disconnect clients which sends to many invalid commands. For example, some spammers try to send email to a lot of different addresses on your server, hoping that your server will accept at least one of them. Using this option, you can automatically disconnect clients that tries to do this.</div>
</div>
</div>
</div>
</div>

## Advanced

### Bind to local IP address

<div class="indented">Use this setting to specify which local IP address hMailServer should use when connecting to other SMTP servers. This can be used if your server has several public IP addresses but you want to use one specific for deliveries. If this setting is not specified, hMailServer will use the Windows default, which works in most cases. An example is '192.168.0.10'.</div>

<div class="indented">
<h3>Maximum number of recipients in batch</h3>
<div class="indented">SMTP servers may reject messages from hMailServer if there are too many recipients for a single email. This may happen if the receiving SMTP server thinks that your email message is spam because you are sending it to a large number of users. Use this setting to limit the number of recipient hMailServer uses in the same delivery. When this number has been reached, hMailServer will disconnect from the recipient server, connect again and continue with the remaining recipients.</div>
<h3>Use STARTTLS if available</h3>
<p>With this option enabled, hMailServer will attempt to use STARTTLS with  SSL/TLS. If the remote peer does not support STARTTLS, or if the peers  can not agree on a cryptographic protocol and cipher, hMailServer will  fallback to a connection with no security.   If the peers agree on a  cryptographic protocol and cipher, but the certificate verification  fails, the connection will be used despite the failed certificate  verification. In this case, a message will be logged in the debug log. </p>
<h3>Add Delivered-To header</h3>
</div>

<div class="indented">If this option is selected, hMailServer will add a Delivered-To header to all email messages. This header will contain the recipient address as given in the SMTP envelope during SMTP communication.</div>

<div class="indented"> </div>

<div class="indented">Please note that this header is added to the mail *after all rules and  script processing* is done, and can therefore not be accessed from  rules/scripts.</div>

### Rule loop count

<div class="indented">This option lets you prevent hMailServer from creating endless message delivery loops. As an example, it's possible to set up an account rule that forwards message from one user (UserA) to another (UserB), and then another rule that forwards the message back from UserB to UserA. To prevent this from resulting in an endless loop, hMailServer limits the number of automatic forwards to the value defined by <em>Rule loop count</em>. (hMailServer 4.2 and later.)</div>

### Maximum number of recipient hosts

This options lets you specify the maximum number of MX records per domain that hMailserver will attempt delivery to. Default value is 15.
