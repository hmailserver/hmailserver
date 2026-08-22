---
id: 220
title: "HOWTO: Enabling SMTP authentication in email client"
slug: howto_enable_smtp_authentication_in_client
parent: howtos
index: 0
is_book: false
updated: 2007-02-19
---

# Overview

This page describes how to enable SMTP authentication in your email client. By default, hMailServer requires SMTP authentication to be enabled in the email client.

## Outlook Express 6

1. Select *Accounts* from the *Tools* menu.
2. Select the *Mail* tab.
3. Double-click on the account you want to modify.
4. Select the *Servers* tab.
5. Under *Outgoing Mail Server*, select *My server requires authentication*.

## Outlook 2003

1. Select *E-mail accounts* from the *Tools* menu.
2. Select *View or change existing e-mail accounts*
3. Double-click on the account you want to modify.
4. Click *More settings*.
5. Select the *Outgoing Server* tab.
6. Select *My outgoing server (SMTP) requires authentication*.
7. Check that *Use same settings as my incoming mail server is selected*.

## PHP/mail()

The PHP mail() function does not support SMTP authentication. Either disable SMTP authentication for the IP range matching the computer where PHP runs, or use another command to send the email.
