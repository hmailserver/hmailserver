//=============================================================================
// Microsoft SQL Server Compact (Version 4.0)
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft
// premium shared source license agreement under which you licensed
// this source code. If you did not accept the terms of the license
// agreement, you are not authorized to use this source code.
// For the terms of the license, please see the license agreement
// signed by you and Microsoft.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
// Component: Common files
// 
// File:  sqlce_err.h
//
// File Comments:   Native Engine Error Codes and Messages.
//=============================================================================

#ifndef __SQLCE_ERR_H__
#define __SQLCE_ERR_H__


//  HRESULTS
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code    
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//-----------------------------------------------------------------------------

// ****************************************************************************
// NOTE: THIS FILE IS PARSED BY SQLDRIVER - PLEASE DO THE FOLLOWING:
// ****************************************************************************
//
// 1. DO NOT put an error (or description) on more than one line
// 2. DO NOT include " in the description
// 3. Make sure the comment is at the end of a line and has a space 
//    before the text ("// ")
// 4. All Errors should document error parameters after the error string 
//    in this format [#,#,#,s,s,s]
//    [,,,,,] == no error parameters for this error. 
// 5. DO NOT REMOVE // DEAD error codes; Everytime you add a new error, make 
//    sure the number hasn't been used in the past; otherwise some user apps 
//    could break;
// 6. DO NOT exceed the maximum error message limit (CCH_MAX_ERRORMESSAGE)
//
// ****************************************************************************


//
// Maximum length of error message
//
#define CCH_MAX_ERRORMESSAGE 4096

//
// Note : IF you are adding the error message which will show the database path 
// in it please make sure that the path is sttriped out in ADO.Net layer as it 
// a potential security risk for Medium-Trust scenarious. The SqlCeError class 
// in ADO.Net handles the task of strriping the path and only showing the
// filename in the error message.
//


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//                              M I N O R   C O D E S
//
//  These are codes paired with a major code of E_FAIL for OLE/DB exposure.
//
//  SQL CE Range         (25000-30000)
//  OLEDB HRESULT Range  (3584-3805) i.e. (0x0E00-0x0EDD)
//
// ****************************************************************************
// NOTE : Our error number range is limited to the SQL Server error number 
//        range, Which is 25000 to 30000. DO NOT ADD ANY ERRORS ABOVE OR
//        BELOW THIS RANGE! 
//        Even though OLEDB HRESULT is of type LONG it is stored as USHORT 
//        in the error resource DLL string table. So this range (3584-3805) 
//        is also reserved!
// ****************************************************************************
//
//  SE Minor Range:             25000 -- 25499
//
//  QP Minor Range:             25500 -- 26999
//
//      25500 -- 25699  :   Compile errors
//      25700 -- 25899  :   Execute errors
//      25900 -- 26099  :   Expr errors
//      26100 -- 26299  :   Storage QP errors
//      26300 -- 26499  :   Core QP errors
//      26500 -- 26999  :   Reserved
//
//  OLEDB Minor Range:          27000 -- 27499
//
//  OLEDB HRESULT Range:        3584  -- 3805
//
//  Common Minor Range:         27500 -- 27749
//
//  Managed Code Minor Range:   27750 -- 27999
//
//  Connectivity Minor Range    28000 -- 30499
//
//      28000 -- 28249  :   Transport   
//      28250 -- 28499  :   Replication provider
//      28500 -- 28999  :   Client Agent
//      29000 -- 29499  :   Server Agent
//      29500 -- 29999  :   Message Protocol
//      30000 -- 30499  :   SQLCE Relay (obsolete component as of now)
//      30500 -- 39499  :   Unused
//      39500 -- 39999  :   Miscellaneous (setup, etc.) 
//
//
// ----------------------------------------------------------------------------
//
// Storage Engine   25000 - 25499
//
#define SSCE_M_NOCURRENTRECORD                  25001           // Either the cursor is not on a row or there are no rows left. [,,,,,]
#define SSCE_M_INVALIDBUFFERSIZE                25002           // The specified buffer size is not valid. [Buffer size specified,,,,,]
#define SSCE_M_COLUMNLENGTHTOOBIG               25003           // The specified length is too long for the column type. [Specified length,Column data type,,,,]
#define SSCE_M_COLUMNNOTUPDATEABLE              25004           // The column cannot be modified. [,,,Column name,,]
#define SSCE_M_NULLINVALID                      25005           // The column cannot contain null values. [,,,Column name,Table name,]
// DEAD SSCE_M_INVALIDOPERATION                 25006           // The operation cannot be performed on the object. [,,,,,]
// DEAD SSCE_M_INTRANSACTION                    25007           // The operation cannot be performed in a transaction. [,,,,,]
// DEAD SSCE_M_INVALIDPARAMETER                 25008           // The parameter is not valid. [,,,,,]
#define SSCE_M_INVALIDPATH                      25009           // The path is not valid. Check the directory for the database. [,,,Path,,]
#define SSCE_M_INVALIDFILENAME                  25010           // The file name is not valid. Check the file name for the database. [,,,File name,,]
#define SSCE_M_INVALIDDATABASE                  25011           // The file that is being referenced is not a SQL Server Compact database file format. [,,,Databasename,,]
#define SSCE_M_UPDATENOTPREPARED                25012           // The record was not prepared for update. [,,,,,]
// DEAD SSCE_M_TABLENOTLOCKED                   25013           // The operation is supported only on tables with exclusive locks. [,,,Table name,,]
#define SSCE_M_SYSCOLACCESSDENIED               25014           // Access to the system column is denied. [,,,,,]
// DEAD SSCE_M_CANTALLOCATEPAGE                 25015           // A page cannot be allocated. There might not be enough memory on the device. [,,,,,]
#define SSCE_M_KEYDUPLICATE                     25016           // A duplicate value cannot be inserted into a unique index. [,,,Table name,Constraint name,]
#define SSCE_M_DATABASECORRUPTED                25017           // The database file may be corrupted. Run the repair utility to check the database file. [,,,Database name,,]
#define SSCE_M_WRITECONFLICT                    25018           // Failed to obtain a lock for a write operation. [,,,,,]
// DEAD SSCE_M_READCONFLICT                     25019           // Failed to obtain a lock for a read operation. [,,,,,]
#define SSCE_M_COMMITCONFLICT                   25020           // Failed to obtain a lock for a commit. [,,,,,]
// DEAD SSCE_M_SESSIONWRITECONFLICT             25021           // Failed to obtain lock for a read or write operation. [,,,,,]
#define SSCE_M_NULLKEYDISALLOWED                25022           // Null values are not allowed in indexes. [,,,,,]
#define SSCE_M_INTEGRITYVIOLATION_MSTR          25025           // The primary key value cannot be deleted because references to this key still exist. [,,,Foreign key constraint name,,]
#define SSCE_M_INTEGRITYVIOLATION_SLV           25026           // A foreign key value cannot be inserted because a corresponding primary key value does not exist. [,,,Foreign key constraint name,,]
#define SSCE_M_RECORDTOOBIG                     25027           // The table definition or the row size exceeds the maximum row size of 8060 bytes. [,,,,,]
#define SSCE_M_INVALIDPASSWORD                  25028           // The specified password does not match the database password. [,,,Data Source,,]
// DEAD SSCE_M_RI_UPDATETWICE                   25029           // An attempt was made to update a field in a record twice. [,,,,,]
#define SSCE_M_RI_DUPLICATEKEY                  25030           // A duplicate key value was found as a result of referential integrity action. [,,,Foreign table name,,]
// DEAD SSCE_M_RI_KEYNULLDISALLOWED             25031           // A referential cascade action is setting a nonnull column to NULL. [,,,Foreign table name,,]
// DEAD SSCE_M_RI_SESSIONWRITECONFLICT          25032           // A write conflict occurred because a referential integrity action is not allowed. [,,,Foreign table name,,]
#define SSCE_M_ILLEGALCOLLATINGSEQ              25033           // The specified locale is not supported on this operating system. [LCID,,,,,]
#define SSCE_M_FILELOCKVIOLATION                25034           // There is a file locking violation. Close any external application that uses the database file. [,,,,,]
#define SSCE_M_FILESHAREVIOLATION               25035           // There is a file sharing violation. A different process might be using the file. [,,,,,]
// DEAD SSCE_M_FILELOCKINGUNAVAILABLE           25036           // File locking is not available. [,,,,,]
#define SSCE_M_DISKFULL                         25037           // There is not enough disk space left for the database. [,,,,,]
#define SSCE_M_TEMPDISKFULL                     25038           // There is not enough disk space left for the temporary database. [,,,,,]
#define SSCE_M_FILEACCESSDENIED                 25039           // Access to the database file is not allowed. [,,,File name,,]
// DEAD SSCE_M_INVALIDSORTDLL                   25042           // The language required for this database file is not fully installed on this operating system. [,,,,,]
#define SSCE_M_TOOMANYOPENFILES                 25044           // OS Error: The OS limit of open files has been exceeded. Other applications will need to be shut down to free up available open files. [,,,,,]
// DEAD SSCE_M_OBSOLETEDATABASEFORMAT           25045           // The database file format is obsolete. [,,,,,]
#define SSCE_M_FILENOTFOUND                     25046           // The database file cannot be found. Check the path to the database. [,,,Data Source,,]
#define SSCE_M_READVERIFYERROR                  25047           // OS Error: SQL Server Compact  encountered a premature EOF for the database file. Running the repair utility might help recover some data. [,,,,,]
#define SSCE_M_NOMOREFILEHANDLES                25048           // OS Error: The OS is reporting that no more file handles can be created. In order to proceed, try closing other applications. [,,,,,]
#define SSCE_M_DISKNOTREADY                     25049           // OS Error: The OS storage system (RAM, CF, SD, or IPSM) is not responding. Retry the operation. [,,,,,]
#define SSCE_M_SHARINGBUFFEREXCEEDED            25050           // OS Error: The OS will not allow sharing of any more files. In order to proceed, try closing other applications. [,,,,,]
#define SSCE_M_DISKIO                           25051           // Internal error: Unable to successfully execute disk IO on the file system. [,,,,,]
// DEAD SSCE_M_THREADSNOTSUPPORTED              25052           // The operating system does not support threads. [,,,,,]
#define SSCE_M_KEYNOTMADE                       25053           // Internal error: A serious error has occurred when trying to create a key. [,,,,,]
#define SSCE_M_TOOMANYKEYS                      25054           // The limit of 16 columns for the multi-column index was exceeded. [,,,,,]
#define SSCE_M_COLUMNWIDTHNOTEVEN               25056           // The size of the Unicode text column is not even. [Column size,,,Column name,,]
#define SSCE_M_NOCURRENTINDEX                   25058           // Internal error: Invalid operation without a current index. [,,,,,]
#define SSCE_M_ILLEGALINDEXCOLUMN               25059           // Long value data type cannot be indexed. [,,,,,]
#define SSCE_M_REFERENCENOTFOUND                25060           // The foreign key constraint does not exist. [,,,,,]
// DEAD SSCE_M_OBJECTNOTREPLICABLE              25062           // The object is not replicable. [,,,Object name,,]
#define SSCE_M_ONLYONESPECIALCOLUMN             25064           // Another column of same special column type found.  ROWGUID, IDENTITY, rowversion are special column types.  For each special column type, there can be only column of that type. [,,,Object name,,]
#define SSCE_M_TOOMANYINDEXES                   25065           // The limit of 249 indexes per tables has been exceeded. [,,,Table name,,]
#define SSCE_M_TOOMANYCOLUMNS                   25066           // The limit of 1024 columns for a table has been exceeded. [Column count,,,Column name,,]
#define SSCE_M_TRANSTOODEEP                     25067           // Internal error: The internal transactions for SQL Server Compact have exceeded the nesting limit. [,,,,,]
// DEAD SSCE_M_SELFREFERENCE                    25068           // A column in an index cannot reference itself. [,,,,,]
// DEAD SSCE_M_NOPRIMARYINDEX                   25069           // The referenced table must have a primary index. [,,,,,]
#define SSCE_M_DIFFNUMRELCOLUMNS                25070           // The definition of referring columns (such as number of columns or data types) in referential relationships must match the referred columns. [,,, Constraint name (if known),,]
// DEAD SSCE_M_CONFLICTINGREFERENCE             25071           // A reference to this column already exists. [,,,Index name,,]
#define SSCE_M_PRIMARYKEYNULLCONFLICT           25072           // Primary keys cannot be created on columns that support null values. [,,,Column name,,]
#define SSCE_M_TABLEMUSTHAVECOLUMNS             25073           // Tables must contain at least one column. [,,,,,]
#define SSCE_M_DEFAULTEXPRTOOLONG               25074           // The expression string for the default value cannot be longer than 4000 characters. [Length of expression string,,,,,]
#define SSCE_M_DISALLOWIDENTITYNULL             25075           // Nullable columns cannot be identity columns. [,,,,,]
#define SSCE_M_CONSTRAINTINUSE                  25077           // The constraint cannot be removed because it is referenced by another constraint. [,,,Constraint name,,]
#define SSCE_M_INDEXINUSE                       25078           // The index cannot be removed because it is being used to enforce a constraint. [,,,Index name,,]
#define SSCE_M_ENCRYPTEDDBMUSTHAVEPWD           25079           // A password must be specified when a database is created by using encryption. [,,,,,]
// Changed for v3.5 SSCE_M_NOOSENCRYPTIONSUPPORT            25080           // The operating system does not support encryption. [,,,,,]
#define SSCE_M_NOOSENCRYPTIONSUPPORT            25080           // The operating system does not support the Encryption Mode provided. [,,,,,]
#define SSCE_M_VIOLATEDAUTOINCRANGE             25081           // The new value generated for the inserted row is outside the range for an INTEGER or BIGINT data type. [Generated value,,,,,]
// DEAD SSCE_M_UNIQUENULLCONFLICT               25082           // Unique indexes and constraints cannot be created on columns that allow null values. [,,,,,]
#define SSCE_M_CYCLEDETECTED                    25083           // The referential relationship will result in a cyclical reference that is not allowed. [,,,Constraint name,,]
#define SSCE_M_RELCOLUMNSTYPEMISMATCH           25084           // The data types of the columns in the relationship do not match. [,,,,,]
#define SSCE_M_PKDUPLICATE                      25085           // The table already has a primary key. [,,,,,]
#define SSCE_M_INVALIDINCREMENT                 25086           // The identity column contains an increment value that is not valid. [,,,Column name,,]

//***** Don't add any more errors here, read the comments below and add appropriately
//
// Errors after this comment were added for v1.1 (PEBBLE) 
//
#define SSCE_M_GENERATEGUIDFAILED               25087           // Internal error: A new GUID cannot be generated for the new row. [,,,,,]

//
// Errors defined for v2.0
//
// DEAD SSCE_M_KEYTOOLARGE                      25088           // The key size exceeded the maximum allowed. [,,,,,]

//
// Errors introduced for SQL-Lite
//
#define SSCE_M_SYNCINPROGRESS                   25089           // Another synchronization is in progress. Try synchronizing later. [,,,,,]
#define SSCE_M_LOCKTIMEOUT                      25090           // SQL Server Compact timed out waiting for a lock. The default lock time is 2000ms for devices and 5000ms for desktops. The default lock timeout can be increased in the connection string using the ssce: default lock timeout property. [Session id,Thread id,Process id,Table name,Conflict type,Resource]
#define SSCE_M_OUTOFLOCKS                       25091           // The lock manager has run out of space for additional locks. This can be caused by large transactions, by large sort operations, or by operations where SQL Server Compact creates temporary tables. You cannot increase the lock space. [,,,,,]
#define SSCE_M_INVALIDTEMPPATH                  25092           // The path specified for the temp database is not valid. [,,,,,]
#define SSCE_M_NOQUERYPROCESSOR                 25100           // Unable to create Command object because Query Processor module is not present on the device or not located in the same directory as the Storage Engine module. [,,,,,]
#define SSCE_M_OUTOFBUFFERPOOLSPACE             25101           // SQL Server Compact has exceeded the buffer size. The default size can be increased on initialization by modifying the ssce: max buffer size property. [The default size,,,,,]
#define SSCE_M_INITPROPCONFLICT                 25102           // Another user has opened the database with different instance-level initialization properties. [,,,,,]
#define SSCE_M_CANTLOADSQLSEDLL                 25103           // Storage Engine module could not be loaded. Reinstall SQL Server Compact. [,,,,,]
#define SSCE_M_DATABASETOOBIG                   25104           // The database file is larger than the configured maximum database size. This setting takes effect on the first concurrent database connection only. [Required Max Database Size (in MB; 0 if unknown),,,,,]
#define SSCE_M_INVALIDSECURITYDESCRIPTOR        25105           // Internal error: Invalid Security Descriptor was specified. [,,,,,]
#define SSCE_M_TOOMANYCONSTRAINTS               25106           // The limit of 249 contstraints per table has been exceeded. [,,,TableName,,]
#define SSCE_M_BADKEYTYPE                       25107           // Internal error: The specified constraint does not define the requested type of key. [,,,,,]
#define SSCE_M_OUTSTANDINGPREPARE               25108           // Another cursor on the same table has a pending operation on the same row. [,,,,,]

// Errors after this comment were added for v3.0 (Laguna)
//
#define SSCE_M_PASSWORDTOOLONG                  25109           // The database password cannot exceed 40 characters. [,,,,,]
#define SSCE_M_SYSTEMNAME                       25110           // The table and column name is part of a reserved name. Table and column names cannot begin with '__Sys'. [,,,Name,,]
#define SSCE_M_COMPRESSEDNOSTREAM               25111           // Internal error: Compressed columns do not support the stream interface(s). [,,,Column Name,,]
#define SSCE_M_COMPRESSEDNONLV                  25112           // Invalid use of compressed flag for non-long value column. [,,,Column Name, Table Name,]
#define SSCE_M_CHANGECOMPRESSED                 25113           // Cannot change compression status for a database. [,,,Db name,,]
#define SSCE_M_FILEALREADYEXISTS                25114           // File already exists. Try using a different database name. [,,,File name,,]
#define SSCE_M_WRONGPAGE                        25115           // Wrong page (potential database corruption). Try running the repair utility. [,,,,,]
#define SSCE_M_SYSTABACCESSDENIED               25116           // Internal error: Access to the system table is denied. [,,,,,]
#define SSCE_M_ALREADYOPENED                    25117           // Database already opened by a different user. [,,,Db name,,]
#define SSCE_M_CANTOPENSHAREDMEMORY             25118           // Internal error: Cannot open the shared memory region. [,,,,,]
#define SSCE_M_CANTCHANGEKEYCOLUMN              25119           // Cannot alter a column that is part of a key or an index. [,,,Index Name,Column,]
#define SSCE_M_RODATABASEREQUIRESTEMPPATH       25120           // Opening a database as read-only requires a temp path to be specified. [,,,Db name,,]
// DEAD SSCE_M_OUTSTANDINGLOCKS                 25121           // A generation cannot be bumped due to open transactions with changes pending. [,,,,,]
#define SSCE_M_FATALLOCKFAILURE                 25122           // Internal error: Fatal failure of the lock subsystem for this database. [,,,Db name,,]
#define SSCE_M_LIBRARYLOADFAILURE               25123           // A SQL Server Compact DLL could not be loaded. Reinstall SQL Server Compact. [GetLastError,,,DLL Name,,]
#define SSCE_M_INCONSISTENTLVVERSION            25124           // The long value was changed by another transaction. [,,,,,]
#define SSCE_M_INVALIDSTATSOPTIONS              25125           // Internal error: Invalid statistics creation options. [Options,,,,,]
#define SSCE_M_PENDINGUPDATE                    25126           // An update transaction is pending, cannot start a sync session. Try synchronizing again. [,,,,,]
#define SSCE_M_INVALIDPROVIDERTYPE              25127           // Invalid provider type. [Db version,Requested version,,File name,,]
#define SSCE_M_INVALIDSHAREDMEMORYFORMAT        25128           // SQL Server Compact has encountered two different versions of the product accessing the same database file. Ensure that each application accessing the same database file is using the same version of SQL Server Compact. [,,,,,]
#define SSCE_M_PREFIXTOOLARGE                   25129           // The string value that has been assigned to the LIKE clause in the SELECT statement exceeds the limit for SQL Server Compact. The limit for single byte character set is 256 characters but can be smaller for double byte character set. [,,,,,]
#define SSCE_M_UNINSTALLEDCOLLATINGSEQ          25130           // The specified locale is not installed on this machine. Make sure you install the appropriate language pack. [LCID,,,,,]
#define SSCE_M_CANTLOADSQLQPDLL                 25131           // Query Processor module could not be loaded. Reinstall SQL Server Compact. [,,,,,]
// DEAD SSCE_M_RESTRICTEDPLATFORM               25132           // This version of SQL Server Compact is not licensed for production use. [,,,,,]
#define SSCE_M_NOTSUPPORTED                     25133           // Internal error: SQL Server Compact made an unsupported request to the host operating system. [,,,,,]

// Errors after this comment were added for v3.5
//
#define SSCE_M_READREQUIREDRECORDS              25134           // The required number of records have already been read. [,,,,,]
#define SSCE_M_IDENTITYINSERT_ON                25135           // IDENTITY_INSERT is already ON for another table.  Cannot perform SET operation for this table. [,,, Other Table Name, This Table Name,]
#define SSCE_M_NO_IDENTITY                      25136           // Table does not have a IDENTITY column. [,,,,,]
#define SSCE_M_INVALIDENCRYPTIONMODE            25137           // The database encryption mode is invalid. [Input Encryption Mode,,,,,]
#define SSCE_M_DBUPGRADENEEDED                  25138           // The database file has been created by an earlier version of SQL Server Compact. Please upgrade using SqlCeEngine.Upgrade() method. [,,,,,]
#define SSCE_M_INVALID3XDATABASE                25139           // The file that is being referenced is not a valid SQL Server Compact 3.5 RTM/SP1/SP2 or SQL Server 2005 Compact Edition (version 3.1) or SQL Server 2005 Mobile Edition (version 3.0) database file to perform upgrade. [,,,Db name,,]
#define SSCE_M_INVALIDSRCPASSWORD               25140           // The password specified for the source database is incorrect. [,,,Data Source,,]
#define SSCE_M_ILLEGALSRCCOLLATINGSEQ           25141           // The source database locale is not supported on this operating system. [LCID,,,,,]
#define SSCE_M_POSSIBLEDATALOSS                 25142           // The repair of the source database results in potential data loss. Full recovery is not possible. [,,,,,]

// Errors after this comment were added for QFEs [25142 - 25147]
//


// Errors after this comment were added for v3.5 SP2
//
#define SSCE_M_TABLEALREADYTRACKED              25148           // The table is already being tracked for changes. To modify the change tracking options on the table, disable the change tracking on the table, and enable the change tracking with the new options. [,,,Table name,,]
#define SSCE_M_INVALIDRETENTIONPERIOD           25149           // The specified retention period is invalid. The retention period cannot be more than 1000 days. [,,,Table name, Retention period,]
#define SSCE_M_TABLEDOESNOTTRACKDELETES         25150           // The retention cleanup sequence in the tombstone table will not do any operation as the deletes are not being tracked in the user table. [,,,Table name,,]
#define SSCE_M_CANTTRACKSYSTEMTABLES            25151           // The change tracking operations are not permitted on the system tables. [,,,Table name,,]
#define SSCE_M_SYSTEMTABLEDELETIONFAILED        25152           // Could not drop system tables, as part of disabling tracking. [,,,,,]
#define SSCE_M_DATABASENOTTRACKED               25153           // The database is not being tracked for changes. Enable tracking to perform the operation. [,,,,,]
#define SSCE_M_READONLYDBPOSTINITFAILED         25154           // The database is opened with a read-only connection. Can't perform post-initialization operations like re-building indexes and upgrading public tracking. Please re-open with a read-write connection. [,,,Database name,,]
#define SSCE_M_SPINLOCKZOMBIED                  25155           // Unable to connect to shared memory region of the SQL Server Compact database. This is because of another process, owning the shared memory region lock, has exited abnormally in the lock acquired state.  This is an intermittent problem. Closing and restarting the database connection should fix the problem. [Thread id,Process id,,Db name,,]


// Errors after this comment were added for v4.0 but ported over for WM7
//
#define SSCE_M_FILECORRUPTED                    25156           // The database file is corrupted, possibly due to a file system issue. [,,,Database name,,]


// Errors after this comment were added for v4.0 (starts at 25158)
//
#define SSCE_M_DEFAULTALREADYPRESENT            25158           // This column already had a default. [,,,,,]
#define SSCE_M_NOTLONGVALUE                     25159           // The column is not of long value type. Check the column value type before opening the long value column. [,,,Table name, Column name,]
#define SSCE_M_ALREADYHAVEPREFIX                25160           // The key column value cannot be added to the current search key. No more key column values can be added to the current search key. The current search key already contains a partial prefix of a key column. Change the make key logic to follow the index key column order and the index prefix range option. [,,,,,]
#define SSCE_M_DELETEKEYNOTFOUND                25161           // The key cannot be deleted. The key to be deleted is already deleted from an index by another concurrent session. [,,,,,]
#define SSCE_M_FREEDPAGE                        25162           // Lock cannot be acquired for a referred object. The page which contains the referred object is already freed by another concurrent session. [,,,,,]
#define SSCE_M_NEEDSESSIONBEGIN                 25163           // The session cannot commit. Session begin did not happen or begin was not successful. Begin the session before its commit. [,,,,,]
#define SSCE_M_MAXNOTIFYCLIENTS                 25164           // The client cannot be registered for notifications. The registered clients have already reached the maximum allowed limit. Retry after some notify clients are unregistered. [,,,,,]
#define SSCE_M_LIBRARYVERIFICATIONFAILURE       25165           // The signature verification of the SQL Server Compact native binaries has failed. This could be due to the presence of incorrect version of the SQL Server Compact native binaries or the binaries may be from a different source than Microsoft. Please install SQL Server Compact binaries that have been digitally signed by Microsoft. [,,,DLL Name,,] 
#define SSCE_M_LIBRARYNOTFOUND                  25166           // A SQL Server Compact DLL is not found. Please reinstall SQL Server Compact properly. [,,,DLL Name,,]
#define SSCE_M_UNEXPECTEDSHAREDMEMORYOPEN       25167           // Internal Error while opening the share memory, cancelled the open. [,,,,,]
#define SSCE_M_BADIMAGEFORMAT                   25168           // The binary is either not designed to run on windows or it contains an error. [,,,,,]
#define SSCE_M_UPGRADENOTNEEDED                 25169           // Database upgrade is not required, the database is already in a compatible format. [,,,Db name,,]

// Error for use when no error mapping exists from OS Error (GetLastError()) to SQL CE Error
//
#define SSCE_M_UNMAPPEDGETLASTERROR             25199           // The operating system error could not be mapped to a known set of errors. [Reported Error,Line Number,,,,]

// Warnings
//
#define SSCE_WRN_COLUMNNULL                     25200           // The column cannot have NULL values. [,,,,,]
#define SSCE_WRN_BUFFERTRUNCATED                25201           // The data was truncated. Try passing down a larger buffer. [,,,,,]
// DEAD SSCE_WRN_SEEKNOTEQUAL                   25202           // Seek was not able to find an exact match. [,,,,,]
// DEAD SSCE_WRN_OPTIONSIGNORED                 25203           // Some specified options were ignored for this operation. [,,,,,]
// DEAD SSCE_WRN_COLUMNDATATRUNCATED            25204           // The data was truncated. [,,,,,]
// DEAD SSCE_WRN_DATABASEENCRYPTED              25205           // The database is encrypted. A password is necessary to access the database. [,,,,,]
#define SSCE_WRN_RI_RECORDSUPDATED              25206           // Additional records were updated as a result of referential integrity actions. [,,,,,]
#define SSCE_WRN_RI_RECORDSDELETED              25207           // Additional records were deleted as a result of referential integrity actions. [,,,,,]
// DEAD SSCE_WRN_ROWLOCKCONNECTFAILED           25208           // The request for a row-level lock was denied. [,,,,,]
// DEAD SSCE_WRN_PAGELOCKCONNECTFAILED          25209           // The request for a page-level lock was denied. [,,,,,]
// DEAD SSCE_WRN_DEFAULTALREADYPRESENT          25210           // This column already had a default. [,,,,,]
//
//Warnings after this comment were added for v1.1 (PEBBLE) 
//
//Warnings after this comment were added for v4.0 
//
#define SSCE_WRN_NOCLEANUPDONE                  25211           // No metadata cleanup has been done as metadata must be retained for one day. The minimum retention period is one day. [,,,,,]

// --------------------------------------------------------------------------------------
//
// OLEDB Layer  27000 - 27499
//
#define SSCE_M_TOOMANYSESSIONSOUTSTANDING       27000           // Too many sessions are open. [,,,,,]

// Rich error messages for Set Property
//
#define SSCE_M_PRIMARYKEYPROPERTYNOTSETTABLE    27001           // The new column cannot be set as the PRIMARY KEY constraint. Create a table-level constraint instead. [,,,,,]
#define SSCE_M_UNIQUEKEYPROPERTYNOTSETTABLE     27002           // The new column cannot be set as a UNIQUE constraint. Create a table-level constraint instead. [,,,,,]
#define SSCE_M_COLUMNNULLABILITYNOTALTERABLE    27003           // Column nullability cannot be changed after the column is created. [,,,,,]
#define SSCE_M_CANTMODIFYIDENTITYATTRIBUTE      27004           // The column cannot become an identity column after it is created. [,,,,,]
#define SSCE_M_INVALIDCOLUMNPROPERTY            27005           // The column property is not valid. [,,,,,]
// DEAD SSCE_M_INVALIDSSCEDBINITPROPERTY        27006           // An attempt to set the SQLServer CE DBINIT property is not valid. [,,,,,]
//
// Errors after this comment were added for v1.1 (PEBBLE) 
//


// --------------------------------------------------------------------------------------
//
// ssceCommon   27500 - 27749
//
#define SSCE_M_DECIMALOVERFLOW                  27500           // An overflow occurred while setting decimal data. [,,,,,]
#define SSCE_M_DECIMALTRUNCATED                 27501           // Decimal data was truncated. [,,,,,]
#define SSCE_M_CMN_INVALIDPARAMETER             27502           // The parameter is not valid. [,,,,,]
//***** Don't add any more errors here, read the comments below and add appropriately
//
// Errors after this comment were added for v1.1 (PEBBLE) 
//

// Warning
// DEAD SSCE_WRN_DECIMALTRUNCATED               27700           // Decimal data was truncated. [,,,,,]
//
// Warnings after this comment were added for v1.1 (PEBBLE) 
//

// Errors after this comment were added for v3.0 (LAGUNA) 
//
#define SSCE_M_INVALIDPREPMODE                  27506           // Internal Error: Unexpected value for SEPREPAREMODE. [,,,,,]
// --------------------------------------------------------------------------------------
//
// Managed Code     27750 - 27999
// 
// This range was added for Daytona
//
// DEAD SSCE_M_SSCEMWFAILURE                    27750           // Cannot load sscemw20.dll or ssceca20.dll is missing or not registered. [,,,,,]
//***** Don't add any more errors here, read the comments below and add appropriately

// Errors after this comment were added for v3.0 (LAGUNA)
//
#define SSCE_M_WRONGPROCEDUREPARAMFORM          27781           // After the form '@name = value' has been used, all subsequent parameters must be passed in the form '@name = value'. [Procedure parameter ordinal,,,,,]
#define SSCE_M_CO_E_NOTINITIALIZED              27782           // The application thread has not been initialized for COM interop because COM threading model has not been set. Please check documentation for System.STAThreadAttribute or System.MTAThreadAttribute classes.[,,,,,]
#define SSCE_M_INVALIDDBRANGEFLAGS              27783           // The following combination of DbRangeOptions in the call to SqlCeCommand:SetRange is invalid. [,,,DbRangeOptions,,]
#define SSCE_M_DBRANGEUNEXPECTEDNULLVALUE       27784           // With the specified DbRangeOptions, the argument in the call to SqlCeCommand:SetRange cannot be null (Nothing in Visual Basic.NET). [,,,DbRangeOptions,Argument Name,]
#define SSCE_M_DBRANGEEXPECTEDNULLVALUE         27785           // With the specified DbRangeOptions, the argument in the call to SqlCeCommand:SetRange MUST be set to null (Nothing in VB.NET). [,,,DbRangeOptions,Argument Name,]
#define SSCE_M_DBSEEKNOKEYVALUES                27786           // No key values were specified in the call to SqlCeDataReader:Seek. [,,,Argument Name,,]
#define SSCE_M_INVALIDDBSEEKOPTION              27787           // Invalid DbSeekOptions specified in the call to SqlCeDataReader:Seek. [,,,,,]

// Errors after this comment were added for v4.0 (VENICE)
//
#define SSCE_M_DUPLICATEASSEMBLY                27751           // The specified assembly already exists in this database. [,,,name,,] 
#define SSCE_M_INVALIDASSEMBLYVERSION           27752           // The assembly with this name is already registered. The version number does not match the expected value. [,,,name, ver, ver]  
#define SSCE_M_INVALIDASSEMBLYNAME              27753           // SQL assembly name and .NET assembly name do not match. [,,,sqlname, clrname,] 
#define SSCE_M_CIRCULARASSEMBLYREFERENCE        27754           // Circular assembly references were detected. Operation has been canceled. [,,,name,,] 
#define SSCE_M_REFERENCELISTNOTEMPTY            27755           // The ReferenceAssemblyList of the assembly being dropped is not empty. [,,,name,,] 
#define SSCE_M_ROUTINELISTNOTEMPTY              27756           // The ReferenceRoutineList of the assembly being dropped is not empty. [,,,name,type,method] 
#define SSCE_M_ASSEMBLYNOTFOUND                 27757           // .NET assembly with the specified name does not exist. [,,,name,,] 
#define SSCE_M_MISSINGASSEMBLY                  27758           // One or more assemblies is missing in the assembly_bits specifier. [,,,name,,] 
#define SSCE_M_DUPLICATEASSEMBLYSPECIFIED       27759           // Assembly specified more than once in the assembly_bits specifier. [,,,name,,] 
#define SSCE_M_OPENSCOPEFAILURE                 27760           // Unable to access/read the specified assembly. [,,,name,,] 
#define SSCE_M_OPENSCOPEONMEMFAILURE            27761           // The specified binary literal is not a valid assembly file. [,,,index,,] 
#define SSCE_M_DUPLICATETRIGGER                 27762           // Trigger with the specified name already exists in this database. [,,,name,,] 
#define SSCE_M_INVALIDTABLENAME                 27763           // The specified table does not exist. [,,,name,,] 
#define SSCE_M_INVALIDTRIGGERNAME               27764           // The specified trigger does not exist in this database. [,,,name,,] 
#define SSCE_M_INVALIDCALLINGCONVENTION         27765           // The calling convention of the specified method does not match the expected value. The method should be defined as public static (Shared in Visual Basic). [,,,assembly, class, method] 
#define SSCE_M_INVALIDNUMBEROFARGS              27766           // The actual number of arguments in the specified method does not match the expected value. [expected, actual,,assembly, class, method] 
#define SSCE_M_INVALIDARGUMENTTYPES             27767           // The argument types in the specified method do not match the expected values. [,,,assembly, class, method] 
#define SSCE_M_TYPENOTFOUND                     27768           // The specified type was not found in this .NET assembly. [,,,assembly, class,] 
#define SSCE_M_METHODNOTFOUND                   27769           // The method with the specified name was not found in the class definition. [,,,assembly, class, method] 
#define SSCE_M_INVALIDRETURNTYPE                27770           // The return type of the specified method does not match the expected value. [,,,assembly, class, method] 
#define SSCE_M_INVOCATIONFAILURE                27771           // A critical error occured while invoking .NET trigger. [,,,assembly,class,name] 
#define SSCE_M_MANAGEDEXCEPTION                 27772           // A .NET exception has been thrown during trigger execution. [,,,message, source, stack trace] 
#define SSCE_M_INSTEADOFTRIGGERFIRED            27773           // The action caused an INSTEAD OF trigger to fire. [,,,,,] 
#define SSCE_M_FOREIGNKEYVIOLATION              27774           // INSTEAD OF DELETE/UPDATE trigger cannot be defined on a table that has a foreign key with a CASCADE on DELETE/UPDATE action defined. [,,,constraint,,] 
#define SSCE_M_TRIGGERNESTLEVELEXCEEDED         27775           // Recursive trigger invocation exceeded the maximum allowed nesting level. [,,,assembly, class, method, nestLevel] 
#define SSCE_M_TRANSACTIONABORTED               27776           // The current transaction has been aborted during trigger invocation. No operation has been performed. [,,,,,] 
#define SSCE_M_CANTLOADSQLCOMPACTDLL            27777           // Could not load database compaction library. Operation has been aborted. [,,,,,] 
#define SSCE_M_LOADASSEMBLYFAILED               27778           // Unable to load .NET assembly with the specified name. This is because the assembly does not exist or because your trigger/stored procedure has been compiled against an assembly with different strong name. [,,,name,,] 
#define SSCE_M_DUPLICATEPROCEDURE               27779           // Procedure with the specified name already exists in this database. [,,,name,,] 
#define SSCE_M_CANTDROPPROCEDURE                27780           // Cannot drop the procedure because it does not exist, or you do not have permission. [,,,name,,] 

#define SSCE_M_DISTRIBUTEDTRANSACTIONNOTSUP     27992           // SQL Server Compact does not support distributed transactions. The command is currently enlisted in a transaction. You can not change the database in use until that transaction is committed or aborted. [,,,,,]
#define SSCE_M_CANTDROPDATABASE                 27993           // Cannot drop the database because it does not exist, or you do not have permission. [,,,,,]
#define SSCE_M_NESTEDTRANSACTIONSSNOTSUPPORTED  27994           // Nested transactions are not supported. [,,,,,]
#define SSCE_M_NOTENLISTEDINTRANSACTION         27995           // The command is not enlisted in a transaction. [,,,,,]
#define SSCE_M_TDSSERVERTHREADDEADLOCK          27996           // SQL Server Compact is not responding, potentially waiting for a lock timeout. The current connection will be closed. [,,,,,]
#define SSCE_M_SCRIPTMAXSIZEEXCEEDED            27997           // The size of the executed SQL Server Compact script has exceeded its maximum value. Use [GO] statements to reduce the size of submitted SQL batches. [maximum size,,,,,] 
#define SSCE_M_OUTOFMEMORY                      27998           // There is not enough memory on the device running SQL Server Compact to complete this operation. [,,,,,] 
#define SSCE_M_INVALIDPRODUCTVER                27999           // SQL Server Compact 4.0 is either not installed or the installation has been corrupted. [,,,,,] 

//***** Don't add any more errors here, read the comments below and add appropriately
//

// --------------------------------------------------------------------------------------
//
// Replication Transport    28000 - 28249
//
#define SSCE_M_OPENTWICE                        28000           // Internal error: The table was unexpectedly opened. [,,,,,]
#define SSCE_M_INTERNETREADERROR                28001           // Internal error: A read operation from the device or the IIS server returned an incorrect number of bytes while trying to process a synchronization request. [,,,,,]
#define SSCE_M_INTERNETWRITEERROR               28002           // A write operation to the Internet wrote an incorrect number of bytes. [,,,,,]
#define SSCE_M_INVALIDRSCBID                    28003           // Internal error: Either the computer running IIS is out of memory or an incorrect session ID was sent in a request. [Session id,,,,,]
#define SSCE_M_TOOMANYPENDINGREQUESTS           28004           // A SQL Server Compact Server Agent queue is full and can take no further requests. Try again later. [,,,,,]
// DEAD SSCE_M_COULDNTSTARTRECTHREAD            28005           // A thread to host the SQL Server Reconciler cannot be started. Check system resources. [,,,,,]
// DEAD SSCE_M_CANTLOADRECRESOURCEDLL           28006           // The SQL Server Reconciler resource DLL cannot be loaded. [,,,,,]
#define SSCE_M_INVALIDRECONCILERPARAM           28007           // Internal error: Invalid reconciler parameter. [,,,,,]
#define SSCE_M_INVALIDTRNSPTCOMMAND             28008           // Internal error: The transport failed due to incorrect message format or content. [Command number,,,,,]
#define SSCE_M_CANTCOCREATERECONCILER           28009           // An instance of the SQL Server Reconciler cannot be created. Try reinstalling the replication components.[,,,,,]
#define SSCE_M_CANTCOCREATERECONERROR           28010           // An instance of the SQL Server Reconciler error object cannot be created. Try reinstalling the replication components.[,,,,,]
#define SSCE_M_HTTPSTATUSDENIED                 28011           // Authentication failed on the computer running IIS. [,,,,,]
#define SSCE_M_HTTPSTATUSPROXYAUTHREQ           28012           // Authentication failed on the proxy server. [,,,,,]
#define SSCE_M_HTTPSTATUSBADREQUEST             28013           // The HTTP request failed due to incorrect format or content. Try restarting the IIS server.[,,,,,]
#define SSCE_M_HTTPSTATUSPAYMENTREQ             28014           // This site requires payment before it can be accessed. Try restarting the IIS server.[,,,,,]
#define SSCE_M_HTTPSTATUSFORBIDDEN              28015           // A Secure Sockets Layer (SSL) connection is required to access this site. [,,,,,]
#define SSCE_M_HTTPSTATUSNOTFOUND               28016           // IIS error 404 HTTP_STATUS_NOT_FOUND: The server has not found anything matching the requested URI (Uniform Resource Identifier). [,,,,,]
#define SSCE_M_HTTPSTATUSBADMETHOD              28017           // IIS Error 405 HTTP_STATUS_BAD_METHOD: The HTTP verb used is not allowed. [,,,,,]
#define SSCE_M_HTTPSTATUSNONEACCEPT             28018           // IIS Error 406 HTTP_STATUS_NONE_ACCEPTABLE: No responses acceptable to the client were found. [,,,,,]
// DEAD SSCE_M_HTTPSTATUSREQTIMEOUT             28019           // The request timed out on the computer running IIS. [,,,,,]
#define SSCE_M_HTTPSTATUSCONFLICT               28020           // The computer running IIS returned a Conflict status. Try restarting the IIS server.[,,,,,]
#define SSCE_M_HTTPSTATUSGONE                   28021           // The computer running IIS returned a Gone status. Try restarting the IIS server.[,,,,,]
#define SSCE_M_HTTPSTATUSSERVERERROR            28022           // An error has occurred on the computer running IIS. Try restarting the IIS server.[,,,,,]
#define SSCE_M_HTTPSTATUSNOTSUPPORTED           28023           // The computer running IIS returned a Not Supported status. Try restarting the IIS server.[,,,,,]
#define SSCE_M_HTTPSTATUSBADGATEWAY             28024           // IIS Error 502 HTTP_STATUS_BAD_GATEWAY:  The server, while acting as a gateway or proxy, received a response that was not valid from the upstream server. [,,,,,]
#define SSCE_M_HTTPSTATUSSVCUNAVAIL             28025           // The IIS service is not available. [,,,,,]
// DEAD SSCE_M_HTTPSTATUSGATTIMEOUT             28026           // The gateway has timed out. [,,,,,]
// DEAD SSCE_M_HTTPSNOTSUPPORTED                28027           // HTTPS is not supported on Windows CE 1.1-based devices. [,,,,,]
#define SSCE_M_REPLPROHIBITED                   28028           // Merge replication is restricted from being processed with the SQL Server Compact Server Agent on the computer running IIS. Check the USAGE key in HKLM\\Software\\Microsoft\\Microsoft SQL Server Compact Edition\\v4.0\\Transport\\USAGE and change to a value of 0 or 1. [,,,,,]
#define SSCE_M_RDAPROHIBITED                    28029           // Remote data access (RDA) is restricted from being processed with the SQL Server Compact Server Agent on the computer running IIS. Check the USAGE key in HKLM\\Software\\Microsoft\\Microsoft SQL Server Compact Edition\\v4.0\\Transport\\USAGE and change to a value of 0 or 2. [,,,,,]
#define SSCE_M_COMPRESSIONFAILED                28030           // Internal error: Compression failed. Try again.[,,,,,]
#define SSCE_M_DECOMPRESSIONFAILED              28031           // Internal error: Decompression failed. Try again.[,,,,,]
#define SSCE_M_FAILUREINITIALIZINGINTERNET      28032           // The call to InternetOpen failed. Check the major error for more information. [,,,,,]
#define SSCE_M_INVALIDURLSYNTAX                 28033           // The URL syntax is not valid. [,,,URL,,]
#define SSCE_M_INTERNETCONNECTFAILURE           28034           // An Internet connection cannot be established. [,,,,,]
#define SSCE_M_FAILUREQUERYINGHEADER            28035           // Internal error: HTTP header information is either corrupted or missing in the transport message. It could be a network transmission error or an IIS problem. [,,,,,]
#define SSCE_M_CLOSEINTERNETHANDLEFAILED        28036           // An internal or network problem was encountered while trying to close an Internet handle. [,,,,,]
#define SSCE_M_HTTPSENDREQUESTFAILED            28037           // A request to send data to the computer running IIS has failed. For more information, see HRESULT. [,,,,,]
#define SSCE_M_MAXIMUMRSCBSEXCEEDED             28038           // The connection was refused by the computer running IIS because the maximum number of sessions has been exceeded. Try connecting later. [,,,,,]
// DEAD SSCE_M_REQUESTSUPERSEDED                28039           // The request was superseded by a subsequent request. [,,,,,]
#define SSCE_M_RESENTCOMMANDIGNORED             28040           // Internal error: During a retry operation to synchronize, the command has been completed. The resent command was ignored. [,,,,,]
#define SSCE_M_RESENDREQUEST                    28041           // Internal error: Unexpected resend request. [,,,,,]
//***** Don't add any more errors here, read the comments below and add appropriately
//
// Errors after this comment were added for v1.1 (PEBBLE) 
//

// --------------------------------------------------------------------------------------
//
// Replication provider    28250 - 28499
//

// --------------------------------------------------------------------------------------
//
// Client Agent     28500 - 28999
//
#define SSCE_M_CANTCOCREATESSCEERRORS               28500       // A SQL Server Compact error collection object cannot be created on the IIS server. The SQL Server reconciler is not loading. Verify that the merge replication components are running on SQL Server. [,,,,,]
// DEAD SSCE_M_SUBSCRIBERDATABASEPATHMISSING        28501       // The SubscriberConnectionString property is not specified. [,,,,,]
// DEAD SSCE_M_INTERNETURLPROPERTYMISSING           28502       // The InternetURL property is not specified. [,,,,,]
// DEAD SSCE_M_PUBLISHERPROPERTYMISSING             28503       // The Publisher property is not specified. [,,,,,]
// DEAD SSCE_M_PUBLISHERDATABASEPROPERTYMISSING     28504       // The PublisherDatabase property is not specified. [,,,,,]
// DEAD SSCE_M_PUBLICATIONPROPERTYMISSING           28505       // The Publication property is not specified. [,,,,,]
// DEAD SSCE_M_SUBSCRIBERPROPERTYMISSING            28506       // The Subscriber property is not specified. [,,,,,]
// DEAD SSCE_M_MISSINGDISTRIBUTORPROPERTIES         28507       // Some Distributor properties are not specified. [,,,,,]
// DEAD SSCE_M_MISSINGDISTRIBUTORADDRESS            28508       // The DistributorAddress property is not specified. [,,,,,]
// DEAD SSCE_M_MISSINGPUBLISHERADDRESS              28509       // The PublisherAddress property is not specified. [,,,,,]
// DEAD SSCE_M_INVALIDPUBLISHERSECURITYMODE         28510       // The value specified for the PublisherSecurityMode property is not valid. [Publisher security mode,,,,,]
// DEAD SSCE_M_INVALIDDISTRIBUTORSECURITYMODE       28511       // The value specified for the DistributorSecurityMode property is not valid. [Distributor security mode,,,,,]
// DEAD SSCE_M_INVALIDQUERYTIMEOUTVALUE             28512       // The value specified for the QueryTimeout property is not valid. [Query timeout value,,,,,]
// DEAD SSCE_M_INVALIDLOGINTIMEOUTVALUE             28513       // The value specified for the LoginTimeout property is not valid. [Login timeout value,,,,,]
// DEAD SSCE_M_INVALIDDISTRIBUTORNETWORK            28514       // The value specified for the DistributorNetwork property is not valid. [Distributor network,,,,,]
// DEAD SSCE_M_INVALIDEXCHANGETYPE                  28515       // The value specified for the ExchangeType property is not valid. [Exchange type,,,,,]
// DEAD SSCE_M_INVALIDVALIDATIONTYPE                28516       // The value specified for the ValidateType property is not valid. [Validation type,,,,,]
// DEAD SSCE_M_INITIALIZEFAILED                     28517       // The Initialize method failed. [,,,,,]
// DEAD SSCE_M_ADDSUBSCRIPTIONFAILED                28518       // The Add Subscription method failed. [,,,,,]
#define SSCE_M_UNSUPPORTEDDBDROPOPTION              28519       // The value specified for DBDROPOPTION is not valid. Valid values are 0 and 1. [,,,,,]
#define SSCE_M_DROPSUBSCRIPTIONFAILED               28520       // The Drop Subscription method failed. [,,,,,]
#define SSCE_M_DBALREADYREPLICABLE                  28521       // The SQL Server Compact subscription already exists. Publisher, PublisherDatabase, and Publication for this subscription should be different from any existing subscription. [,,,,,]
// DEAD SSCE_M_RUNFAILED                            28522       // The Run method failed. [,,,,,]
#define SSCE_M_INITIALIZATIONFAILED                 28523       // Internal error: Initializing an internal object failed. [,,,,,]
// DEAD SSCE_M_TERMINATEFAILED                      28524       // The Terminate method failed. [,,,,,]
// DEAD SSCE_M_REINITIALIZESUBSCRIPTIONFAILED       28525       // The Reinitialize Subscription method failed. [,,,,,]
#define SSCE_M_UNEXPECTEDROCENCOUNTERED             28526       // The message contains an unexpected replication operation code. The version of SQL Server Compact Client Agent and SQL Server Compact Server Agent should match. [replication operation code,,,,,]
#define SSCE_M_COLUMNORDINALNOTFOUND                28527       // Internal error: The column ordinal cannot be found. [,,,Table name (if known),,]
#define SSCE_M_NICKNAMEGENERATIONFAILED             28528       // Internal error: A nickname was not generated. [,,,,,]
#define SSCE_M_DBNOTREPLICABLE                      28529       // The SQL Server Compact database is not enabled for replication. Try synchronizing again. [,,,,,]
#define SSCE_M_FAILUREREADINGREGISTRY               28530       // Internal error: SQL Server Compact was unable to read the generated machine ID from the registry. If a network device is installed on the device, a generated machine ID will not be necessary. [,,,,,]
#define SSCE_M_FAILUREWRITINGREGISTRY               28531       // Internal error: SQL Server Compact was unable to write the generated machine ID from the registry. If a network device is installed on the device, a generated machine ID will not be necessary. [,,,,,]
#define SSCE_M_TRANSPORTREADRETURNEDNODATA          28532       // A read operation from the transport returned no data. The version of SQL Server Compact Client Agent and SQL Server Compact Server Agent should match.[,,,,,]
// DEAD SSCE_M_CANTCOCREATESQLSERVERCE              28533       // Another instance of SQL Server Compact cannot be created. [,,,,,]
// DEAD SSCE_M_RDAPULLFAILED                        28534       // The Pull method failed. [,,,,,]
// DEAD SSCE_M_RDAPUSHFAILED                        28535       // The Push method failed. [,,,,,]
// DEAD SSCE_M_RDASUBMITSQLFAILED                   28536       // The SubmitSQL method failed. [,,,,,]
#define SSCE_M_IDENTITYRANGE                        28537       // Internal error: The identity range was not established. [,,,,,]
// DEAD SSCE_M_INVALIDARTICLE                       28538       // An article in the message is not valid. [Article nickname,,,,,]
#define SSCE_M_OBJECTALREADYINITIALIZED             28539       // Internal error: The database is already initialized. [,,,,,]
#define SSCE_M_OBJECTNOTINITIALIZED                 28540       // Internal error: The database is not initialized. [,,,,,]
#define SSCE_M_LOCALTABLENAMEMISSING                28541       // The value for the Local Table Name parameter is not specified for the Push call. [,,,,,]
#define SSCE_M_NOTALLPARAMSSETFORPULL               28542       // Some parameters were not specified for the Pull method. [,,,,,]
#define SSCE_M_TABLENOTTRACKED                      28543       // The table is not a tracked table. [,,,Table name,,]
#define SSCE_M_RDAERRORROWSRETURNED                 28544       // The Push method returned one or more error rows. See the specified error table. [,,,Error table name,,]
// DEAD SSCE_M_SUBSCRIBERCONNECTIONSTRINGMISSING    28545       // SQL Server Compact connection string is missing. [,,,,,]
#define SSCE_M_INVALIDCONNECTIONSTRING              28546       // The specified OLE DB for SQL Server Compact connection string is not valid. [,,,,,]
#define SSCE_M_DATABASEALREADYOPEN                  28547       // SQL Server Compact database cannot be compacted because it is open. [,,,,,]
#define SSCE_M_PARAMETERTOOLARGE                    28548       // A specified parameter is too large. [Size of parameter,Max size,,Parameter value,,]
#define SSCE_M_FAILEDRETRIES                        28549       // The row operation cannot be reapplied due to an integrity violation. Check the Publication filter. [,,,Table,Operation,RowGuid]
// DEAD SSCE_M_COMPACTFAILED                        28550       // The SQL Server Compact database cannot be compacted. [,,,,,]
#define SSCE_M_MISSINGCONNECTSTRING                 28551       // The Local Connection String parameter is not specified for either a Push or Pull method. [,,,,,]
#define SSCE_M_NO_ROWGUID                           28552       // Internal error: This table does not have a ROWGUID column. [,,,,,]
#define SSCE_M_SPECIAL_COLUMN                       28553       // Internal error: This table does not have a ROWGUID column nor an Identity range. [,,,,,]
#define SSCE_M_VALIDATEFAILED                       28554       // The number of rows at the client does not match the number of rows at the server. Retry the operation. [SQL Server count, SQL Server Compact count,,TableName,,]
#define SSCE_M_FAILEDMAKEDBREP                      28555       // The SQL Server Compact database cannot be replicated. The subscription must be reinitialized. [,,,,,]
#define SSCE_M_TABLENAMEMISSING                     28556       // The Local Table Name parameter is not specified. [,,,,,]
// DEAD SSCE_M_UNUSABLEDATABASE                     28557       // The SQL Server Compact database cannot be used for replication. Delete and then recreate the database. [,,,Connection string,,]
#define SSCE_M_CREATEDATABASE                       28558       // SQL Server Compact encountered problems when creating the database. [,,,,,]
#define SSCE_M_OPENDATABASE                         28559       // SQL Server Compact encountered problems when opening the database. [Internal Error Number,,,Internal Error String,,]
#define SSCE_M_EXECUTEFAILED                        28560       // The SQL statement failed to execute. If this occurred while using merge replication, this is an internal error. If this occurred while using RDA, then the SQL statement is invalid either on the PULL statement or on the SubmitSQL statement. [,,,SQL statement,,]
#define SSCE_M_ROLLBACKFAILED                       28561       // Internal error: Failed to roll back changes. [,,,,,]
#define SSCE_M_PROCESSDELETEFAILED                  28562       // Internal error: The delete message for the server was not created. [,,,,,]
#define SSCE_M_CLIENTAGENT_NOMEMORY                 28563       // The device does not have sufficient memory. SQL Server Compact cannot allocate more space on the device. There also might not be enough contiguous memory space. Try closing other applications. [,,,,,]
// DEAD SSCE_M_DELETEFAILED                         28564       // The delete action failed. For more details on this specific error code, check the HRESULT error and also refer to the MSDN OLE DB documentation. [,,,Table name,,]
#define SSCE_M_INSERTFAILED                         28565       // The insert action failed. For more details on this specific error code, check the HRESULT error and also refer to the MSDN OLE DB documentation. [,,,Table name,,]
#define SSCE_M_UPDATEFAILED                         28566       // The update action failed. For more details on this specific error code, check the HRESULT error and also refer to the MSDN OLE DB documentation. [,,,Table name,,]
#define SSCE_M_OPENTABLEFAILED                      28567       // Internal error: The table cannot be opened. [,,,Table name,,]
// DEAD SSCE_M_CLOSETABLEFAILED                     28568       // The table cannot be closed. [,,,Table name,,]
#define SSCE_M_INVALIDINDEX                         28569       // Internal error: The index cannot be set on the ROWGUID column. [,,,,,]
#define SSCE_M_ERRORFETCHINGDATA                    28570       // Internal error: The data row cannot be accessed. [,,,,,]
#define SSCE_M_ERRORFETCHINGCOLUMNDATA              28571       // Internal error: The column data for a row cannot be accessed. [,,,,,]
#define SSCE_M_ERRORSETTINGDATA                     28572       // Internal error: Row data cannot be set. [,,,,,]

//***** Don't add any more errors here, read the comments below and add appropriately
//
//Errors after this comment were added for v1.1 (PEBBLE) 
//
#define SSCE_M_TABLEALREADYEXISTS                   28573       // The LocalTableName parameter is already specified. [,,,Table name,,]

// Errors after this comment were added for v2.0 (DAYTONA)
//
#define SSCE_M_INTERNALERROR                        28574       // An internal error occurred. [ID,,,,,]
// DEAD SSCE_M_INSUFFICIENTLINEAGESPACE             28575       // There is not enough space for lineage in a row. [,,,,,]
#define SSCE_M_ERRORBINDINGCOLUMN                   28576       // Internal error: Could not bind to column. [,,,columnname,,]
#define SSCE_M_SCHEMAHASCHANGED                     28577       // The schema has changed since the table was pulled. [,,,,,]
#define SSCE_M_UNSUPPORTEDDATATYPE                  28578       // The data type is not valid. [Datatype,,,,,]
// DEAD SSCE_M_MOVENEXTFAILED                       28579       // The move to the next row failed. [,,,,,]
#define SSCE_M_INVALIDCOMPACTPARAMETER              28580       // The specified parameter is not valid. [,,,parameter name,,]
#define SSCE_M_FAILUREMAKINGMESSAGE                 28581       // Internal error: The message cannot be built. The Make Message failed. [,,,,,]
#define SSCE_M_INVALIDPARAMETER                     28582       // The parameter is either not specified or not valid. [,,,,,]
#define SSCE_M_PUBLISHERLOGINMISSING                28583       // The Publisher login property is not specified. [,,,,,]

// Errors after this comment were added for v3.0 (LAGUNA)
//
// DEAD SSCE_M_INVALIDCONNECTIONRETRYTIMEOUT        28584       // The value specified for the ConnectionRetryTimeout property is not valid. [ConnectionRetryTimeout,,,,,]
// DEAD SSCE_M_INVALIDCOMPRESSIONLEVEL              28585       // The value specified for the CompressionLevel property is not valid. [CompressionLevel,,,,,]
// DEAD SSCE_M_INVALIDREPLPARAMETER                 28586       // The parameter is either not specified or not valid. [,,,,,]
#define SSCE_M_INVALIDHOSTNAME                      28587       // The HostName property value does not match the one saved for the subscription. To resolve, use the original HostName or reinitialize the subscription. [,,,current hostname,original hostname,]
// DEAD SSCE_M_ROWHASCHANGED                        28588       // The row has changed by a different user and cannot be modified. [,,,,,]
#define SSCE_M_CONNMANMAPURLFAILED                  28589       // The Connection Manager could not map a URL into a connection. Check your Connection Manager settings. [,,,URL,,]
#define SSCE_M_CONNMANCONNECTFAILED                 28590       // The Connection Manager could not connect to the network. Check your Connection Manager settings. [,,,,,]
// DEAD SSCE_M_RESYNC                               28592       // An internal resync is necessary. User should never see this error. [,,,,,]
#define SSCE_M_SUBSCRIPTIONNOTFOUND                 28593       // The subscription was not found. Publisher, PublisherDatabase, or Publication  could be incorrect. [,,,,,]
#define SSCE_M_SUBSCRIPTIOWASNOTSAVED               28594       // The subscription could not be loaded because it was not saved. [,,,,,]


// DEAD SSCE_M_PROFILEEXISTS                        28595       // The profile already exists. [,,,,,]
// DEAD SSCE_M_PROFILEPROPENCRYPTFAILED             28596       // Encryption of a profile property failed. [,,,,,]
// DEAD SSCE_M_PROFILEPROPDECRYPTFAILED             28597       // Decryption of a profile property failed. [,,,,,]
#define SSCE_M_PROCESSVALIDATEFAILED                28598       // Validation of table row counts failed. Try resynchronizing. [,,,,,]
#define SSCE_M_NOTARGETKEY                          28599       // The referenced table must have a primary or candidate key. [,,,FK Name,,]
#define SSCE_M_DBINSYNC                             28600       // The database is currently synchronizing with the server. [,,,,,]
#define SSCE_M_STREAMACCESSDENIED                   28601       // Internal error: Stream access denied. [,,,,,]
#define SSCE_M_UNIQUEREQUIRED                       28602       // Internal error: When trying to add a GUID tracking column, SQL Server Compact could not create the column because the column did not contain the uniqueidentifier attribute. [,,,Column name, Table name,]
// DEAD SSCE_M_INVALIDROWVERSIONDEF                 28603       // Invalid type for rowversion column. [,,,Column name, Table name,]
#define SSCE_M_READONLYTABLE                        28604       // Internal error: This table is read-only. The internal system objects and system lock tables are not updateable. [,,,Table name,,]
#define SSCE_M_RESTRICTEDDDL                        28605       // Data Definition Language (DDL) operations are not allowed on this table, or on any system table or tracked user table.  Disable tracking before you run DDL operations. [,,,Table name,,]
#define SSCE_M_QPDEFAULTSSERVICEMISSING             28606       // QP is missing and it is required to evaluate default expressions. Ensure that Query Processor module is in the same directory as the Storage Engine module. [,,,,,]
#define SSCE_M_TABLEDOESNOTEXIST                    28607       // The table does not exist [,,,Table name,,]
#define SSCE_M_CANTMODIFYCOLUMNTYPE                 28608       // Internal error: Cannot modify the column type [,,,Table name, Column name,]
#define SSCE_M_INVALIDDBVERSION                     28609       // Incompatible Database Version. If this was a compatible file, run repair. For other cases refer to documentation. [Db version,Requested version,,File name,,]
#define SSCE_M_SINGLEUSEROPERATION                  28610       // This operation requires the database to be opened in a single user mode and the database is currently in a multi-user mode. [,,,File name,,]
#define SSCE_M_PATHTOOLONG                          28611       // The file resolves to a path that is too long. The maximum length is 260 characters. [,,,File name,,]
#define SSCE_M_INVALIDRDAERRORTABLE                 28612       // The schema of the RDA error table  is not valid. Delete the error table and try to repull the table. [,,,,,]
#define SSCE_M_STRINGTOGUIDFAILED                   28613       // Internal error: Failure converting string to GUID. [,,,String,,]
#define SSCE_M_GETMINORERRORFAILED                  28614       // Internal error: Internal error: Failed to get the minor error code. [,,,,,]
#define SSCE_M_GETROWGUIDINDEXFAILED                28615       // Internal error: Failed to get the index on the row GUID. [,,,Table name, Guid Column Name,]
#define SSCE_M_CHECKLOCALTABLEFAILED                28616       // Internal error: Failure checking existance of local table. [,,,Table name,,]
// DEAD SSCE_M_GETIDENTITYCOLUMN                    28617       // Failed to get the name of the identity column. [,,,Table name,,]
#define SSCE_M_QUERYINTERFACE                       28618       // Internal error: QueryInterface failed. [,,,,,]
#define SSCE_M_SEQUENTIALSTREAMFAILURE              28619       // Internal error: Failure reading from SequentialStream. [,,,,,]
#define SSCE_M_OPENROWSETFAILED                     28620       // Internal error: Opening of the rowset failed. [,,,,,]
#define SSCE_M_BINDINGS                             28621       // Internal error: Failure setting up bindings, possibly caused by insufficient permissions. [,,,,,] [,,,,,]
#define SSCE_M_NONPARAMQUERY                        28622       // Internal error: Failure setting up a non parameterized query, possible incorrect SQL query. [,,,,,]
#define SSCE_M_INCORRECTDATABASETYPE                28623       // Internal error: SQL Server Compact function called with SQL database type. [,,,,,]
#define SSCE_M_CLOSECURSORFAILED                    28624       // Internal error: Failure closing a cursor. [,,,,,]
#define SSCE_M_COLUMNINFO                           28625       // Internal error: Failure getting column information. [,,,,,]
#define SSCE_M_SETCOLUMN                            28626       // Internal error: Failure setting column value. [,,,,,]
#define SSCE_M_OPENSQLSERVER                        28627       // Failure to open SQL Server with given connect string. [,,,connect string,,]
#define SSCE_M_CLOSEDATABASEFAILED                  28628       // Internal error: Closing the database failed. [,,,,,]
#define SSCE_M_DELETEDATABASE                       28629       // Deleting the database failed. Ensure that other applications do not have the database open. [,,,,,]
#define SSCE_M_STARTTRANSACTION                     28630       // Starting the transaction failed. Try to reduce the number of other open transactions in the application. [,,,,,]
#define SSCE_M_HTTPOPENREQUEST                      28631       // WinInet Error: HTTP Open request failed. [,,,,,]
#define SSCE_M_UNEXPECTEDHTTPSTATUS                 28632       // IIS Error: IIS returned an HTTP status that is not expected to be returned to SQL Server Compact client. This error does not mean that the synchronization did not work, it only means that SQL Server Compact does not recognize the error from IIS. [,,,status,,]
#define SSCE_M_OPENMESSAGEFILE                      28633       // Internal error: Failure opening the message file. This occurs when processing the message fails on the client side. [,,,,,]
#define SSCE_M_WRITEMESSAGEFILE                     28634       // Failure writing to a message file on the device. The error typically comes from running out of memory. While there might appear to be plenty of available memory on the device, there might not be enough contiguous memory. Try closing other applications or warm booting the device. [,,,,,]
#define SSCE_M_READMESSAGEFILE                      28635       // Failure reading from a message file. The error typically comes from running out of memory. While there might appear to be plenty of available memory on the device, there might not be enough contiguous memory. Try closing other applications or warm booting the device. [,,,,,]
#define SSCE_M_CLOSETRANSPORT                       28636       // Internal error: Failure closing the transport. [,,,,,]
#define SSCE_M_DROPDATABASE                         28637       // Could not delete the database because more then one subscription exists. [,,,Publication, Publication Database, Publisher]
#define SSCE_M_DUPLICATETABLE                       28638       // Existing subscription already contains table included in the new subscription. [,,,Table name,,]
#define SSCE_M_SYNCPARTNERGENERATIONFAILED          28639       // Internal error: A sync partner ID (GUID value) was not generated. [,,,,,]
#define SSCE_M_INVALIDSNAPSHOTTRANSFERTYPE          28640       // The value specified for the SnapshotTransferType property is not valid. Valid values are 0 and 1. [SnapshotTransferType,,,,,]
#define SSCE_M_SYNCHADWARNINGS                      28641       // The synchronization succeeded but generated warnings. [No of warnings generated,No of warnings in error collection,,,,]
#define SSCE_M_RDABASEROWMISSING                    28642       // Internal error: Unable to create error table row because corresponding base table row has been deleted. [,,,,,]
// DEAD SSCE_M_TRACEALREADYON                       28643       // The tracer is already switched on. [,,,,,]
#define SSCE_M_PROPERTYMISSING                      28644       // A required property is not specified. [,,,Property name,,]
#define SSCE_M_INVALIDPROPERTYVALUENUMERIC          28645       // An invalid value was supplied for a property. [property value,,,Property name,,]
#define SSCE_M_SYNC_CANCELED                        28646       // The sync has been canceled. [,,,,,]

// Errors after this comment were added for v3.5
//
#define SSCE_M_DBAUTOUPGRADEFAILED                  28647       // The database auto upgrade from recognized old database format to the current database format failed due to a read-only connection.  Please open a the connection in write mode to perform auto upgrade. [Db version,Requested version,,File name,,]
#define SSCE_M_CONFLICTINGSUBSCRIPTION              28648       // There is a conflicting subscription found. Can't use replication protocols RDA or merge with OCS. [,,,Table name,,]

// Errors after this comment were added for v3.5 SP1
//
#define SSCE_M_AUTOPROXYURLACCESSFAILED             28649      // The proxy information can not be retrieved for the URL. proxyURL can not be accessed, try to manually access proxyURL [URL,proxyURL,,,,]
#define SSCE_M_AUTOPROXYCALLFAILED                  28650      // Calling a method in JSProxy dll failed. Make sure JSProxy.dll is in path, and the method is exposed and can be called. [method name,,,,,]

// Warnings
//
#define SSCE_WRN_RDAERRORROWSRETURNED               28800       // The Push returned one or more error rows. See the specified error table. [,,,,,]
#define SSCE_WRN_SUBWINSERRORSFIXED                 28801       // A data error occurred and was fixed. Call SqlCeReplication.Synchronize (or Run in eVC/eVB) method again to merge data. [,,,,,]

//
// Warnings after this comment were added for v1.1 (PEBBLE) 

// Warnings after this comment were addef for v3.0 (Laguna)
//
#define SSCE_WRN_FAILEDUPLOAD                       28802       // A subscriber row failed to apply at the publisher. Review the error parameters and the conflict logging tables at the publisher to determine the reason the row failed to apply. [,,,Table name, Row guid,]

// --------------------------------------------------------------------------------------
//
// Server Agent     29000 - 29499
//
// DEAD SSCE_M_UNEXPECTEDSTATEREADINGGENINFO        29000       // An unexpected state was found when reading generation information. [,,,,,]
#define SSCE_M_EXPECTEDINSERTUPDATESONLY            29001       // Internal error: An unexpected state was encountered when processing inserts or updates. [,,,,,]
// DEAD SSCE_M_IDENTITYSEEDORRANGETOOLARGE          29002       // The identity seed or range is too large. [,,,,,]
// DEAD SSCE_M_UNEXPECTEDSTATEPROCESSINGTOMBSTONES  29003       // An unexpected state was encountered when processing deletes. [,,,,,]
#define SSCE_M_CANTCOCREATEMSDAINITIALIZE           29004       // The OLE DB provider for SQL Server cannot be initialized. Check if SQLOLEDB.dll is installed correctly. [,,,,,]
#define SSCE_M_RSCBWITHSAMEEXCHANGEID               29005       // A replication session control block already exists. The IIS server was probably shut down during a synchronization process. Try synchronizing again. [,,,,,]
#define SSCE_M_SQLRECONCILERFAILED                  29006       // A call to SQL Server Reconciler failed. Try to resynchronize. [,,,,,]
#define SSCE_M_QUERYCANNOTBETRACKED                 29007       // The query cannot be tracked. There might not be a primary key, or the query might involve multiple tables. [,,,Query string,,]
// DEAD SSCE_M_PREPAREUPDATECMDFAILED               29008       // The prepare/update command process failed. [,,,,,]
#define SSCE_M_EXPRESSIONWITHOUTALIAS               29009       // Internal error: The expression or literal cannot be processed without an alias. RDA error. [,,,,,]
#define SSCE_M_NOPRIMARYKEY                         29010       // The table does not have a primary key. [,,,Table name,,]
#define SSCE_M_ROWVERCOLUMN                         29011       // The table contains a timestamp or another row versioning column. [,,,Table name,,]
#define SSCE_M_INVALIDPUSHSEQUENCE                  29012       // Internal error: An error occurred during the push operation in the SQL Server Compact Server Agent. [,,,,,]
// DEAD SSCE_M_PUSHPARAMSETONVALUE                  29013       // The attempt to replace an existing parameter value in the push operation failed. [,,,,,]
// DEAD SSCE_M_NOTALLPARAMSETFORPUSH                29014       // Incomplete parameters were set for the push operation. [,,,,,]
#define SSCE_M_PUSHPKCOLCOUNTMISMATCH               29015       // The expected number of columns in the primary key on SQL Server does not match the count in SQL Server Compact. The schema on SQL Server has changed since the last RDA Pull. The table will have to be PULLED again to match the primary key schema defined on SQL Server. [Supplied count, Expected count,,,,]
#define SSCE_M_VF_MISSINGSERVERCOLUMN               29016       // Client-side and server-side columns for the RDA table do not match. [,,,Table name,,]
#define SSCE_M_VF_MISMATCHEDTYPES                   29017       // The OLE DB data type information in the SQL Server Compact columns does not match the information in the SQL Server columns for the RDA table. [Client type, Server type,,Table name,,]
#define SSCE_M_VF_MISSINGTABLE                      29018       // No columns were found. Either the table name is missing or the permissions are not valid. [,,,Table name,,]
#define SSCE_M_NOROWSAFFECTED                       29020       // A row cannot be updated or deleted in the SQL Server by using RDA. [,,,,,]
#define SSCE_M_TOOMANYROWSAFFECTED                  29021       // More than one row was affected by an RDA update or delete operation in the SQL Server database. [,,,,,]
#define SSCE_M_INCORRECTPROVIDERVERSION             29022       // The version of the Microsoft OLE DB Provider for SQL Server is not correct. Install MDAC 2.8 or later. [,,,Version,,]
#define SSCE_M_INCORRECTPROVIDER                    29023       // The OLE DB Provider for SQL Server on the computer running IIS is not compatible with SQL Server Compact. Install MDAC 2.8 or later. [,,,Provider,,]
#define SSCE_M_VF_PRIMARYKEYCHANGE                  29024       // The push operation cannot match the order of the primary key columns on the client side and server side. Repull the table or reset the primary key column. [,,,,,]
#define SSCE_M_VF_MISSINGSERVERPKCOLUMN             29025       // The push operation cannot find a primary key column on the SQL Server table. [,,,,,]
#define SSCE_M_VF_EXTRASERVERPKCOLUMN               29026       // The push operation has found an extra primary key column on the SQL Server table. [,,,,,]
// DEAD SSCE_M_MISSINGCOLUMNSCHEMACOLS              29027       // The current OLE DB provider is not supported. [,,,,,]
#define SSCE_M_UNSUPPORTEDOLEDBTYPE                 29028       // The OLE DB data type is not supported by the Microsoft OLE DB Provider for SQL Server. Install MDAC 2.8 or later. [Data type,,,,,]
#define SSCE_M_VF_NONLOBIUNKNOWN                    29029       // Internal error: The SQL Server Compact column metadata information does not match with the OLEDB provider (non-SQL Server). [,,,,,]
#define SSCE_M_NOTRACKPKCOLMISSING                  29030       // The query cannot be tracked because one or more columns of the primary key are missing. [,,,,,]
#define SSCE_M_NOTRACKCOMPUTEDCOLUMN                29031       // The query cannot be tracked because the table includes a computed column that cannot be pulled. [,,,,,]
#define SSCE_M_NOTRACKMULTITABLE                    29032       // The query cannot be tracked because it is a multi-table query. [,,,,,]
#define SSCE_M_INVALIDCODEPAGE                      29033       // The server code page, or the override code page, is either not valid or not installed on the computer running IIS. [Code page,,,,,]
//***** Don't add any more errors here, read the comments below and add appropriately
// 
// Following error is a V1.1 (Pebble) error
// included here due to the numbering problem that produced the gap below.
//
#define SSCE_M_MISMATCHEDCOMPONENTS                 29034       // The Client Agent and Server Agent component versions are incompatible. For a meaningful handshake, the Client Agent version should not be later than the Server Agent version. Client Agent version should be no earlier than SQL Server 2005 Mobile Edition (version 3.0).  Re-install the replication components with the matching versions for client and server agents. [,,,Client Agent version,Server Agent version,]
// Note:  There is a gap in the numbering here. Do not renumber the following, since they are already documented.
//        New errors for this section should continue from the highest value in the section.
//        The two errors below are from V1.0    
//
#define SSCE_M_RSCBTIMEDOUT                         29042       // The replication session was inactive for longer than the cleanup interval and the synchronization was cancelled. This typically occurs when the server has an extremely high CPU utilization. The default of 2 hrs was exceeded. Try the operation again. [,,,,,]
// DEAD SSCE_M_NOPROXYSERVERSUPPORT                 29043       // The operating system on the Windows CE-based device does not support authenticated proxy servers. [,,,,,]
//
// Errors after this comment were added for v1.1 (PEBBLE) 
//
#define SSCE_M_FILERENAMEFAILED                     29044       // SQL Server Compact failed to rename an .IN file to .OUT on the IIS server.  This typically occurs when there are large amounts of referential integrity retry situations on SQL Server Compact while using merge replication because of which SQL Server Compact is unable to modify the file name extensions. Try again. [,,,,,]
#define SSCE_M_INITIALIZERECONCILERFAILED           29045       // Initializing SQL Server Reconciler has failed. Try again. [,,,,,]

//
// Errors after this comment were added for v2.0 (DAYTONA)
//
#define SSCE_M_READONLY                             29046       // While performing an RDA Push, SQL Server Compact was only able to obtain a read-only cursor on the table(s) being pushed to SQL Server. There could be locking conflicts on SQL Server while trying to do the push. Verify that the table on SQL Server is updatable and try again. [,,,,,]
// DEAD SSCE_M_PROCESSINGROWS                       29047       // Row processing resulted in an error. [,,,,,]
#define SSCE_M_IMPERSONATIONFAILED                  29048       // The Windows user account could not be impersonated at the computer running IIS. [,,,,,]

//
// Errors after this comment were added for v3.0 (LAGUNA)
//
#define SSCE_M_CANTCONNECTTOPUBLISHER               29049       // The server agent could not connect to the publisher. [,,,,,]
#define SSCE_M_GETROWSET                            29050       // Internal error: Failure getting a rowset. [,,,,,]
#define SSCE_M_GETCOLUMNS                           29051       // Internal error: Failure getting columns. [,,,,,]
#define SSCE_M_CREATEINDEXSQL                       29052       // Internal error: Failure generating RDA create index SQL statement. [,,,,,]
#define SSCE_M_CREATETABLESQL                       29053       // Internal error: Failure generating RDA create table SQL statement. [,,,,,]
#define SSCE_M_GETPRIMARYKEYCOLUMNS                 29054       // Internal error: Failure getting RDA primary key columns. . [,,,,,]
#define SSCE_M_CREATEPRIMARYKEY                     29055       // Internal error: Failure generating create RDA primary key sql statement. [,,,,,]
#define SSCE_M_VERIFYSCHEMA                         29056       // Internal error: Failure while verifying RDA schema. [,,,,,]
#define SSCE_M_PREPAREINSERT                        29057       // Internal error: Failure while preparing an RDA insert. [,,,,,]
#define SSCE_M_PREPAREUPDATE                        29058       // Internal error: Failure while preparing an RDA update. [,,,,,]
#define SSCE_M_PREPAREDELETE                        29059       // Internal error: Failure while preparing an RDA delete. [,,,,,]
#define SSCE_M_CONNECTFAILURESQL                    29060       // Failure to connect to SQL Server with provided connection information. SQL Server does not exist, access is denied because the SQL user is not a valid user on the SQL Server, or the password is incorrect.[,,,,,]
#define SSCE_M_CONNECTFAILUREIIS                    29061       // Failure to connect to SQL Server with provided connection information. SQL Server does not exist, access is denied because the IIS user is not a valid user on the SQL Server, or the password is incorrect.[,,,,,]
#define SSCE_M_SELECTSTRINGMISSING                  29062       // The SQLSelectString parameter is not specified. [,,,,,] 

// --------------------------------------------------------------------------------------
//
// Message Protocol     29500- 29999
//  
#define SSCE_M_TRANSPORTREADWRONGSIZE               29500       // Internal error: The transport read operation returned an incorrect data length. [Returned length,Expected length,,,,]
#define SSCE_M_UNEXPECTEDSTATEWRITINGMESSAGE        29501       // Internal error: The write message failed. [,,,,,]
#define SSCE_M_INVALIDCOLUMNNUMBER                  29502       // Internal error: The schema on SQL Server differs from the schema on SQL Server Compact. [Column number,,,Table name,,]
#define SSCE_M_CANTFINDCOLUMNBYNAME                 29503       // The column cannot be found. [,,,Column name,Table name,]
#define SSCE_M_NAMESBUFFERTOOSMALL                  29504       // Internal error: The names buffer is too small. [,,,,,]
// DEAD SSCE_M_INCORRECTNUMBEROFCOLUMNSPROCESSED    29505       // Incorrect number of columns processed. [columns processed,expected number,,,,]
#define SSCE_M_UNEXPECTEDSTATEGETTINGCOLUMNS        29506       // Internal error: Failure getting column information. Retry the operation. [,,,,,]
#define SSCE_M_UNEXPECTEDSTATEGETCOLLENGTH          29507       // Internal error: Failure getting column length. Retry the operation. [,,,,,]
#define SSCE_M_UNEXPECTEDSTATEGETCOLDATA            29508       // Internal error: Failure getting column data. Retry the operation. [,,,,,]
#define SSCE_M_UNEXPECTEDSTATESETCOLUMN             29509       // Internal error:  Failure setting column data. Retry the operation. [,,,,,]
#define SSCE_M_STREAMNOTEXHAUSTED                   29510       // BLOB data could not be completely read. Retry the operation. [,,,,,]
#define SSCE_M_GARBLEDMESSAGEFROMSERVER             29511       // Internal error: The message received from the server cannot be decoded. [,,,,,]
// DEAD SSCE_M_GARBLEDMESSAGEFROMCLIENT             29512       // The message received from the client cannot be decoded. [,,,,,]
//***** Don't add any more errors here, read the comments below and add appropriately
//
// Errors after this comment were added for v1.1 (PEBBLE) 
//


// --------------------------------------------------------------------------------------
//
// Query Compile        25500 -- 25699
//  
#define SSCE_M_QP_BADOPT                            25500       // Internal error: The execution plan cannot be generated. [,,,,,]
#define SSCE_M_QP_BADPARSE                          25501       // There was an error parsing the query. [Token line number,Token line offset,,Token in error,,]
#define SSCE_M_QP_SETLISTCOUNT                      25502       // The count of column names and source expressions do not match. [Column name count,Source expression count,,,,]
#define SSCE_M_QP_BAD_COLNAME                       25503       // The column name is not valid. [,,,Node name (if any),Column name,]
#define SSCE_M_QP_BADCNSTS                          25505       // The constraint specified is not valid. [,,,,,]
#define SSCE_M_QP_NOMEMORY                          25506       // There is not enough memory to complete the operation. [,,,,,]
#define SSCE_M_QP_UNCLOSEDCOMMENT                   25507       // There is an unclosed comment block in the SQL statement. Ensure that there are balanced '/*' and '*/' comment markers in the SQL statement. [,,,,,]
#define SSCE_M_QP_TOKTOOLONG                        25508       // The token specified is too long. The maximum length is 128 characters. [Maximum size of token (if known),,,Token(if known),,]
#define SSCE_M_QP_ATOF_OVERFLOW                     25509       // A conversion from string to float data failed. [,,,String,,]
#define SSCE_M_QP_NUME_OUTOFRANGE                   25510       // A conversion from string to numeric data failed. [,,,String value,,]
#define SSCE_M_QP_UNCLOSEDQUOTE                     25511       // A quotation mark delimiter is missing from the end of the query. [,,,,,]
#define SSCE_M_QP_BADMONEY                          25512       // Conversion from string to money data failed. [,,,Bad value,,]
#define SSCE_M_QP_IDENTWITHNULL                     25513       // Null values are not valid identifiers. [,,,Identifier,,]
#define SSCE_M_QP_NAME_EMPTY                        25514       // The identifier cannot be an empty string. [,,,,,]
#define SSCE_M_QP_NO_SCALAR_IN_SELECT               25515       // In aggregate and grouping expressions, the SELECT clause can contain only aggregates and grouping expressions. [,,,Select clause,,]
#define SSCE_M_QP_NO_AGG_IN_AGG                     25516       // Nested aggregate expressions are not allowed. [,,,,,]
#define SSCE_M_QP_NO_SCALAR_IN_HAVING               25517       // In aggregate and grouping expressions, the HAVING clause can contain only aggregate functions and grouping expressions. [,,,Having clause,,]
#define SSCE_M_QP_NO_SCALAR_IN_AGGRORDERBY          25518       // In aggregate and grouping expressions, the ORDER BY clause can contain only aggregate functions and grouping expressions. [,,,,,]
#define SSCE_M_QP_NO_AGG_IN_GROUPBY                 25519       // Expressions in the GROUP BY list cannot contain aggregate functions. [,,,,,]
#define SSCE_M_QP_NO_AGG_IN_ORDERBY                 25520       // Expressions in the ORDER BY list cannot contain aggregate functions. [,,,,,]
#define SSCE_M_QP_NO_DUPEXPR_IN_ORDERBY             25521       // The ORDER BY list cannot have duplicate expressions. [,,,Order by list,Duplicate expression,]
#define SSCE_M_QP_DUP_TABLE_ALIAS                   25522       // The table aliases must be unique. [,,,Name of duplicate alias,,]
#define SSCE_M_QP_DUP_COLUMN_ALIAS                  25523       // The column aliases must be unique. [,,,Name of duplicate alias,,]
#define SSCE_M_QP_HAVING_WITHOUT_GROUPBY            25524       // A HAVING clause must have a GROUP BY clause (implicit or explicit). [,,,,,]
#define SSCE_M_QP_NO_AGG_IN_WHERE                   25525       // The WHERE clause cannot refer to aggregate expressions. [,,,,,]
#define SSCE_M_QP_DUP_CNST                          25526       // Duplicate constraints cannot be specified. [,,,,,]
#define SSCE_M_QP_AMBIGUOUS_FUNC                    25527       // A function with the same signature already exists. [,,,Name of function,,]
// DEAD SSCE_M_QP_CANT_COCREATE                     25529       // The OLE DB provider cannot be instantiated. [,,,,,]
#define SSCE_M_QP_SETLIST_DUPCOLNAME                25530       // A column with the same name has already been specified. [,,,Name of duplicate column,,]
#define SSCE_M_QP_AMBIGUOUS_COLNAME                 25531       // The column name cannot be resolved to a table. Specify the table to which the column belongs. [,,,Name of ambiguous column,,]
#define SSCE_M_QP_BAD_FLOAT_PRECISION               25532       // The float precision is not valid. [,,,,,]
#define SSCE_M_QP_LONGIDNAME                        25533       // The identifier/column name is too long. The maximum length is 128 characters. [,,,Name of column(if known),,]
#define SSCE_M_QP_LOB_IN_ORDERBY                    25534       // Large objects (ntext and image) cannot be used in ORDER BY clauses. [,,,,,]
#define SSCE_M_QP_INVALIDMETA_FOR_LOB               25536       // Length, precision or scale cannot be specified for NTEXT or IMAGE data types. Remove these attributes from the IMAGE or NTEXT data types in the DLL statement. [,,,,,]
#define SSCE_M_QP_SUBQUERY_COLUMN_MISMATCH          25537       // The columns in the subquery do not match. [Column ordinal,,,Column name,,]
#define SSCE_M_QP_ENCRYPTION_NO_DBPASSWORD          25538       // The database is encrypted. A password must be specified. [,,,,,]
#define SSCE_M_QP_BADLIKEESCCHAR                    25539       // The escape character for the LIKE predicate is not valid. [,,,Escape character,,]
#define SSCE_M_QP_BADDATEOPTION                     25540       // The datepart is not valid. [,,,Portion of data in error,,]
#define SSCE_M_QP_INVALID_IDENTIFIER                25541       // The identifier is not valid. [,,,Identifier,,]
#define SSCE_M_QP_READONLYINSERT                    25542       // Data cannot be added to a system table or a schema information view. [,,,Name of read-only table,,]
#define SSCE_M_QP_READONLYUPDATE                    25543       // Data cannot be updated in a system table or a schema information view. [,,,Name of read-only table,,]
#define SSCE_M_QP_READONLYDELETE                    25544       // Data cannot be deleted from a system table or a schema information view. [,,,Name of read-only table,,]
#define SSCE_M_QP_NOINSERTCOLUMNS                   25545       // A value must be provided for the INSERT statement into <table_name>. [,,,Table name,,]
#define SSCE_M_QP_MODIFYPROTECTEDCOL                25546       // The column cannot be modified because it is an identity, rowversion or a system column. [,,,Column name,,]
#define SSCE_M_QP_SCALARORDERBYNOTSUPPORTED         25547       // ORDER BY <column number> not supported. [,,,,,]
#define SSCE_M_QP_LOB_IN_DISTINCT                   25548       // DISTINCT cannot be used with  ntext and image data. [,,,,,]
#define SSCE_M_QP_LOB_IN_GROUPBY                    25549       // GROUP BY cannot be used with ntext and image data. [,,,,,]
#define SSCE_M_QP_MULTI_IDENTITY_COLUMNS            25550       // Too many identity columns are specified for the table. Only one identity column for each table is allowed. [,,,,,]
#define SSCE_M_QP_BAD_IDENTITY_TYPE                 25551       // The identity column must be either an integer or big integer data type and cannot be NULL. [,,,,,]
#define SSCE_M_QP_GROUPBY_MUST_REFERENCE_COLS       25552       // Elements in the GROUP BY clause must reference columns in the select list. [,,,,,]
#define SSCE_M_QP_CASCADEONSELFREFFK                25553       // A foreign key constraint that has an UPDATE or a DELETE CASCADE rule, and self-references a column in the same table, is not allowed. [,,,Constraint name,,]
#define SSCE_M_QP_NODEFAULTONIDENTITYCOL            25554       // Defaults cannot be created on identity columns. [,,,Name of table,Name of column,,]
#define SSCE_M_QP_SUBQUERY_NOLOBCOLUMNS             25555       // Subquery columns cannot be ntext or image data types. [,,,,,]
#define SSCE_M_QP_INVALID_CHARACTER_IN_IDENTIFIER   25556       // The number sign (#) is reserved and cannot be used as the first character of an identifier. [,,,Invalid identifier,,]
//***** Don't add any more errors here, read the comments below and add appropriately
//
// Errors after this comment were added for v2.0 
//
#define SSCE_M_UNIONCOLUMNCOUNT                     25557           // Expressions combined by a UNION operator must have the same number of columns. [Number of columns in first UNION expression, Number of columns in other UNION expression,,,,]
#define SSCE_M_DIFFERENTCOLUMNCOUNT                 25558           // The number of columns in the query and the table must match. [Number of columns in query, Number of columns in table,,,,]

// Errors after this comment were added for v3.0 
//
#define SSCE_M_INVALID_INDEX_HINT_ORDINAL           25559           // Invalid index ordinal in index hint specification. [Index ordinal,,,,,]
#define SSCE_M_INVALID_INDEX_HINT_NAME              25560           // Invalid index name in index hint specification. [,,,Index name,,]
#define SSCE_M_INVALID_QUERY_HINT_OPTION            25561           // Invalid query hint specification. [,,,,,]
#define SSCE_M_QP_INVALIDPERCENTSPEC                25562           // Percentage of rows to be sampled should be an integer between 1 and 100. [,,,Table Name,,]
#define SSCE_M_QP_STATS_ALREADY_EXIST               25563           // There are already statistics existing on this index. [,,,Table Name, Index Name,]
#define SSCE_M_QP_STATS_NOT_EXIST                   25565           // Statistics on this index do not exist. [,,,Table Name, Index Name,]
#define SSCE_M_QP_STATS_TABLE_TOO_SMALL             25567           // Table has too few rows to build statistics. [,,,Table Name,,]
#define SSCE_M_QP_INVALID_STATS_OPTION              25568           // Invalid statistics creation option. [,,,Stats option,,]
#define SSCE_M_QP_INVALID_SET_OPTION                25569           // Invalid set option. [,,,Set option,,]
#define SSCE_M_QP_ORDERBY_NOT_IN_SELECT_DISTICT     25571           // ORDER BY items must appear in the select list if SELECT DISTINCT is specified. [,,,,,]
#define SSCE_M_QP_MULTIPLE_SETSHOWPLAN_IN_BATCH     25572           // The SET SHOWPLAN statements must be the only statements in the batch. [,,,,,]
#define SSCE_M_QP_MISMATCHEDPARAMETERTYPE           25573           // Query cannot use both named and unnamed parameters at the same time. [,,,,,]
#define SSCE_M_QP_DUPLICATEPARAMETERNAME            25574           // Duplicated parameter names are not allowed. [,,, Parameter name,,]
#define SSCE_M_QP_ONLY_IN_FROM                      25575           // Index hints are only allowed in a FROM clause. [,,,,,]
#define SSCE_M_QP_BAD_LOCK_HINT                     25576           // Unrecognized LOCK HINTS option. [,,,,,]
#define SSCE_M_QP_ONLY_IN_SELECT                    25577           // The NOLOCK lock hint is only allowed in a SELECT statement. [,,,,,]
#define SSCE_M_QP_CONFL_LOCKING                     25578           // Conflicting locking hints specified. [,,,,,]
#define SSCE_M_QP_BAD_SET_STMT                      25579           // Unrecognized SET statement. [,,,,,]
#define SSCE_M_QP_INVALID_SEEDINCREMENT_VALUE       25580           // Identity seed or increment value is invalid. [,,,,,]
// DEAD SSCE_M_QP_CLR_CRASM_INVALID_EXPR            25581           // Invalid expression for assembly source parameter. CREATE ASSEMBLY failed. [,,,,,]
#define SSCE_M_QP_ALTERTABLE_NONULL                 25582           // Alter table only allows columns to be added which can contain null values. The column cannot be added to the table because it does not allow null values. [,,,,,]
// DEAD SSCE_M_QP_INVALIDROWSPEC                    25583           // Number of rows to be sampled should be larger than 0. [,,,Table Name,,]
#define SSCE_M_QP_COLUMNINUSE                       25584           // The column could not be modified because one or more objects access this column. [,,,Column Name, Object Name (if available),]
#define SSCE_M_QP_QUERY_NOT_KEYSET_NO_TABLE         25585           // Cannot generate a keyset cursor for the query because there is no reference to base table. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_DISTINCT      25586           // Cannot generate an updatable cursor for the query because there is a DISTINCT operator. [Cursor Option,,,,,]
#define SSCE_M_QP_CURSOR_NOT_UPDATABLE              25587           // Cursor is not updatable. [,,,,,]
#define SSCE_M_QP_COLUMN_NOT_UPDATABLE              25588           // Column is not updatable. [,,,Name of column,,]
#define SSCE_M_QP_INVALID_LONG_VALUE_COLUMN         25589           // Internal error: This is not a base table long value column. [,,,,,]
#define SSCE_M_QP_INVALID_CURSOR_OPTIONS            25590           // The combination of cursor options is not valid. For instance, updateable and insensitive is an invalid option. [Options specified,,,,,]
#define SSCE_M_QP_INVALID_LONG_VALUE_OBJECT_ID      25591           // Internal error: The storage interface for long value is invalid or unsupported. [,,,,,]
#define SSCE_M_QP_INVALID_SETCOLUMN                 25592           // SetColumn cannot be called during delete operation. [,,,,,]
#define SSCE_M_QP_BOOKMARK_NOT_SUPPORTED            25593           // Bookmark is not supported for this cursor. [,,,,,]
#define SSCE_M_QP_ALTERCOLUMN_LOB_INVALID           25594           // Cannot alter column of type NTEXT or IMAGE [,,,Column Name,,]
#define SSCE_M_QP_QUERY_NOT_KEYSET_DISTINCT         25595           // Cannot generate a keyset cursor for the query because there is a DISTINCT operator. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_KEYSET_GROUPBY          25596           // Cannot generate a keyset cursor for the query because there is an explicit or implicit GROUP BY operator. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_KEYSET_OUTER_JOIN       25597           // Cannot generate a keyset cursor for the query because there is a OUTER JOIN operator. [Cursor Option,,,,,]
// DEAD SSCE_M_QP_QUERY_NOT_KEYSET_SORT             25598           // Cannot generate a keyset cursor for the query because there is a SPOOL/SORT operator. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_KEYSET_UNION            25599           // Cannot generate a keyset cursor for the query because there is a UNION operator. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_GROUPBY       25600           // Cannot generate an updatable cursor for the query because there is an explicit or implicit GROUP BY operator. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_OUTER_JOIN    25601           // Cannot generate an updatable cursor for the query because there is an OUTER JOIN operator. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_SORT          25602           // Cannot generate an updatable cursor for the query because there is a SPOOL/SORT operator. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_UNION         25603           // Cannot generate an updatable cursor for the query because there is a UNION operator. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_NON_STANDARD_JOIN 25604       // Cannot generate an updatable cursor for the query because there is a non-standard join expression. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_JOIN_GRAPH_CYCLE 25605        // Cannot generate an updatable cursor for the query because there is a cycle in join graph. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_11_KEY_ORDER  25606           // Cannot generate an updatable cursor for the query because a one-to-one join must compare columns in the same order as declared in unique indexes. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_MM            25607           // Cannot generate an updatable cursor for the query because there is a many-to-many relationship. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_NON_UPDATABLE_RELATIONSHIP 25608 // Cannot generate an updatable cursor for the query because there are one or more non-updatable relationships among base tables. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_DISCONNECTED_JOIN_GRAPH 25609 // Cannot generate an updatable cursor for the query because one or more tables are not explicitly joined together. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_SELF_JOIN     25610           // Cannot generate an updatable cursor for the query because there is a self join. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_NOT_SAME_COLUMNS 25611        // Cannot generate an updatable cursor for the query because updatable tables are not joined on the same columns. [Cursor Option,,,,,]
#define SSCE_M_QP_QUERY_NOT_UPDATABLE_NO_UPDATABLE_COLUMN 25612     // Cannot generate an updatable cursor for the query because there is no updatable column. [Cursor Option,,,,,]
#define SSCE_M_QP_FTOA_OVERFLOW                     25613           // A conversion from float to string data failed. [,,,,,]
#define SSCE_M_QP_MISSINGPROCEDUREPARAMETER         25614           // Procedure expects parameter, which was not supplied. [,,,Procedure Name, Parameter Name,]
#define SSCE_M_QP_SCHEMAHASCHANGED                  25615           // The table schema has changed since the query was last compiled. Recompile the query. [,,,Table Name,,]
#define SSCE_M_QP_STATS_NOT_SUPPORTED               25616           // Statistics not supported on system tables. [,,,Table Name,,]
#define SSCE_M_QP_INVALID_INDEX_OPTION              25617           // Invalid option specified in CREATE INDEX statement. [,,,Option Name,,]
#define SSCE_M_QP_INVALID_STORED_PROC_NAME          25618           // Invalid stored procedure name. [,,,Stored Proc Name,,]
#define SSCE_M_QP_FULLSCAN_REQUIRED                 25619           // FULLSCAN option is required for CREATE/UPDATE STATISTICS. [,,,,,]
#define SSCE_M_QP_NAMED_CONSTRAINT_NOT_SUPPORTED    25620           // Named Constraint is not supported for this type of constraint. [,,,Constraint Name,,]
#define SSCE_M_QP_INVALIDPROCEDUREARGS              25621           // The specified argument value for the procedure is not valid. [Argument #,,,Name of procedure(if known),,]
#define SSCE_M_QP_INVALIDTABLENAME                  25622           // The table name is not valid. [,,,Table Name,,]
#define SSCE_M_QP_INVALID_GLOBAL_VARIABLE_NAME      25623           // The global variable name is not valid. [,,,Global variable Name,,]
#define SSCE_M_QP_INVALID_CREATE_DATABASE_OPTION    25624           // Invalid CREATE DATABASE option. [,,,Option,,]


// Errors after this comment were added for v3.5
//
#define SSCE_M_QP_NODEFAULTONROWVERCOL              25625           // Defaults cannot be created on row version columns. [,,,Name of table,Name of column,,]
#define SSCE_M_QP_ALTERCOLUMN_SRC_TIMESTAMP_INVALID 25626           // Cannot alter column of type TIMESTAMP [,,,Column Name,,]
#define SSCE_M_QP_NO_ORDERBY_WITHOUT_TOP_OR_OFFSET_IN_SUBQUERY   25628       // The ORDER BY clause is invalid in subqueries unless TOP or OFFSET is also specified. [,,,,,]
#define SSCE_M_QP_INVALID_TOP_OR_FETCH_VALUE        25629           // The number of rows in the TOP/FETCH clause must be a positive integer. [,,,,,]
#define SSCE_M_QP_INVALID_TOP_OR_FETCH_EXPRESSION   25630           // Invalid expression in the TOP/FETCH clause. Provide an integer constant, expression on constants or parameter.[,,,,,]
#define SSCE_M_QP_NOCOL_ALIAS_IN_SUBQUERY           25632           // Column names must be specified for constants, expressions or aggregate functions when they occur in a FROM sub query. [,,,,,]
#define SSCE_M_QP_MULTIPLE_COL_SPEC                 25633           // Column name was specified multiple times for a sub query. [,,,Name of column, Alias of sub query,]
#define SSCE_M_QP_CORRELATED_AGGR_IN_APPLY          25634           // Aggregates on the right side of an APPLY cannot reference columns from the left side. [,,,,,]
#define SSCE_M_QP_ALTERCOLUMN_TGT_TIMESTAMP_INVALID 25635           // Cannot alter column to be type TIMESTAMP [,,,Column Name,,]
#define SSCE_M_QP_CONSTEXPR_IN_GROUP_BY             25636           // Each GROUP BY expression must contain at least one column that is not an outer reference. [,,,,,]

// Errors after this comment were added for v3.5 sp1
//
#define SSCE_M_QP_NO_CONSTANT_IN_ORDERBY             25637          // A constant expression was encountered in the ORDER BY list. [,,,Position,,]
#define SSCE_M_QP_NO_ORDERBY_WO_SELECT_IN_UNION      25638          // ORDER BY items must appear in the select list if the statement contains a UNION. [,,,,,]

// Errors after this comment were added for v4.0
//
#define SSCE_M_QP_NO_OFFSET_WITHOUT_ORDERBY         25649           // OFFSET clause is invalid unless ORDER BY clause is specified. [,,,,,]
#define SSCE_M_QP_NO_FETCH_WITHOUT_OFFSET           25650           // FETCH clause is invalid unless OFFSET clause is specified. [,,,,,]
#define SSCE_M_QP_NO_TOP_WITH_OFFSET                25651           // TOP and OFFSET clause cannot be used together in the same query. [,,,,,]
#define SSCE_M_QP_INVALID_SKIP_VALUE                25652           // The number of rows in the OFFSET clause must be a positive integer. [,,,,,]
#define SSCE_M_QP_INVALID_SKIP_EXPRESSION           25653           // Invalid expression in the OFFSET clause. Provide an integer constant, expression on constants or parameter.[,,,,,]
#define SSCE_M_QP_INVALID_FETCH_VALUE               25654           // The number of rows in the FETCH clause must be an integer greater than 0. [,,,,,]

// --------------------------------------------------------------------------------------
//
// Query Expression Service     25900 -- 26099
//  
#define SSCE_M_QP_BADEXPR                           25900       // The query processor encountered an error evaluating the expression. [,,,,,]
#define SSCE_M_QP_OVFLO                             25901       // Expression evaluation caused an overflow. [,,,Name of function (if known),,]
#define SSCE_M_QP_EXPRTYPECHECK                     25902       // The data types in the IN expression do not match. [,,,,,]
#define SSCE_M_QP_DATEPART                          25903       // There was an error in a part of the date format. [,,,Expression (if known),,]
#define SSCE_M_QP_DATEABORT                         25904       // There was an error in the date format. [,,,Expression,,]
#define SSCE_M_QP_SYNTAX                            25905       // There was a syntax error in the date format. [,,,Expression,,]
#define SSCE_M_QP_CONVSYNTAX                        25906       // There was a syntax error while converting from one data type to another. [,,,Expression,,]
#define SSCE_M_QP_MONEYOVERFLOW                     25907       // An overflow occurred while converting the string data type to money. [,,,String,,]
#define SSCE_M_QP_NOCOLUMNREFINDEFAULT              25909       // Default values cannot have column references. [,,,,,]
#define SSCE_M_QP_NOAGGRINDEFAULT                   25910       // Default values cannot have aggregate functions. [,,,,,]
#define SSCE_M_QP_NOAGGRONLOB                       25911       // Aggregate value expressions cannot have ntext or image data. [,,,,,]
#define SSCE_M_QP_NOCONVDUETOTRUNC                  25912       // The original data is truncated and cannot be converted. [,,,,,]
#define SSCE_M_QP_ARITHOPNOTSUPPORTED               25913       // The arithmetic operator is not supported for the data type. [,,,,,]
#define SSCE_M_QP_NOMODULOSUPPORT                   25914       // Modulo is not supported on real, float, money, and numeric data types. [,,,Data type,,]
#define SSCE_M_QP_SUBQUERYINVALIDOP                 25916       // An operator on the result data type from a subquery is not valid. [,,,,,]
#define SSCE_M_QP_NOBITOPSUPPORT                    25917       // Bit operators (&,|,^,~) are not supported on real, float, money, and numeric data types. [,,,Data type,,]
#define SSCE_M_QP_BADARITHTYPE                      25918       // The data type is not valid for the arithmetic operation. [,,,Data type (if known),,]
// DEAD SSCE_M_QP_BADARITHOP                        25919       // The arithmetic operator is not recognized by SQL Server Compact. [,,,Operator,,]
#define SSCE_M_QP_TRUNCATION                        25920       // The data was truncated while converting from one data type to another. [,,,Name of function(if known),,]
#define SSCE_M_QP_INVALIDFUNCTIONNAME               25921       // The function is not recognized by SQL Server Compact. [,,,Name of function,Data type (if known),]
#define SSCE_M_QP_INVALIDFUNCTIONARGS               25922       // The specified argument value for the function is not valid. [Argument #,,,Name of function(if known),,]
#define SSCE_M_QP_NOSUPPORTFORLOB                   25923       // The ntext and image data types cannot be used in WHERE, HAVING, GROUP BY, ON, or IN clauses, except when these data types are used with the LIKE or IS NULL predicates. [,,,,,]
#define SSCE_M_QP_BINDTSYNTAX                       25924       // An overflow might have occurred while converting binary to datetime.[,,,,,]
#define SSCE_M_QP_NUMDTSYNTAX                       25925       // An overflow might have occurred while converting numeric to datetime. [,,,,,]
#define SSCE_M_QP_MONDTSYNTAX                       25926       // An overflow might have occurred while converting money to datetime. [,,,,,]
#define SSCE_M_QP_BIGINTDTSYNTAX                    25927       // An overflow might have occurred while converting bigint to datetime. [,,,,,]
#define SSCE_M_QP_TOOMANYARGS                       25928       // There are too many arguments specified for the procedure or function. [Count of arguments,,,,,]
#define SSCE_M_QP_UMINUSNUMONLY                     25929       // A unary minus can be performed only on tinyint, smallint, int, bigint, money, and numeric data types. [,,,,,]
#define SSCE_M_QP_PRECISIONMISMATCH                 25930       // The operation cannot be performed because the precision of the numeric data types do not match. [Precision of right data type (if known), Precision of left data type (if known),,,]
#define SSCE_M_QP_SCALEMISMATCH                     25931       // The operation cannot be performed because the scale of the numeric data types do not match. [Scale of right data type (if known), Scale of left data type (if known),,,]
#define SSCE_M_QP_NOIMPLICITCONVALLOWED             25932       // The data types cannot be converted implicitly. [,,,Source data type,Destination data type,]
#define SSCE_M_QP_DATETIMEOVFLO                     25933       // An overflow occurred while converting to datetime. [,,,,,]
// DEAD SSCE_M_QP_MISSINGTYPELOGIC                  25934       // An error occurred when executing a code path that requires type specific logic that is missing. [,,,,,]
#define SSCE_M_QP_DIVBYZERO                         25935       // Dividing by zero is not allowed. [,,,Name of function (if known),,]
#define SSCE_M_QP_NOTIMEPARTSPECIFIED               25937       // A time datepart must be specified to convert a string to datetime. [,,,String,,]
#define SSCE_M_QP_INVALIDHOURTIMEPARTSPECIFIED      25938       // The specified hour in the time datepart is not valid. [,,,String,,]
#define SSCE_M_QP_INVALIDMINUTETIMEPARTSPECIFIED    25939       // The specified minute in the time datepart is not valid. [,,,String,,]
#define SSCE_M_QP_INVALIDSECONDIMEPARTSPECIFIED     25940       // The specified second in the time datepart is not valid. [,,,String,,]
#define SSCE_M_QP_INVALIDMSEDCONDIMEPARTSPECIFIED   25941       // The specified millisecond in the time datepart is not valid. [,,,String,,]
#define SSCE_M_QP_SYNTAXERRORINDATETIMESTRING       25942       // The specified date or time datepart is not valid. [,,,String,,]
#define SSCE_M_QP_FORMATERRORINDATETIMESTRING       25943       // The format of the specified date or time datepart is not valid. [,,,String,,]
#define SSCE_M_QP_INVALIDORDERINDATETIMESTRING      25944       // The order of the specified date or time datepart is not valid. [,,,String,,]
#define SSCE_M_QP_INVALID_ESCAPE_SEQUENCE           25945       // The escape sequence used in the LIKE predicate is not valid. [,,,,,]

//***** Don't add any more errors here, read the comments below and add appropriately
//
// Errors after this comment were added for v1.1 (PEBBLE) 
//

//Errors after this comment were added for v2.0 
//
#define SSCE_M_QP_INVALIDFUNCTIONARGNUM             25946       // The number of arguments specified for the function is not correct. [Minimum argument count, Maximum argument count,,Name of function(if known),,]
#define SSCE_M_QP_INVALIDCONVERSION                 25947       // The conversion is not supported. [,,,Type to convert from (if known), Type to convert to (if known),]
#define SSCE_M_QP_INVALIDFUNCTIONARGVALUE           25948       // The specified argument value for the function is not valid. [,,,Name of function(if known),,]
#define SSCE_M_QP_INCOMPATIBLE_TYPES                25949       // The expressions in UNION are not compatible. [Expression ordinal,,Type of left expression, Type of right expression,,]
#define SSCE_M_QP_MISSINGPARAMETER                  25950       // A parameter is missing. [Parameter ordinal,,,,,]
#define SSCE_M_QP_PARAMETERNOTALLOWED               25951       // A parameter is not allowed in this location. Ensure that the '@' sign is in a valid location or that parameters are valid at all in this SQL statement. [,,,,,]
// DEAD SSCE_M_QP_EMPTYFROMLIST                     25952       // At least one input table is required. [,,,,,]


// Errors after this comment were added for v3.0 
//
#define SSCE_M_QP_SIGNMISMATCH                      25953       // Expression evaluation caused a sign mismatch. [,,,,,]
#define SSCE_M_QP_CANNOTALTERCOLUMNWITHDEFAULT      25954       // Cannot alter column because it has a default value. [,,,Column Name,,]
#define SSCE_M_QP_BAD_BOOL_OP_TYPE                  25955       // The data type is not valid for the boolean operation. [,,,Data type (if known),Data type (if known),]

// Errors after this comment were added for v3.5
#define SSCE_M_QP_MISSINGPARAMETERTYPE              25956       // Parameter type has not been declared. [Parameter ordinal,,,Parameter name (if available),,]
#define SSCE_M_QP_LOB_IN_UNION_DISTINCT             25957       // The ntext and image data types can be used in select list of UNION only when ALL option is specified. [,,,,,]
#define SSCE_M_QP_OVERFLOW_IN_COUNT                 25958       // An overflow occurred in computing COUNT. COUNT can only be used when return values are less than 2^31. [,,,,,]

// --------------------------------------------------------------------------------------
//
// Query Storage Layer          26100 -- 26199
//  
#define SSCE_M_QP_BADTABLE                          26100       // The table name is not valid. [Token line number (if known),Token line offset (if known),,Table name,,]
// DEAD SSCE_M_QP_BADSCHEMAROWSET                   26101       // An error occurred while accessing the schema rowset. [,,,,,]
// DEAD SSCE_M_QP_TBLSCHEMAROWSET                   26102       // An error occurred while accessing the table schema rowset. [,,,Table name,,]

//***** Don't add any more errors here, read the comments below and add appropriately
//
// Errors after this comment were added for v1.1 (PEBBLE) 
//

// --------------------------------------------------------------------------------------
//
// Query Core                   26300 -- 26499
//  
// DEAD SSCE_M_QP_OLEDBERR                          26300       // OLE DB returned an error. [,,,,,]
// DEAD SSCE_M_QP_BADADT                            26301       // Cannot convert from the specified type. [,,,,,]
#define SSCE_M_QP_BADTYPE                           26302       // The specified data type is not valid. [,,,Data type (if known),,]
#define SSCE_M_QP_INTERNAL                          26303       // Internal error: A query processor error occurred. [,,,,,]
#define SSCE_M_QP_BADADTMETAINFO                    26305       // The DDL statement contains invalid parameters associated with the column definition. [,,,,,]
#define SSCE_M_QP_COERSIONERROR                     26306       // Data conversion failed. [OLE DB status value (if known),,,,,]
#define SSCE_M_QP_BADQUERY                          26307       // Syntax error in query. [,,,,,]
// DEAD SSCE_M_QP_ZEROLENGTHBINARYCONST             26308       // Zero-length binary constants are not supported in SQL Server Compact. [,,,,,]

// ***********************************************************************
// NOTE : Our error number range is limited to the SQL Server error number 
//        range, Which is 25000 to 30000. DO NOT ADD ANY ERRORS ABOVE OR
//        BELOW THIS RANGE!
// ***********************************************************************


// --------------------------------------------------------------------------------------
//
// SSCERelay                    30000 -- 30499 
//
//
// Errors after this comment were added for v1.1 (PEBBLE) 
//
// DEAD SSCE_M_WSANOTINITIALISED                    30000       // The socket cannot be initialized/uninitialized. [,,,,,]
// DEAD SSCE_M_WSAENETDOWN                          30001       // The network was unavailable, and the initialization/uninitialization of the socket failed. [,,,,,]
// DEAD SSCE_M_WSAEINPROGRESS                       30002       // The socket cannot be initialized/uninitialized. A blocking operation is currently executing. [,,,,,]
// DEAD SSCE_M_WSASYSNOTREADY                       30003       // The socket caninitialized/uninitialized. Either a driver is missing or multiple implementations are being used. [,,,,,]
// DEAD SSCE_M_WSAVERNOTSUPPORTED                   30004       // The socket cannot be initialized/uninitialized. The current version is not supported. [,,,,,]
// DEAD SSCE_M_WSAEPROCLIM                          30005       // The socket cannot be initialized/uninitialized. The limit on the number of processes using Windows Sockets has been reached. [,,,,,]
// DEAD SSCE_M_WSAEFAULT                            30006       // An address passed to the function is not valid. [,,,,,]
// DEAD SSCE_M_CREATEEVENTFAILED                    30007       // A create event failed. For more information, see HRESULT. [,,,,,]
// DEAD SSCE_M_SETEVENTFAILED                       30008       // A set event failed. For more information, see HRESULT. [,,,,,]
// DEAD SSCE_M_WAITFAILED                           30009       // A WAITFOR event failed. For more information, see HRESULT. [,,,,,]
// DEAD SSCE_M_INVALIDCLIENTPORT                    30010       // The specified desktop computer port is not valid. [,,,,,]
// DEAD SSCE_M_CONVERSIONTOUNICODEFAILED            30011       // The conversion of the server name to Unicode failed. [,,,,,]
// DEAD SSCE_M_INVALIDSERVERPORT                    30012       // The server port specified is not valid. [,,,,,]
// DEAD SSCE_M_FAILUREACCESSINGSYSTEMMENU           30013       // The system menu cannot be accessed. [,,,,,]
// DEAD SSCE_M_FAILURESETTINGREGISTRY               30014       // The registry keys cannot be set or created. [,,,,,]
// DEAD SSCE_M_INVALIDARGUMENT                      30015       // A parameter passed to SQL Server Compact Relay is not valid. [,,,,,]
// DEAD SSCE_M_MISSINGPORTORSERVER                  30017       // Either a port number is not valid or the server name is missing. [,,,,,]
// DEAD SSCE_M_SOCKETCALLFAILED                     30018       // The socket call failed. [,,,,,]
// DEAD SSCE_M_PORTALREADYINUSE                     30019       // The socket port is already in use on this device. [,,,,,]
// DEAD SSCE_M_BINDFAILED                           30020       // The bind failed. [,,,,,]
// DEAD SSCE_M_CANTRESOLVESERVERADDRESS             30021       // The server address cannot be resolved. [,,,,,]
// DEAD SSCE_M_LISTENFAILED                         30022       // The Listen function failed on a socket. [,,,,,]
// DEAD SSCE_M_ACCEPTFAILED                         30023       // An accept from the server failed. [,,,,,]
// DEAD SSCE_M_CONNECTFAILED                        30024       // A connection request to the server has failed. [,,,,,]
// DEAD SSCE_M_CANTCREATETHREAD                     30025       // A thread cannot be created due to system problems or lack of resources. [,,,,,]
// DEAD SSCE_M_SETKEEPALIVEFAILED                   30026       // Internal Error: Setting a socket option for keep alive failed. [,,,,,]
// DEAD SSCE_M_SETNAGLEFAILED                       30027       // Internal Error: Setting a socket option for the Nagle algorithm failed. [,,,,,]
// DEAD SSCE_M_RECEIVEFAILED                        30028       // The receive operation from the socket failed. [,,,,,]
// DEAD SSCE_M_SENDFAILED                           30029       // The send operation to the socket failed. [,,,,,]
// DEAD SSCE_M_INITARGSFAILED                       30030       // The initialization of arguments failed.[,,,,,]
// DEAD SSCE_M_WSASTARTUPFAILED                     30031       // The WSAStartup function failed during socket initialization. [,,,,,]
// DEAD SSCE_M_WSACLEANUPFAILED                     30032       // The WSACleanup function failed during a cleanup operation. [,,,,,]
// DEAD SSCE_M_INVALIDSERVERNAME                    30033       // The server name is either not valid or not specified. [,,,,,]
// DEAD SSCE_M_CONNECTIONRESET                      30034       // The socket connection was reset. [,,,,,]

// --------------------------------------------------------------------------------------
//
// Miscellaneous                    39500 -- 39999 
//
//
// Errors after this comment were added for v1.1 (PEBBLE) 
//
// DEAD SSCE_M_EVALUATIONCOPYEXPIRED                39999       // The evaluation copy has expired. [,,,,,]

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
//                              O L E D B   E R R O R   C O D E S
//
//
#define DB_E_BADACCESSORHANDLE                  (HRESULT)0x80040E00L    // Accessor is invalid. [,,,,,]
#define DB_E_ROWLIMITEXCEEDED                   (HRESULT)0x80040E01L    // Row could not be inserted into the rowset without exceeding provider's maximum number of active rows. [,,,,,]
#define DB_E_READONLYACCESSOR                   (HRESULT)0x80040E02L    // Accessor is read-only. Operation failed. [,,,,,]
#define DB_E_SCHEMAVIOLATION                    (HRESULT)0x80040E03L    // Values violate the database schema. [,,,,,]
#define DB_E_BADROWHANDLE                       (HRESULT)0x80040E04L    // Row handle is invalid. [,,,,,]
#define DB_E_OBJECTOPEN                         (HRESULT)0x80040E05L    // Object was open. [,,,,,]
#define DB_E_BADCHAPTER                         (HRESULT)0x80040E06L    // Invalid chapter. [,,,,,]
#define DB_E_CANTCONVERTVALUE                   (HRESULT)0x80040E07L    // A literal value in the command could not be converted to the correct type due to a reason other than data overflow. [,,,,,]
#define DB_E_BADBINDINFO                        (HRESULT)0x80040E08L    // Invalid binding info. [,,,,,]
#define DB_SEC_E_PERMISSIONDENIED               (HRESULT)0x80040E09L    // Permission denied. [,,,,,]
#define DB_E_NOTAREFERENCECOLUMN                (HRESULT)0x80040E0AL    // Specified column does not contain bookmarks or chapters. [,,,,,]
#define DB_E_LIMITREJECTED                      (HRESULT)0x80040E0BL    // Some cost limits were rejected. [,,,,,]
#define DB_E_NOCOMMAND                          (HRESULT)0x80040E0CL    // No command has been set for the command object. [,,,,,]
#define DB_E_COSTLIMIT                          (HRESULT)0x80040E0DL    // Unable to find a query plan within the given cost limit. [,,,,,]
#define DB_E_BADBOOKMARK                        (HRESULT)0x80040E0EL    // Invalid bookmark. [,,,,,]
#define DB_E_BADLOCKMODE                        (HRESULT)0x80040E0FL    // Invalid lock mode. [,,,,,]
#define DB_E_PARAMNOTOPTIONAL                   (HRESULT)0x80040E10L    // No value given for one or more required parameters. [,,,,,]
#define DB_E_BADCOLUMNID                        (HRESULT)0x80040E11L    // Invalid column ID. [,,,,,]
#define DB_E_BADRATIO                           (HRESULT)0x80040E12L    // Invalid ratio. [,,,,,]
#define DB_E_BADVALUES                          (HRESULT)0x80040E13L    // Invalid value. [,,,,,]
#define DB_E_ERRORSINCOMMAND                    (HRESULT)0x80040E14L    // The command contained one or more errors. [,,,,,]
#define DB_E_CANTCANCEL                         (HRESULT)0x80040E15L    // The executing command cannot be canceled. [,,,,,]
#define DB_E_DIALECTNOTSUPPORTED                (HRESULT)0x80040E16L    // The provider does not support the specified dialect. [,,,,,]
#define DB_E_DUPLICATEDATASOURCE                (HRESULT)0x80040E17L    // A data source with the specified name already exists. [,,,,,]
#define DB_E_CANNOTRESTART                      (HRESULT)0x80040E18L    // The rowset was built over a live data feed and cannot be restarted. [,,,,,]
#define DB_E_NOTFOUND                           (HRESULT)0x80040E19L    // No key matching the described characteristics could be found within the current range. [,,,,,]
#define DB_E_NEWLYINSERTED                      (HRESULT)0x80040E1BL    // The provider is unable to determine identity for newly inserted rows. [,,,,,]
#define DB_E_CANNOTFREE                         (HRESULT)0x80040E1AL    // Ownership of this tree has been given to the provider. [,,,,,]
#define DB_E_GOALREJECTED                       (HRESULT)0x80040E1CL    // No nonzero weights specified for any goals supported, so goal was rejected; current goal was not changed. [,,,,,]
#define DB_E_UNSUPPORTEDCONVERSION              (HRESULT)0x80040E1DL    // Requested conversion is not supported. [,,,,,]
#define DB_E_BADSTARTPOSITION                   (HRESULT)0x80040E1EL    // lRowsOffset would position you past either end of the rowset, regardless of the cRows value specified; cRowsObtained is 0. [,,,,,]
// DEAD DB_E_NOQUERY                            (HRESULT)0x80040E1FL    // Information was requested for a query, and the query was not set. [,,,,,]
#define DB_E_NOTREENTRANT                       (HRESULT)0x80040E20L    // Provider called a method from IRowsetNotify in the consumer and the method has not yet returned. [,,,,,]
#define DB_E_ERRORSOCCURRED                     (HRESULT)0x80040E21L    // Errors occurred. [,,,,,]
#define DB_E_NOAGGREGATION                      (HRESULT)0x80040E22L    // A non-NULL controlling IUnknown was specified and the object being created does not support aggregation. [,,,,,]
#define DB_E_DELETEDROW                         (HRESULT)0x80040E23L    // The current row was deleted. [,,,,,]
#define DB_E_CANTFETCHBACKWARDS                 (HRESULT)0x80040E24L    // The rowset does not support fetching backwards. [,,,,,]
#define DB_E_ROWSNOTRELEASED                    (HRESULT)0x80040E25L    // All HROWs must be released before new ones can be obtained. [,,,,,]
#define DB_E_BADSTORAGEFLAG                     (HRESULT)0x80040E26L    // One of the specified storage flags was not supported. [,,,,,]
#define DB_E_BADCOMPAREOP                       (HRESULT)0x80040E27L    // The comparison operator was invalid. [,,,,,]
#define DB_E_BADSTATUSVALUE                     (HRESULT)0x80040E28L    // The specified status flag was neither DBCOLUMNSTATUS_OK nor DBCOLUMNSTATUS_ISNULL. [,,,,,]
#define DB_E_CANTSCROLLBACKWARDS                (HRESULT)0x80040E29L    // The rowset cannot scroll backwards. [,,,,,]
#define DB_E_BADREGIONHANDLE                    (HRESULT)0x80040E2AL    // Invalid region handle. [,,,,,]
#define DB_E_NONCONTIGUOUSRANGE                 (HRESULT)0x80040E2BL    // The specified set of rows was not contiguous to or overlapping the rows in the specified watch region. [,,,,,]
#define DB_E_INVALIDTRANSITION                  (HRESULT)0x80040E2CL    // A transition from ALL* to MOVE* or EXTEND* was specified. [,,,,,]
#define DB_E_NOTASUBREGION                      (HRESULT)0x80040E2DL    // The specified region is not a proper subregion of the region identified by the given watch region handle. [,,,,,]
#define DB_E_MULTIPLESTATEMENTS                 (HRESULT)0x80040E2EL    // The provider does not support multi-statement commands. [,,,,,]
#define DB_E_INTEGRITYVIOLATION                 (HRESULT)0x80040E2FL    // A specified value violated the integrity constraints for a column or table. [,,,,,]
#define DB_E_BADTYPENAME                        (HRESULT)0x80040E30L    // The given type name was unrecognized. [,,,,,]
#define DB_E_ABORTLIMITREACHED                  (HRESULT)0x80040E31L    // Execution aborted because a resource limit has been reached; no results have been returned. [,,,,,]
#define DB_E_ROWSETINCOMMAND                    (HRESULT)0x80040E32L    // Cannot clone a command object whose command tree contains a rowset or rowsets. [,,,,,]
#define DB_E_CANTTRANSLATE                      (HRESULT)0x80040E33L    // Cannot represent the current tree as text. [,,,,,]
#define DB_E_DUPLICATEINDEXID                   (HRESULT)0x80040E34L    // The specified index already exists. [,,,,,]
#define DB_E_NOINDEX                            (HRESULT)0x80040E35L    // The specified index does not exist. [,,,,,]
#define DB_E_INDEXINUSE                         (HRESULT)0x80040E36L    // The specified index was in use. [,,,,,]
#define DB_E_NOTABLE                            (HRESULT)0x80040E37L    // The specified table does not exist. [,,,,,]
#define DB_E_CONCURRENCYVIOLATION               (HRESULT)0x80040E38L    // The rowset was using optimistic concurrency and the value of a column has been changed since it was last read. [,,,,,]
#define DB_E_BADCOPY                            (HRESULT)0x80040E39L    // Errors were detected during the copy. [,,,,,]
#define DB_E_BADPRECISION                       (HRESULT)0x80040E3AL    // A specified precision was invalid. [,,,,,]
#define DB_E_BADSCALE                           (HRESULT)0x80040E3BL    // A specified scale was invalid. [,,,,,]
#define DB_E_BADTABLEID                         (HRESULT)0x80040E3CL    // Invalid table ID. [,,,,,]
#define DB_E_BADTYPE                            (HRESULT)0x80040E3DL    // A specified type was invalid. [,,,,,]
#define DB_E_DUPLICATECOLUMNID                  (HRESULT)0x80040E3EL    // A column ID occurred more than once in the specification. [,,,,,]
#define DB_E_DUPLICATETABLEID                   (HRESULT)0x80040E3FL    // The specified table already exists. [,,,,,]
#define DB_E_TABLEINUSE                         (HRESULT)0x80040E40L    // The specified table was in use. [,,,,,]
#define DB_E_NOLOCALE                           (HRESULT)0x80040E41L    // The specified locale ID was not supported. [,,,,,]
#define DB_E_BADRECORDNUM                       (HRESULT)0x80040E42L    // The specified record number is invalid. [,,,,,]
#define DB_E_BOOKMARKSKIPPED                    (HRESULT)0x80040E43L    // Although the bookmark was validly formed, no row could be found to match it. [,,,,,]
#define DB_E_BADPROPERTYVALUE                   (HRESULT)0x80040E44L    // The value of a property was invalid. [,,,,,]
#define DB_E_INVALID                            (HRESULT)0x80040E45L    // The rowset was not chaptered. [,,,,,]
#define DB_E_BADACCESSORFLAGS                   (HRESULT)0x80040E46L    // Invalid accessor. [,,,,,]
#define DB_E_BADSTORAGEFLAGS                    (HRESULT)0x80040E47L    // Invalid storage flags. [,,,,,]
#define DB_E_BYREFACCESSORNOTSUPPORTED          (HRESULT)0x80040E48L    // By-ref accessors are not supported by this provider. [,,,,,]
#define DB_E_NULLACCESSORNOTSUPPORTED           (HRESULT)0x80040E49L    // Null accessors are not supported by this provider. [,,,,,]
#define DB_E_NOTPREPARED                        (HRESULT)0x80040E4AL    // The command was not prepared. [,,,,,]
#define DB_E_BADACCESSORTYPE                    (HRESULT)0x80040E4BL    // The specified accessor was not a parameter accessor. [,,,,,]
#define DB_E_WRITEONLYACCESSOR                  (HRESULT)0x80040E4CL    // The given accessor was write-only. [,,,,,]
#define DB_SEC_E_AUTH_FAILED                    (HRESULT)0x80040E4DL    // Authentication failed. [,,,,,]
#define DB_E_CANCELED                           (HRESULT)0x80040E4EL    // The change was canceled during notification; no columns are changed. [,,,,,]
#define DB_E_CHAPTERNOTRELEASED                 (HRESULT)0x80040E4FL    // The rowset was single-chaptered and the chapter was not released. [,,,,,]
#define DB_E_BADSOURCEHANDLE                    (HRESULT)0x80040E50L    // Invalid source handle. [,,,,,]
#define DB_E_PARAMUNAVAILABLE                   (HRESULT)0x80040E51L    // The provider cannot derive parameter info and SetParameterInfo has not been called. [,,,,,]
#define DB_E_ALREADYINITIALIZED                 (HRESULT)0x80040E52L    // The data source object is already initialized. [,,,,,]
#define DB_E_NOTSUPPORTED                       (HRESULT)0x80040E53L    // The provider does not support this method. [,,,,,]
#define DB_E_MAXPENDCHANGESEXCEEDED             (HRESULT)0x80040E54L    // The number of rows with pending changes has exceeded the set limit. [,,,,,]
#define DB_E_BADORDINAL                         (HRESULT)0x80040E55L    // The specified column did not exist. [,,,,,]
#define DB_E_PENDINGCHANGES                     (HRESULT)0x80040E56L    // There are pending changes on a row with a reference count of zero. [,,,,,]
#define DB_E_DATAOVERFLOW                       (HRESULT)0x80040E57L    // A literal value in the command overflowed the range of the type of the associated column. [,,,,,]
#define DB_E_BADHRESULT                         (HRESULT)0x80040E58L    // The supplied HRESULT was invalid. [,,,,,]
#define DB_E_BADLOOKUPID                        (HRESULT)0x80040E59L    // The supplied LookupID was invalid. [,,,,,]
#define DB_E_BADDYNAMICERRORID                  (HRESULT)0x80040E5AL    // The supplied DynamicErrorID was invalid. [,,,,,]
#define DB_E_PENDINGINSERT                      (HRESULT)0x80040E5BL    // Unable to get visible data for a newly-inserted row that has not yet been updated. [,,,,,]
#define DB_E_BADCONVERTFLAG                     (HRESULT)0x80040E5CL    // Invalid conversion flag. [,,,,,]
#define DB_E_BADPARAMETERNAME                   (HRESULT)0x80040E5DL    // The given parameter name was unrecognized. [,,,,,]
#define DB_E_MULTIPLESTORAGE                    (HRESULT)0x80040E5EL    // Multiple storage objects can not be open simultaneously. [,,,,,]
#define DB_E_CANTFILTER                         (HRESULT)0x80040E5FL    // The requested filter could not be opened. [,,,,,]
#define DB_E_CANTORDER                          (HRESULT)0x80040E60L    // The requested order could not be opened. [,,,,,]
#define DB_E_NOCOLUMN                           (HRESULT)0x80040E65L    // The supplied columnID was invalid. [,,,,,]
#define DB_E_COMMANDNOTPERSISTED                (HRESULT)0x80040E67L    // The supplied command does not have a DBID. [,,,,,]
#define DB_E_DUPLICATEID                        (HRESULT)0x80040E68L    // The supplied DBID already exists. [,,,,,]
#define DB_E_OBJECTCREATIONLIMITREACHED         (HRESULT)0x80040E69L    // The maximum number of Sessions supported by the provider has already been created. The consumer must release one or more currently held Sessions before obtaining a new Session Object. [,,,,,]
#define DB_E_BADINDEXID                         (HRESULT)0x80040E72L    // The index ID is invalid. [,,,,,]
#define DB_E_BADINITSTRING                      (HRESULT)0x80040E73L    // The initialization string specified does not conform to specificiation. [,,,,,]
#define DB_E_NOPROVIDERSREGISTERED              (HRESULT)0x80040E74L    // The OLE DB root enumerator did not return any providers that matched any of the SOURCES_TYPEs requested. [,,,,,]
#define DB_E_MISMATCHEDPROVIDER                 (HRESULT)0x80040E75L    // The initialization string specifies a provider which does not match the currently active provider. [,,,,,]
#define DB_E_BADCOMMANDID                       (HRESULT)0x80040E76L    // The specified DBID is invalid. [,,,,,]
#define SEC_E_BADTRUSTEEID                      (HRESULT)0x80040E6AL    // Invalid trustee value. [,,,,,]
#define SEC_E_NOTRUSTEEID                       (HRESULT)0x80040E6BL    // The trustee is not for the current data source. [,,,,,]
#define SEC_E_NOMEMBERSHIPSUPPORT               (HRESULT)0x80040E6CL    // The trustee does not support memberships/collections. [,,,,,]
#define SEC_E_INVALIDOBJECT                     (HRESULT)0x80040E6DL    // The object is invalid or unknown to the provider. [,,,,,]
#define SEC_E_NOOWNER                           (HRESULT)0x80040E6EL    // No owner exists for the object. [,,,,,]
#define SEC_E_INVALIDACCESSENTRYLIST            (HRESULT)0x80040E6FL    // The access entry list supplied is invalid. [,,,,,]
#define SEC_E_INVALIDOWNER                      (HRESULT)0x80040E70L    // The trustee supplied as owner is invalid or unknown to the provider. [,,,,,]
#define SEC_E_INVALIDACCESSENTRY                (HRESULT)0x80040E71L    // The permission supplied in the access entry list is invalid. [,,,,,]
#define DB_E_BADCONSTRAINTTYPE                  (HRESULT)0x80040E77L    // The ConstraintType was invalid or not supported by the provider. [,,,,,]
#define DB_E_BADCONSTRAINTFORM                  (HRESULT)0x80040E78L    // The ConstraintType was not DBCONSTRAINTTYPE_FOREIGNKEY and cForeignKeyColumns was not zero. [,,,,,]
#define DB_E_BADDEFERRABILITY                   (HRESULT)0x80040E79L    // The Deferrability was invalid or the value was not supported by the provider. [,,,,,]
#define DB_E_BADMATCHTYPE                       (HRESULT)0x80040E80L    // The MatchType was invalid or the value was not supported by the provider. [,,,,,]
#define DB_E_BADUPDATEDELETERULE                (HRESULT)0x80040E8AL    // The UpdateRule or DeleteRule was invalid or the value was not supported by the provider. [,,,,,]
#define DB_E_BADCONSTRAINTID                    (HRESULT)0x80040E8BL    // Constraint ID is invalid. [,,,,,]
#define DB_E_BADCOMMANDFLAGS                    (HRESULT)0x80040E8CL    // The dwFlags was invalid. [,,,,,]
#define DB_E_OBJECTMISMATCH                     (HRESULT)0x80040E8DL    // The rguidColumnType pointed to a GUID that does not match the object type of this column or this column was not set. [,,,,,]
#define DB_E_NOSOURCEOBJECT                     (HRESULT)0x80040E91L    // There is no source row. [,,,,,]
#define DB_E_RESOURCELOCKED                     (HRESULT)0x80040E92L    // The OLE DB object represented by this URL is locked by one or more other processes. [,,,,,]
#define DB_E_NOTCOLLECTION                      (HRESULT)0x80040E93L    // The client requested an object type that is only valid for a collection. [,,,,,]
#define DB_E_READONLY                           (HRESULT)0x80040E94L    // The caller requested write access to a read-only object. [,,,,,]
#define DB_E_ASYNCNOTSUPPORTED                  (HRESULT)0x80040E95L    // The provider does not support asynchronous operations. [,,,,,]
#define DB_E_CANNOTCONNECT                      (HRESULT)0x80040E96L    // The provider could not connect to the server for this object. [,,,,,]
#define DB_E_TIMEOUT                            (HRESULT)0x80040E97L    // The attempt to bind to the object timed out. [,,,,,]
#define DB_E_RESOURCEEXISTS                     (HRESULT)0x80040E98L    // The provider was unable to create an object at this URL because an object named by this URL already exists. [,,,,,]
#define DB_E_RESOURCEOUTOFSCOPE                 (HRESULT)0x80040E8EL    // The requested URL was out-of-scope. [,,,,,]
#define DB_E_DROPRESTRICTED                     (HRESULT)0x80040E90L    // Column or constraint could not be dropped because it is referenced by a dependent view or constraint. [,,,,,]
#define DB_E_DUPLICATECONSTRAINTID              (HRESULT)0x80040E99L    // Constraint already exists. [,,,,,]
#define DB_E_OUTOFSPACE                         (HRESULT)0x80040E9AL    // Object cannot be created at this URL because the server is out of physical storage. [,,,,,]
#define DB_E_NS_BOUNDARYDESCRIPTOR              (HRESULT)0x80040EA0L	// Failed to create boundary descriptor. [,,,,,]
#define DB_E_NS_OPEN                            (HRESULT)0x80040EA1L	// Failed to create or open namespace. [,,,,,]
#define DB_E_NS_ALREADY_EXISTS                  (HRESULT)0x80040EA2L	// Namespace already exists. [,,,,,]
#define DB_S_ROWLIMITEXCEEDED                   (HRESULT)0x00040EC0L    // Fetching requested number of rows would have exceeded total number of active rows supported by the rowset. [,,,,,]
#define DB_S_COLUMNTYPEMISMATCH                 (HRESULT)0x00040EC1L    // One or more column types are incompatible; conversion errors will occur during copying. [,,,,,]
#define DB_S_TYPEINFOOVERRIDDEN                 (HRESULT)0x00040EC2L    // Parameter type information has been overridden by caller. [,,,,,]
#define DB_S_BOOKMARKSKIPPED                    (HRESULT)0x00040EC3L    // Skipped bookmark for deleted or non-member row. [,,,,,]
#define DB_S_NONEXTROWSET                       (HRESULT)0x00040EC5L    // There are no more rowsets. [,,,,,]
#define DB_S_ENDOFROWSET                        (HRESULT)0x00040EC6L    // Reached start or end of rowset or chapter. [,,,,,]
#define DB_S_COMMANDREEXECUTED                  (HRESULT)0x00040EC7L    // The provider re-executed the command. [,,,,,]
#define DB_S_BUFFERFULL                         (HRESULT)0x00040EC8L    // Variable data buffer full. [,,,,,]
#define DB_S_NORESULT                           (HRESULT)0x00040EC9L    // There are no more results. [,,,,,]
#define DB_S_CANTRELEASE                        (HRESULT)0x00040ECAL    // Server cannot release or downgrade a lock until the end of the transaction. [,,,,,]
#define DB_S_GOALCHANGED                        (HRESULT)0x00040ECBL    // Specified weight was not supported or exceeded the supported limit and was set to 0 or the supported limit. [,,,,,]
#define DB_S_UNWANTEDOPERATION                  (HRESULT)0x00040ECCL    // Consumer is uninterested in receiving further notification calls for this reason. [,,,,,]
#define DB_S_DIALECTIGNORED                     (HRESULT)0x00040ECDL    // Input dialect was ignored and text was returned in different dialect. [,,,,,]
#define DB_S_UNWANTEDPHASE                      (HRESULT)0x00040ECEL    // Consumer is uninterested in receiving further notification calls for this phase. [,,,,,]
#define DB_S_UNWANTEDREASON                     (HRESULT)0x00040ECFL    // Consumer is uninterested in receiving further notification calls for this reason. [,,,,,]
#define DB_S_ASYNCHRONOUS                       (HRESULT)0x00040ED0L    // The operation is being processed asynchronously. [,,,,,]
#define DB_S_COLUMNSCHANGED                     (HRESULT)0x00040ED1L    // In order to reposition to the start of the rowset, the provider had to reexecute the query; either the order of the columns changed or columns were added to or removed from the rowset. [,,,,,]
#define DB_S_ERRORSRETURNED                     (HRESULT)0x00040ED2L    // The method had some errors; errors have been returned in the error array. [,,,,,]
#define DB_S_BADROWHANDLE                       (HRESULT)0x00040ED3L    // Invalid row handle. [,,,,,]
#define DB_S_DELETEDROW                         (HRESULT)0x00040ED4L    // A given HROW referred to a hard-deleted row. [,,,,,]
#define DB_S_TOOMANYCHANGES                     (HRESULT)0x00040ED5L    // The provider was unable to keep track of all the changes; the client must refetch the data associated with the watch region using another method. [,,,,,]
#define DB_S_STOPLIMITREACHED                   (HRESULT)0x00040ED6L    // Execution stopped because a resource limit has been reached; results obtained so far have been returned but execution cannot be resumed. [,,,,,]
#define DB_S_LOCKUPGRADED                       (HRESULT)0x00040ED8L    // A lock was upgraded from the value specified. [,,,,,]
#define DB_S_PROPERTIESCHANGED                  (HRESULT)0x00040ED9L    // One or more properties were changed as allowed by provider. [,,,,,]
#define DB_S_ERRORSOCCURRED                     (HRESULT)0x00040EDAL    // Errors occurred. [,,,,,]
#define DB_S_PARAMUNAVAILABLE                   (HRESULT)0x00040EDBL    // A specified parameter was invalid. [,,,,,]
#define DB_S_MULTIPLECHANGES                    (HRESULT)0x00040EDCL    // Updating this row caused more than one row to be updated in the data source. [,,,,,]
#define DB_S_NOTSINGLETON                       (HRESULT)0x00040ED7L    // The bind failed because the provider was unable to satisfy all of the bind flags or properties. [,,,,,]
#define DB_S_NOROWSPECIFICCOLUMNS               (HRESULT)0x00040EDDL    // The row has no row-specific columns. [,,,,,]

#endif // of ! __SQLCE_ERR_H__

