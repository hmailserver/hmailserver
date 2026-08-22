---
id: 92
title: "IMAP information"
slug: information_imap
parent: book_other
index: 0
is_book: false
updated: 2009-07-03
---

IMAP stands for Internet Message Access Protocol. It is a protocol that an email client can use to download email from an email server. IMAP includes many more features than POP3. The IMAP protocol is designed to let users keep their email on the server. The IMAP protocol requires more disk space and CPU resources on the server than the POP3 protocol, since all email messages remain stored on the server after the email client downloads them. IMAP normally uses port 143.

## IMAP folders

It is the IMAP client itself (webmail client, Outlook Express, Netscape Mail and so on) that is responsible for the properties of the components that make up the IMAP client. For example, hMailServer does not decide what name the "Sent items" folder should have. Nor does it know that the "Sent items" folder actually contains sent items. The IMAP client asks the server to create a folder for sent items, and then puts messages in the folder. If you want to change the name of that folder, you should do so in your IMAP client, not in hMailServer.

## Refreshing IMAP folder list

The IMAP protocol doesn't provide any mechanism for automatic refresh of IMAP folders. If you have several IMAP clients connected to the same account at the same time, and one of the clients creates a new IMAP folder, the other clients will not automatically be notified about this folder.

## Refreshing message lists

The IMAP protocol has an extension called IMAP IDLE. The IMAP IDLE extension lets the client "Subscribe" to an IMAP folder and receive notifications from the server when new messages exist and when state of messages change. This way the client does not need to check for new messages itself. hMailServer 4.3 and later support this extension. 

## Sorting of message lists

hMailServer supports the IMAP SORT extension. This extension can dramatically improve performance when working with large IMAP folders. Read [more here](?page=details_imap_sort).

## Folder separators

hMailServer uses . (dot) as folder separator. This means that you cannot have dot in an IMAP folder name.
