---
title: "IMAP information"
slug: information_imap
parent: book_other
index: 0
is_book: false
---

IMAP stands for Internet Message Access Protocol. It is a protocol that an email client can use to download email from an email server. IMAP includes many more features than POP3. The IMAP protocol is designed to let users keep their email on the server. The IMAP protocol requires more disk space and CPU resources on the server than the POP3 protocol, since all email messages remain stored on the server after the email client downloads them. IMAP normally uses port 143.

## IMAP folders

By default, it is the IMAP client itself (Outlook, Thunderbird, Apple Mail, a webmail client and so on) that decides which folder it uses for sent items, drafts, deleted items and junk, and what that folder is named. hMailServer does not try to guess this from the folder name - the client creates the folder it wants to use, and then puts messages in it. If you want to rename such a folder, do so in your IMAP client, not in hMailServer.

Many modern email clients can also tell hMailServer what a folder is for when they create it, using a standard feature called special-use folders. hMailServer remembers this and passes it on to other clients, so they can automatically recognize, for example, which folder holds sent mail. hMailServer can also create the well-known Drafts, Sent, Trash and Junk folders itself, already marked as special-use, for every new account - this is controlled by *Create default special-use folders* under Settings -> Protocols -> IMAP -> Advanced, and is enabled by default. This only affects accounts created after the setting is enabled; the folder names are fixed and cannot be renamed in hMailServer.

## Refreshing IMAP folder list

The IMAP protocol doesn't provide any mechanism for automatic refresh of IMAP folders. If you have several IMAP clients connected to the same account at the same time, and one of the clients creates a new IMAP folder, the other clients will not automatically be notified about this folder.

## Refreshing message lists

The IMAP protocol has an extension called IMAP IDLE. The IMAP IDLE extension lets the client "Subscribe" to an IMAP folder and receive notifications from the server when new messages exist and when state of messages change. This way the client does not need to check for new messages itself. hMailServer 4.3 and later support this extension. 

## Sorting of message lists

hMailServer supports the IMAP SORT extension. This extension can dramatically improve performance when working with large IMAP folders. Read [more here](?page=details_imap_sort).

## Folder separators

By default, hMailServer uses . (dot) as the folder separator. This can be changed to a different character under Settings -> Protocols -> IMAP -> Advanced. Whichever character is selected as the separator cannot also be used in an IMAP folder name.
