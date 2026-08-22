---
id: 229
title: "HOWTO: Determine hMailServer database password"
slug: howto_determine_mysql_password
parent: howtos
index: 0
is_book: false
updated: 2009-11-02
---

If you are using the built-in version that comes with hMailServer, you might not know your password. To determine the password, follow these steps:

1. Run the script hMailServer/Addons/Utilities/DecryptBlowfish.vbs.
2. Enter your main hMailServer Administrator password. This is the password you specified during installation. Click OK.
3. Open up hMailserver.ini, located in your Windows directory or hMailServer bin directory.
4. Copy the database password from hMailServer.ini to the "Enter encrypted password" dialog.
5. Click OK in DecryptBlowfish to view your password
