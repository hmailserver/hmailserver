---
id: 80
title: "Reset the MySQL password"
slug: howto_reset_mysql_password
parent: howtos
index: 0
is_book: false
updated: 2007-04-11
---

## Background

When you install hMailServer, you can select to use the internal MySQL database. If you do this, the installation program will create a MySQL database which hMailServer will use. The installation program will also create a root user in the MySQL server which will receive a randomly generated password. This password is then written to hMailServer.ini in encrypted forum. If you have access to hMailServer.ini, you can decrypt the password using hMailServer\Addons\Utilities\DecryptBlowfish.vbs.
