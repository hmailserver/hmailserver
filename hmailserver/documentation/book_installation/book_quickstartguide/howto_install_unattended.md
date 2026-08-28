---
title: "Unattended installation"
slug: howto_install_unattended
parent: book_quickstartguide
index: 5
is_book: false
---

## Overview

The setup program can run without user interaction, which is useful when you
install or upgrade hMailServer using a script or a deployment tool.

```
hMailServer-<version>.exe /VERYSILENT /SUPPRESSMSGBOXES /ADMINPASSWORD=<password>
```

### Parameters

The setup program is built with Inno Setup, so it accepts the standard Inno
Setup parameters such as `/SILENT`, `/VERYSILENT`, `/SUPPRESSMSGBOXES` and
`/LOG`. In addition, hMailServer accepts the following:

<table width="600" cellspacing="1" cellpadding="1" border="1">
    <tbody>
        <tr>
            <td><strong>Parameter</strong></td>
            <td><strong>Description</strong></td>
        </tr>
        <tr>
            <td>/ADMINPASSWORD=&lt;password&gt;</td>
            <td>The hMailServer administrator password. On a new installation the password is set to this value. On an upgrade it is used to log on so that the database can be upgraded.</td>
        </tr>
        <tr>
            <td>/useinternaldbms=false</td>
            <td>Don't use the built-in database. Use this if hMailServer should use an external database server. Defaults to true.</td>
        </tr>
    </tbody>
</table>

### The administrator password

The setup program upgrades the hMailServer database at the end of the
installation, and it must log on as the administrator to do that. During an
unattended installation there is no dialog to ask for the password, so if an
administrator password is set you must supply it with `/ADMINPASSWORD`.

If the password is missing or wrong, the database is not upgraded. hMailServer
then refuses to open any ports, and writes an error to the error log:

```
The database is too old for this version of hMailServer.
```

You can recover from this by running the database updater manually:

```
"C:\Program Files\hMailServer\Bin\DBUpdater.exe" /silent password:<password>
```

### Checking the result

The setup program exits with a non-zero exit code if the installation fails,
including when the database upgrade fails. Always check the exit code - a
service which is running is not proof that the installation succeeded.

Add `/LOG="C:\setup.log"` to get a log file describing what setup did and why
it failed.
