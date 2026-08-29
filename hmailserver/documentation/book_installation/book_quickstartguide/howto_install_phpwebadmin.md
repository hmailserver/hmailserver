---
title: "Installing PHPWebAdmin"
slug: howto_install_phpwebadmin
parent: book_quickstartguide
index: 20
is_book: false
---

## Overview

This page describes how to install hMailServer WebAdmin, the PHP administration front-end for hMailServer.

### Prerequisites

<ul>
    <li>Microsoft IIS (or any other PHP compatible web server on Windows)</li>
    <li>PHP x86 versions <strong class="text-strong">starting with</strong> 7.4.0 but earlier than 8.0.0</li>
</ul>

While hMailServer may work with older versions of PHP, it has not been tested.  

The current code is incompatible with PHP versions starting with 8.0.0 due to function deprecation.

### Step 1: Configure PHP

A clean installation of PHP has many features and extensions disabled. Some of these needs to be enabled to be able to use PHPWebAdmin. If you have just installed PHP, you need to copy php.ini-production to php.ini and make the following adjustments. 

#### Enable COM support

PHP accesses hMailServer API using COM. COM support is not enabled by default in PHP.

To enable it, locate the *Dynamic Extensions* section in php.ini, and add the following line:

```
extension=php_com_dotnet.dll
```

#### Enable Session support

In PHP.ini, change the setting session.save_path to a folder where you want PHP to store session files. For example:

```
session.save_path = "C:\PHP\sessions"
```

Note: The directory must exist, and be writeable by the web server process..  

 

### Step 2: Configure hMailServer DCOM

PHP needs to have access to the hMailServer COM API. To do this, you need to [configure DCOM permissions](?page=howto_dcom_permissions).

### Step 3: Copy the PHPWebAdmin files

<p>Copy PHPWebAdmin from the hMailServer directory to your web root directory. The web root differs between web servers so check your web server's documentation if you're unsure where your web root is located. Copy the entire PHPWebAdmin from <span class="Code">C:\Program Files\hMailServer</span>  to your web root. Example: If your web root is <span class="Code">C:/wwwroot,</span> copy PHPWebAdmin to <span class="Code">C:\wwwroot</span>, so that you get <span class="Code">C:\wwwroot\PHPWebAdmin</span>.  <br>
<br>
Alternately, you can use the PHPWebAdmin folder where hMailServer installed, you will need to point your webserver to that folder and set permissions as above.</p>

### Step 4: Configure PHPWebAdmin

Go to the PHPWebAdmin directory in the web root.

<ol>
    <li>Create a copy of the file named <span class="Code">config-dist.php</span> and give it the name <span class="Code">config.php</span>.</li>
    <li>The file <span class="Code">config.php</span> contains the basic settings for PHPWebAdmin and needs to be modified to correctly adjust it for your system:<br>
    <br>
    <ul>
        <li>Set the value of rooturl to the URL where the WebAdmin will be accessed.<br>
        <br>
        Example:<br>
        <span class="Code">$hmail_config['rooturl'] = "http://localhost/PHWebAdmin/"; </span></li>
    </ul>
    </li>
</ol>

### Step 4: Use it

Point your web browser to http://localhost/PHPWebAdmin and log in with your hMailServer username and password.

When you upgrade your hMailServer installation to a later version, make sure to copy the latest WebAdmin from the hMailServer installation directory to your web directory. If you do not do this, you will get an error message when trying to access WebAdmin.

### Common configuration errors

There's a few common set up mistakes. Read more in the [troubleshooting guide](?page=ts_setup_phpwebadmin).

 

 
