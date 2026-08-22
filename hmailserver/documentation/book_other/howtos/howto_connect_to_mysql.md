---
title: "Connect to MySQL"
slug: howto_connect_to_mysql
parent: howtos
index: 0
is_book: false
---

This page describes how to connect to the MySQL database to execute statements.

## Step 1: Determine MySQL password

If you are using an external version of MySQL, you should already know your MySQL password. If you are using the built-in MySQL version that comes with hMailServer, you might not know your password. To determine it, follow these steps:

1. Open up hMailserver.ini, located in your Windows directory.
2. If you can't find *passwordencryption=1* in the file, locate the password in the file and go to *Step 2: Connect to the server* below.
3. Run the script \hMailServer\Addons\Utilities\DecryptBlowfish.vbs.
4. Copy the password from hMailServer.ini to the DecryptBlowfish dialog.
5. Click OK in DecryptBlowfish to view your password. DecryptBlowfish will now show your password. Either keep the dialog open or memorize the password.

## Step 2: Connect to the server

To connect to MySQL, you need a MySQL client. You can use [MySQL Front](http://www.hmailserver.com/download/MySQLFront.zip) for this.

1. Download and unzip MySQLFront
2. Run MySqlFront.exe
3. In the connection dialog, click New
4. Enter the connection information for your MySQL server. If you are using the built-in server, you should enter hostname: localhost, user:root, password:the password, port:3307
5. Click Connect to connect to the database server

Now you have connected to MySQL.

## Step 3: Executing statements

After you have connected to MySQL, you can execute statements by following these steps:

1. In the tree-menu to the left, click on the hmailserver database. To the right, you now see a list over tables in the database.
2. Over the list of tables, there is a "tab" with the title *Query*. Click on this tab.
3. Enter your SQL-query
4. Click on the *Execute SQL* button in the keyboard, or press F9 on the keyboard to run the query
