---
id: 107
title: "DCOM permissions"
slug: howto_dcom_permissions
parent: howtos
index: 0
is_book: false
updated: 2015-12-03
---

## Overview

By default, IIS6 is not allowed to connect to hMailServer because of the Distributed COM permissions that Windows uses. Also, Apache run as a user account with restricted permissions is not allowed to connect by default.

## Steps

Follow these steps to give IIS or Apache the required permissions to connect to hMailServer using DCOM.

1. Start DCOM config by selecting Start, Run, enter dcomcnfg.exe and press OK.
2. In the Component Services program, expand the Component Services folder
3. Expand down to Computers | My Computer | DCOM Config
4. Right-click on hMailServer and select properties
5. Select the Security tab
6. Under "Launch and Activation Permissions", select Customize and click on Edit
7. Under "Group or user names", click Add
8. For IIS6: add the built-in anonymous IIS user account
9. For IIS7 >> add the builtin IIS_IUSRS user account
10. For Apache: add the Apache user account
11. Set the Local Launch and Local Activation permissions for this user to Allow

 
