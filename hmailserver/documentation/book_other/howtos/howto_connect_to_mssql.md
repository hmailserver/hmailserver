---
title: "Connect to MSSQL"
slug: howto_connect_to_mssql
parent: howtos
index: 0
is_book: false
---

This page describes how to connect to the hMailServer MSSQL database to execute statements.

## Step 1: Determine MSSQL password

If you are using an external version of MSSQL, you should already know your MSSQL password. If you are using the built-in MSSQL version that comes with hMailServer, you might not know your password. To determine it, follow these steps:

<ol>
    <li>Open up hMailserver.ini, located in your Windows directory.</li>
    <li>If you can't find <i>passwordencryption=1</i> in the file, locate the password in the file and go to <i>Step 2: Connect to the server</i> below.</li>
    <li>Run the script <span class="Code">\hMailServer\Addons\Utilities\DecryptBlowfish.vbs</span>.</li>
    <li>Copy the password from hMailServer.ini to the DecryptBlowfish dialog.</li>
    <li>Click OK in DecryptBlowfish to view your password. DecryptBlowfish will now show your password. Either keep the dialog open or memorize the password.</li>
</ol>

## Step 2: Install required software

To be able to connect to the Microsoft SQL Server Compact database, you need Microsoft SQL Server 2008 Studio Express. The following software needs to be installed for Microsoft SQL Server 2008 Studio Express.

- [Windows Installer 4.5 or later](http://www.microsoft.com/downloads/details.aspx?familyid=5A58B56F-60B6-4412-95B9-54D056D6F9F4&displaylang=en)
- [.NET Framework 3.5 Service pack 1](http://www.microsoft.com/downloads/details.aspx?FamilyID=AB99342F-5D1A-413D-8319-81DA479AB0D7&displaylang=en)
- [Windows Powershell](http://www.microsoft.com/windowsserver2003/technologies/management/powershell/download.mspx)

After these tools have been installed, you can install [Microsoft SQL Server 2008 Studio Express](http://www.microsoft.com/downloads/details.aspx?familyid=08e52ac2-1d62-45f6-9a4a-4b76a8564a2b&displaylang=en). During the installation, make sure you select to install the Management Tools.

## Step 3: Connect to the server

1. Start SQL Server Management Studio found in the start menu.
2. Select File -> Connect Object Explorer
3. Select SQL Server Compact Edition from the Server type list.
4. Select the hMailServer database file, typically C:\Program Files\hMailServer\Database\hMailServer.sdf
5. Type in the password you retrieved in step 1
6. Click Connect to connect to the database server

Now you have connected to MSSQL

## Step 3: Executing statements

After you have connected to MSSQL, you can execute statements by following these steps:

1. In the tree-menu to the left, right click on the hmailserver database.
2. Select New Query from the context menu.

 
