---
title: "PHPWebAdmin setup problems"
slug: ts_setup_phpwebadmin
parent: troubleshooting_tips
index: 0
is_book: false
---

## Overview

This page describes common set up mistakes when installing PHPWebAdmin.

## Basic set up

- When you upgrade your hMailServer installation to a later version, make sure to copy the latest PHPWebAdmin from the hMailServer installation directory to your web directory. Not doing this may prevent PHPWebAdmin from working.

## Settings in config.php

- When you specify paths and URLs in the config file, it is very important that you use the forward slash /, and not the backward slash , as folder separator. It is also very important that you end every path and every URL with /.

## Settings in PHP.ini

### Editing the wrong PHP.ini

A common cause of trouble is to edit PHP.ini in the wrong directory. When a PHP script is executed, no error is reported if the file PHP.ini cannot be found. Sometimes users edits a PHP.ini in a specific directory and later it turns out that PHP tried to read the file from another directory. To check which PHP.ini is used, follow these steps:

<ol>
    <li>Create a new file called test.php in your webroot.</li>
    <li>Add the following code to the file:<br>
    <span class="Code">&lt;?php phpinfo();?&gt;</span></li>
    <li>Go to the PHP script using your web browser (http://www.example.com/test.php)</li>
    <li>Check the value of the <em>Configuration File (php.ini) Path</em> setting near the top.</li>
</ol>

### Error reporting isn't enabled

<p>If you get a blank page when visiting PHPWebAdmin, it's likely that error reporting is not enabled in php.ini. Enable error reporting to make it easier to troubleshoot errors. Make sure that <span class="Code">error_reporting</span> is uncommented and set to E_ALL. If you aren't running any other PHP scripts on the server, set <span class="Code">display_errors</span> to On. If you are running other PHP scripts, you probably don't want errors to displayed in your web browser and need to check the PHP log for errors instead.</p>

### DCOM not being enabled

<p>PHPWebAdmin uses the hMailServer COM API to access the settings and your objects in your hMailServer installation. For this to work, it may be required to enable DCOM in the PHP configuration. Make sure that <span class="Code">com.allow_dcom = true</span> exists and is uncommented in php.ini.</p>

<p>For php version 5.3.15 / 5.4.5 and higher the COM API has been disabled as a module. The error probably will be <span class="Code">PHP Fatal error:  Class 'COM' not found in \phpwebadmin\initialize.php on line xx</span>. Make sure you edit the php.ini file has been edited with the value <span class="Code">extension=php_com_dotnet.dll</span> in the extensions.</p>

### Session directory not created

<p>If PHP is configured to use sessions, you must ensure that <span class="Code">session.save_path</span> is uncommented and has a valid value. You must also check that your web browser has write permission in this directory. For example, if you are using Microsoft IIS, the user IUSR_[computername] needs write-permissions on this directory.</p>

## Other permission issues

- If you are using Microsoft SQL Server for hMailServer, you may need to give the user IUSR_[computername] access to the hMailServer database.
- If you are using IIS, or Apache under a dedicated user, you must [give the service account access](?page=howto_dcom_permissions) to the hMailServer COM library.

 
