---
title: "Domain"
slug: reference_domain
parent: book_configuration
index: 0
is_book: false
---

## Overview

Every email account in hMailServer must belong to a domain. The domains specified in hMailServer can be local network domains or global internet domains such as hMailServer.com.

## General

### Domain name

<div class="indented">The name of the domain. To be considered valid, a domain name must include a dot. You must set up your DNS servers so that email can be sent to your mail server.</div>

<div class="indented"> </div>

<div class="indented"><strong>IMPORTANT:</strong>  When EDITING (renaming) an <span style="text-decoration: underline">existing domain</span>  be aware that the process can take some time to complete and you will  temporarily need double the disk space taken by the old domain data  directory.  This is due to the process actually creating a data  directory for the new domain and then <strong>copying all email data from the old domain at operating system level</strong>  before then deleting the old domain directory.  Depending on the old  domain data size and processor power, this copying could take many  minutes.  It is advised to restrict email use for the old domain whilst  the copying of this data is taking place and to monitor for the process  completion by watching for the old domain data directory to finally be  deleted.</div>

## Names

<div class="indented">One domain can have several names. These are also known as domain aliases. For example, your organization might own the domain, company.com, but it might also own company.org, company.se and company.de. If you want to be able to receive email for all these domains, you will have two options:
<ol>
    <li>Add all four domains to hMailServer. The problem with this is that you then have to add every email account 4 times, once for each domain.</li>
    <li>Add company.com as a domain, and then, under it, in the <em>Names</em>-tab, add company.org, company.se and company.de. That is, you add company.org, company.se and company.de as aliases of company.com. That, usually, is the route most users prefer.</li>
</ol>
<p>If you set up a domain named example.com, and an alias named example.net, your server will accept email for both someone@example.com and someone@example.net. Your users will be able to log on as both someone@example.com and someone@example.net as well.</p>
<h3>Creating a domain name alias</h3>
<ol>
    <li>Start hMailServer Administrator.</li>
    <li>Expand the <em>Domains</em> node and select the domain (e.g. <code>mydomain.com</code>).</li>
    <li>Select the <em>Names</em> tab.</li>
    <li>Add the domain names to the list (e.g. <code>mydomain.org</code> and <code>mydomain.net</code>)</li>
</ol>
<h3>Notes</h3>
<ul>
    <li>You should not add the primary name (in our example, <code>mydomain.com</code>) to the list.</li>
    <li>You can not add the same domain name aliases to multiple domains.</li>
</ul>
<div class="indented">
<h2>Signature</h2>
<div class="indented">On the signature tab, you can configure hMailServer to add a signature to all email sent from this domain. It's possible to enter both a plain text version and a HTML version of the signature. If no HTML signature is specified, hMailServer will use the plain text signature as HTML signature as well.</div>
<div class="indented"> </div>
<div class="indented">Please note that the first two options, <em>Add signatures to replies</em> and <em>Add signatures to local email</em> applies both to domain signatures and account signatures. Even if you do not enable domain signatures, these two settings will have effect on account signatures.</div>
<div>
<ul>
    <li>Add signatures to replies<br>
    If this option has been selected, hMailServer will add signature to replies. To determine whether a message is a reply, hMailServer checks for the In-Reply-To and the References header in the e-mail. This option is de-selected by default. Note that this option affects all email sent within the domain - not just domain signatures.<br>
     </li>
    <li>Add signatures to local email<br>
    If you select this option, hMailServer will add signatures to local email. An email is considered local in this case if both the sender and all the recipients exist in the same domain. An email sent from one domain to another is not considered local, since the sender and recipient may not be aware of the fact that they both are hosted on the same server. Note that this option affects all email sent within the domain - not just domain signatures.<br>
     </li>
    <li>Enable domain signature<br>
    If you select this option, the specified signature will be appended to email.<br>
    <br>
    <ul>
        <li>Use signature if none has been specified in the senders account.<br>
        When selected, hMailServer will only use domain signature if an account signature has not been specified. <br>
         </li>
        <li>Overwrite account signature<br>
        If you select this option, hMailServer will not use the account signatures for this domain. Instead, the domain signature will be used for all email.<br>
         </li>
        <li>Append to account signature<br>
        When selected, hMailServer will append the account signature with the domain signature. This can be use if you for example want to add disclaimers to all outgoing email.</li>
    </ul>
    </li>
    <li>
    <p>Plain text signature / HTML signature<br>
    These two fields specify the domain signature to be used</p>
    </li>
</ul>
</div>
It is possible to use the macros %User.FirstName% and %User.LastName% in signatures. These macros will be replaced with the users first and last name as specified in the account settings.</div>
<div class="indented">
<h2>Limits</h2>
<h3>Maximum size (MB)</h3>
<div class="indented">If this value has been specified (is not 0), the system administrator and the domain owner will be prevented from adding accounts so that the total size of all accounts exceeds this value. If you have specified 500MB, the total size of all messages in the domain will not exceed 500MB.</div>
<h3>Maximum message size</h3>
<div class="indented">If specified, hMailServer will reject messages larger than this size.</div>
<div class="indented">If not specified, the server-wide Max message size defined in the <a href="?page=reference_protocolsmtp">SMTP settings</a> will be used.</div>
<div class="indented">
<h3>Maximum size of accounts (MB)</h3>
<div class="indented">If this value has been specified (is not 0), the server administrator and the domain owner will be prevented from adding accounts with a total size larger than this value.</div>
</div>
<h3>Number of accounts, aliases and distribution lists</h3>
<div class="indented">Using these settings, you can limit the number of accounts, aliases and distribution lists server administrators and domain owners can create in this domain.</div>
<div class="indented">
<div class="indented">
<h2>DKIM signing </h2>
<p>DKIM, Domain Keys Identified Mail, is a method to sign the content of messages. The recipient can verify that the message is sent from a server authenticated to send from the sender's domain, and that the message content has not been modified in transit.</p>
<p>Before hMailServer delivers a message to another server, it will look at the sender address of the message. If the sender address matches a local domain, the DKIM signing options from that domain will be used. If DKIM signing has been enabled, the message will be signed.</p>
<p>Whether or not the original sender has used SMTP authentication when delivering the message to hMailServer has no effect on the DKIM signing process.</p>
<p>hMailServer signs a message with DKIM before deciding whether it will be delivered locally or to another server, so messages between two accounts on the same server are signed as well.</p>
<p>hMailServer only signs messages smaller than an internally-defined limit. Currently that limit is set to 50MB in the program code. The limit is not runtime-configurable.</p>
<h3>Private key file</h3>
<div class="indented">The private key to use when signing messages with DKIM. This must be a file existing on the local file system, readable by hMailServer, and the file must not have a password set.</div>
<h3>Selector</h3>
<div class="indented">This is the DKIM-selector to use for signing. To be able to use DKIM, you must specify a selector. The selector must be the same as the selector you are using for your DKIM record in your DNS server. For example, if your DNS record is named myselector._domainkey.example.net, you should enter "myselector" as selector (without quotes).</div>
<div class="indented">
<h3>Header method</h3>
<div class="indented">Choose between simple and relaxed canonicalization method. If you choose the simple canonicalization method, the signed headers of the message must not be modified at all. If a new line is added in an header the verification will fail. Choose the relaxed canonicalization method if you want to allow minor modifications to header li</div>
</div>
<h3>Body method</h3>
<div class="indented">Choose between simple and relaxed canonicalization method. If you choose the simple canonicalization method, the body of the message must not be modified at all. Choose the relaxed canonicalization method if you want to allow minor modifications to the body.</div>
</div>
</div>
</div>
<div class="indented">
<h3>Signing method</h3>
Choose between the algorithms SHA1 and SHA256. SHA256 is encouraged since it gives higher security than SHA1. Senders of low-security messages such as newsletters may want to use SHA1 instead since it requires less CPU resources.<br>
<h2>Advanced</h2>
<div class="indented">This tab contains the advanced settings for the domain. You normally don't need to modify these settings.</div>
<div class="indented">
<h3>Catch-all address</h3>
<div class="indented">
<p>It is possible to specify an email address that receives all emails being sent to non-existent addresses on your domain. For example, you may have <code>sales@mydomain.com</code>, <code>webmaster@mydomain.com</code> and <code>support@mydomain.com</code> as existing accounts. But there's a risk that someone might misspell an email address, writing <code>sails@mydomain.com</code> instead of <code>sales@mydomain.com</code>. <br>
<br>
The solution is to specify an account - either a previously existing one, or one created for the purpose - to be the <i>catch-all account</i>. All email sent to non-existent addresses on the domain will then be delivered to the catch-all account.</p>
<h4>Example</h4>
<ol>
    <li>Start hMailAdmin</li>
    <li>Expand the <i>Domains</i> node and select the domain, say, <code>mydomain.com</code></li>
    <li>Create a new account with the name <code>catchall@mydomain.com</code></li>
    <li>Select your domain, and enter <code>catchall@mydomain.com</code> as catch-all address</li>
    <li>Save the changes</li>
</ol>
<h4>Notes</h4>
<ul>
    <li>The catch-all address can be any email address you like. It does not have to be strictly of the form <code>catchall@myDomain.com</code></li>
    <li>The catch-all address does not have to belong to an account on your domain or even on hMailServer. You can forward messages to external servers.</li>
    <li>If you want hMailServer to reject any messages sent to non-existent addresses in your domain, you should not specify a catch-all address.</li>
</ul>
</div>
</div>
<div class="indented">
<h3>Plus addressing</h3>
<p>Use this option to enable plus addressing for this domain. To avoid confusion and configuration problems, only a limited set of characters are allowed for plus addressing.</p>
</div>
<div class="indented">
<h3>Grey listing</h3>
<p>Use this option to enable and disable grey listing for this domain.</p>
</div>
</div>
</div>

 
