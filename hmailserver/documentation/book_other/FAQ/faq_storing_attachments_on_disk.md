---
id: 111
title: "Storing only attachments on disk"
slug: faq_storing_attachments_on_disk
parent: FAQ
index: 0
is_book: false
updated: 2005-11-22
---

## Question: Why not store only attachments on disk?

The question seems to imply that the "body" and the "attachments" are separate entities, which they are not. An email message is a contiguous stream of bytes, which includes headers, body, attachments and all. When hMailServer sends or receives data, it does so as MIME-encoded data, not as binary data; this includes the attachments. Each message is delivered to the email client exactly as it was received; it's then up to the client what to do with it.

Presently, what hMailServer does is to get a stream of data from the network, store it as a file precisely as it was received, and deliver it to the email client when requested. Eventually, if a virus scanner is present, it will have to MIME-decode the whole message and scan it for viruses.

However, per your suggestion, every message would have to be MIME-decoded at the server, the attachments would be stripped from the message and stored as binary data, and then the rest of the email message would be stored in the database. Attachments needed to be plain binary data (not MIME-encoded using base64 or any other algorithm); otherwise they couldn't be scanned for viruses.

This scenario would lead to at least two problems:

*Problem 1:* When hMailServer accepts a message, it should then decode it from Base64/Quoted-Printable and store every attachment to disk, actually having to MIME-decode the entire message before it could save it. This not only raises the memory requirements, but also consumes a lot of CPU.

*Problem 2:* When a POP3 client requests that message, the server would have to MIME-encode all of the attachments and put them back into one single stream before sending them. This would, again, increase memory usage and CPU usage.

So, having hMailServer extract the attachments would serve no useful purpose, because it would actually involve more work, not less. That's why extracting the attachments from the message is traditionally a role for the email client, not for the server.
