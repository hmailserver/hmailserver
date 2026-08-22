---
title: "Choosing database engine"
slug: choosing_database_engine
parent: book_installation
index: 15
is_book: false
---

## Overview

hMailServer supports 4 different database engines

- [Microsoft SQL Server 2000 and later](http://www.microsoft.com/SQL/default.mspx)
- [Microsoft SQL Server Compact Edition (CE)](http://www.microsoft.com/sql/editions/compact/default.mspx)
- [MySQL 4 and later](http://www.mysql.com)
- [PostgreSQL](http://www.postgresql.org/)

## Choosing database

If you are going to be running a commercial hosting service, you should choose one of the following servers:

- Microsoft SQL Server 2000 or later
- MySQL
- PostgreSQL

Microsoft SQL Server Compact Edition **may not** be used with commercial hosting services.

The biggest benefit with SQL Server Compact Edition is the small memory and disk footprint and the fact that it does not require any external software to run on the computer. The database engine runs inside of hMailServer which means that hMailServer has no dependencies on external database engines.  Previous versions of hMailServer (4 and older) included MySQL but this was changed to MSSQL CE in version 5. There are downsides with the default database though:

It may only be used for personal installations; not in commercial environments. The Microsoft SQL Server Compact Edition installation which comes with hMailServer is limited to 4GB . If you expect that your installation will become large (hundreds of thousands of e-mail messages or many accounts) it's recommended that you choose either Microsoft SQL Server or MySQL. A MSSQL CE database of 4GB can hold references to about 10 million email messages. Performance-wise, MSSQL CE is slower than the other supported database engine. Also, there are few tools avaiable if the SQL CE database becomes corrupt, for example due to hardware failure or a system crash.

## Recommendations

Microsoft SQL Server or MySQL is recommended if...

- ... sending or receiving of email is critical to you and you cannot risk any loss of data  

  
- ... you plan to do an integration which involves the hMailServer database. There are more client tools available for Microsoft SQL Server and MySQL compared to PostgreSQL.

PostgreSQL is recommended if you have used it before and feel comfortable with it.

 
