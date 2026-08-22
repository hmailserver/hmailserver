---
title: "Why are there empty directories in the data directory?"
slug: faq_empty_dirs_in_datadir
parent: FAQ
index: 0
is_book: false
---

## Question: Why are there empty directories in the data directory?

hMailServer store email messages in the data directory. The message files are stored in sub-directories named after the domain and user account. 

In early hMailServer versions, all message files were stored directly in the user account folder (for example `hMailServer/Data/domain.com/user1/`. When there was 50 000 messages in a users account, there was 50 000 messages in the users data folder. Since Windows doesn't handle huge folders efficiently, these numbers of message files had a negative effect on hMailServer performance. 

To improve performance, hMailServer now stores the message files in sub directories consisting of 2 characters from 0-9 and A-F. These sub directories are created dynamically when needed, but they aren't deleted until the account is deleted. Because of this, there may be several empty sub directories. Since the directory name only consists of 2 characters, there will be at most 256 sub directories.
