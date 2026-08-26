---
title: "IMAP information"
slug: information_imap
parent: book_other
index: 0
is_book: false
---

IMAP stands for Internet Message Access Protocol. It is a protocol that an email client can use to download email from an email server. IMAP includes many more features than POP3. The IMAP protocol is designed to let users keep their email on the server. The IMAP protocol requires more disk space and CPU resources on the server than the POP3 protocol, since all email messages remain stored on the server after the email client downloads them. IMAP normally uses port 143.

## IMAP folders

By default, it is the IMAP client itself (webmail client, Outlook Express, Netscape Mail and so on) that decides which folder it uses for sent items, drafts, deleted items and junk, and what that folder is named. hMailServer does not try to guess this from the folder name - the client asks the server to create a folder, and then puts messages in it. If you want to rename such a folder, do so in your IMAP client, not in hMailServer.

hMailServer does support the IMAP SPECIAL-USE extension ([RFC 6154](https://tools.ietf.org/html/rfc6154)), so a client can tell the server what a folder is for when it creates it (`CREATE ... USE (\Sent)` and similar), and hMailServer will remember this and report it back to clients that ask for it (`LIST (SPECIAL-USE)`). You can also enable *Create default special-use folders* under Settings -> Protocols -> IMAP -> Advanced (disabled by default) to have hMailServer create the well-known Drafts, Sent, Trash and Junk folders itself, already tagged with the matching SPECIAL-USE attribute, for every new account. This only affects accounts created after the setting is enabled; the folder names are fixed and cannot be renamed in hMailServer.

## Refreshing IMAP folder list

The IMAP protocol doesn't provide any mechanism for automatic refresh of IMAP folders. If you have several IMAP clients connected to the same account at the same time, and one of the clients creates a new IMAP folder, the other clients will not automatically be notified about this folder.

## Refreshing message lists

The IMAP protocol has an extension called IMAP IDLE. The IMAP IDLE extension lets the client "Subscribe" to an IMAP folder and receive notifications from the server when new messages exist and when state of messages change. This way the client does not need to check for new messages itself. hMailServer 4.3 and later support this extension. 

## Sorting of message lists

hMailServer supports the IMAP SORT extension. This extension can dramatically improve performance when working with large IMAP folders. Read [more here](?page=details_imap_sort).

## Folder separators

hMailServer uses . (dot) as folder separator. This means that you cannot have dot in an IMAP folder name.
