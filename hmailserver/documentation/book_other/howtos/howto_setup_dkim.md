---
title: "Setting up DKIM"
slug: howto_setup_dkim
parent: howtos
index: 0
is_book: false
---

<div>This page describes how to set up DKIM with hMailServer.</div>

<div> </div>

1. Download and install OpenSSL, for example from:  

 https://slproweb.com/products/Win32OpenSSL.html
2. Open up a command prompt and CD to C:\OpenSSL-Win32\Bin
3. Run the command openssl genrsa -out dkim-private.pem 1024 -outform PEM  

 This will generate the private key. This is to be kept secret.
4. Run the command openssl rsa -in dkim-private.pem -out dkim-public.pem -pubout -outform PEM  

 This will generate a public key.  This can be shared with others.
5. Copy the file dkim-private.pem to your mail server, for example C:\Keys\dkim-private.pem
6. In your DNS settings, add a new TXT record with the name dkim._domainkey and value "k=RSA; p=<content-from-dkim-public.pem>". Remove any whitelines. Example:  

   

 k=rsa;p=MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCoEELiGTItqXPA8YzGd3/UX8NKYlTm9KDYy89KPofpqdlWRvvFDSzQdJiV0toYLzKA5P4x0fdARpXU54S6UNWngVSO3AVvSc1/gG8yfITPMcTyUmxf16GFTzKl2h5/K8rXrZd7SKeNaYEHcdFzGH5aTg3OJQiJMDT01r6xOlo3DQIDAQA 
7. In the domain settings in hMailServer Administrator, choose to enable DKIM signing. Select the private key file (for example C:\Keys\dkim-private.pem), and enter the selector *dkim*.
8. Choose signing algorithm SHA256.

<div>After setting everything up, it's recommended that you use a DKIM validator to set your set up, for example <a href="http://dkimvalidator.com/">dkimvalidator.com</a>.</div>

<div> </div>

<div> </div>

 
