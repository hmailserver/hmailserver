---
title: "Frequently Asked Questions"
slug: FAQ
parent: book_other
index: 0
is_book: false
---

### Question 1 - What is hMailServer?

hMailServer is an e-mail server for Microsoft Windows. It allows you to handle all your email yourself without having to rely on an Internet service provider (ISP) to manage it. Compared to letting your ISP host your email, hMailServer adds flexibility and security and gives you the full control over spam protection.

### Question 2 - Who are using hMailServer?

hMailServer is used by a lot of different types of users. Here's some examples:

- The most common type is companies who use hMailServer to host email for their own domains.
- hMailServer is also used by small and large Internet service provider who use hMailServer to host email for their customers.
- Different types of governments spread across the world use hMailServer for hosting their email.
- A lot of individuals and families are running hMailServer at home to reduce the amount of spam which reaches their inboxes.

### Question 3 - When should I use hMailServer?

Anyone with an email address may benefit from using hMailServer. Typically, organizations and persons who have registered a domain name on the Internet use hMailServer to host email for this domain.

### Question 4 - Do I need to be connected to the Internet to run hMailServer?

No, you can run hMailServer without being connected to the Internet, with the obvious limitation that you cannot send email to other people on the Internet.

### Question 5 - How many domains, accounts and messages can hMailServer handle?

That depends mostly on the hardware of the computer where hMailServer runs and what type of users you have. For example, if your users are very active, you need a faster server compared to if your users does not check their email often. There are hMailServer installations with tens of thousands of users accounts on a single server.

### Question 6 - How do I get started?

The first thing you need to do is to determine what you want to use hMailServer for. After that, [follow the installation tutorial](?page=howto_install).

### Question 7 - I have installed hMailServer, what do I do next?

That depends on how you plan to use hMailServer. If you want to host email for your own domain, you should add a domain and user accounts to hMailServer. The steps to do this are described on this page: [Basic configuration](?page=basic_configuration)

### Question 8 - I get the message "SMTP authentication is required" when I try to send a message. What should I do?

This is a common error message which in most cases is caused by an incorrectly configured email client. Please check the following pages for information about error messages: [Error messages](?page=reference_error_messages)

### Question 9 - Do I have to set up host names pop.example.com, smtp.example.com and so on?

It is unnecessary from a technical point of view to have multiple host names resolving to the same IP address. As long as the host name you are using is pointing at the correct IP address, it does not matter what the name of the host name is.

### Some more technical questions:

- [Why aren't entire e-mail messages stored in the database?](?page=faq_storing_message_in_database)
- [Why not store only attachments on disk?](?page=faq_storing_attachments_on_disk)
- [Why are there empty directories in the data directory?](?page=faq_empty_dirs_in_datadir)
