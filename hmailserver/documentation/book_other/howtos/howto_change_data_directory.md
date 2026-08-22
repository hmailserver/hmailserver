---
id: 25
title: "Change the data directory"
slug: howto_change_data_directory
parent: howtos
index: 20
is_book: false
updated: 2015-07-08
---

## Background

By default, all emails are stored in the hMailServer data directory, under C:\Program Files\hMailServer\Data. In some cases, you might want to change this to a different path. For example, if you have more disk space on another drive. 

## Steps

1. Stop the hMailServer service
2. Open up hMailServer.ini
3. Locate *DataFolder* under the *Directories*-section
4. Specify the new path
5. Move all the message files from the old to the new path

 

NOTE:- This will only work if the diagnostic tests show that 'Relative message paths are stored in the database for all messages' - (second last test). If this is not the case then head to the forum and ask how this can achieved.
