---
id: 218
title: "Ini-file settings"
slug: reference_inifilesettings
parent: book_configuration
index: 0
is_book: false
updated: 2016-12-20
---

# Overview

<p>Most settings in an hMailServer installation is stored in the database. However, some settings are stored in the <span class="Code">hMailServer.ini</span> file. Examples of settings stored in the ini-file are paths and database connection information. This document lists all the available settings in hMailServer.ini.</p>

<p>If you want to use a setting and it's not available in the hMailServer.ini file in your system, you can add the setting yourself. For example, to add the setting <span class="Code">ConnectionAttempts</span> to the <span class="Code">Database</span> section, simply add the line <span class="Code">ConnectionAttempts=5</span> below the line <span class="Code">[Database]</span> in hMailServer.ini. In some cases, you may need to add the actual section ([SectionName]) as well. If the section already exists in the file, you should add the setting to that file. You cannot have two ini file sections with the same name in the same ini-file.</p>

 

## Sections

### Directories

- ProgramFolder - The path to the hMailServer directory. By default, C:\Program Files\hMailServer.
- DataFolder - The path to the hMailServer data directory. By default, C:\Program Files\hMailServer\Data.
- LogFolder - The path where hMailServer logs are stored. By default, C:\Program Files\hMailServer\Logs
- TempFolder - The path where hMailServer stores temporary files, such as attachments during virus scanning. By default C:\Program Files\hMailServer\Temp
- EventFolder - The path where the hMailServer event file is located. By default, C:\Program Files\hMailServer\Events

### GUILanguages

- ValidLanguages - A list of valid hMailServer user interface languages. hMailServer Administrator uses this list to determine which languages to display in the *Language* menu.

### Database

- Internal - 1 if the internal MySQL database is used, 0 otherwise. hMailServer uses this setting to determine whether scripts should be applied to the MySQL database on the first launch. For example, if a new version of MySQL is included with the installation program, hMailServer might run SQL scripts to patch it.
- Type - Type of database. Can be either MySQL or MSSQL. hMailServer uses it to determine what method to use to connect to the database server, and which syntax to use for SQL statements.
- Username - hMailServer will use this username when connecting to the database server. If it's left empty, and MSSQL is used, hMailServer will try to use Windows Authentication.
- Password - The password hMailServer should use when connecting to the database server. If the passwordencryption is set to 1, the password is encrypted using blowfish.
- Passwordencryption - If set to 1, the database password is encrypted using blowfish. In this case, the hMailServer service decodes the password before connecting to the database.
- Port - The port hMailserver should connect to on the database server.
- Server - The database server host name hMailServer should connect to.
- Database - The name of the database hMailServer should try to use.
- NumberOfConnections - The number of connections should open to the database. The default value of this setting is 5, which means that hMailServer will open 5 connections to the database server. hMailServer often wants to execute several database queries at the same time. Since a specific database connection can only be used for one SQL statement at a time, multiple database connections improves performance.
- ConnectionAttempts - The number of times hMailServer should try to connect to the database before giving up on start-up. Default 6 times. (hMailServer 4.4 and later)
- ConnectionAttemptsDelay - The number of seconds hMailServer should pause between each connection attempt during start-up. Default 5 seconds. (hMailServer 4.4 and later)

### Security

- AdministratorPassword - The main hMailServer administration password. The user for example needs to enter this password when starting hMailServer Administrator. This password is encoded using MD5.

### Settings

The settings below should be edited carefully. The exist in the ini file only for database compatibility reasons. They will be moved to the database in an upcoming version. When you install a future version of hMailServer, you may need to change the setting once again, using hMailServer Administrator.

- DNSBLChecksAfterMailFrom - By default, hMailServer runs DNS blacklists checks after SMTP/MAIL FROM. Some users prefer to have it running after the SMTP/RCPT TO command. In this case, set the value of this setting to 0.
- DisableAUTHList (from 5.6.3) -  Setting DisableAUTHList allows you to specify a comma-separated list of SMTP ports which authentication should not be enabled for. This is useful when working with legacy systems with malfunctioning SMTP support.
- AddXAuthUserHeader - If set to 1, hMailSever will add a X-AuthUser header containing a username to messages received using SMTP, if the user has authenticated.
- GreylistingEnabledDuringRecordExpiration - This setting lets you configure hMailServer to temporarily disable grey listing functionality while old grey listing records are cleaned away. This may be required if you have a large amount of greylisting records and are using SQL. When hMailServer deletes old records, the entire greylisting table will become locked for a long time. If other database connections tries to access the table meanwhile, they will have to wait for the deletion to complete. If this takes several minutes, this is likely to cause problems. Default value is 1, which means that hMailServer will continue to use grey listing when deleting records from the database.
- GreylistingRecordExpirationInterval - This setting defines how often hMailserver should delete expired greylisting records from the database. Deleting records may be a time consuming task. The default value is 240, which means that hMailServer will clear expired records every 240 minute (every 4th hour).
- PreferredHashAlgorithm  - This setting allows you to specify which hashing algorithm hMailServer should use for passwords in the hMailServer database. In old versions of hMailServer, passwords were stored in plain text. In hMailServer 4, passwords were stored in MD5. In hMailServer 5, the default preferred hash is now salted SHA256. The following values are valid for this setting:  

     0 - None. Store passwords in clear text. This is not recommended.  

     1 - Blowfish. Store passwords encrypted using Blowfish. This is not recommended, since the password used for encryption is known. Hence, this is no more safe than option 0.  

     2 - MD5. Store passwords in MD5 hash. This is only recommended to preserve backwards compatibility if you have application which integrates with the hMailServer database.  

     3 - SHA256 - Store passwords in SHA256 hashes. This is currently the recommended option which gives the highest level of security.
- RewriteEnvelopeFromWhenForwarding (from version 5.6.2 b 2223) - When performing forwarding, hMailServer now keeps the original From address rather than changing to that of the forwarding account. This change was made to reduce risk of message delivery failures. To force the previous behavior, set RewriteEnvelopeFromWhenForwarding=1
