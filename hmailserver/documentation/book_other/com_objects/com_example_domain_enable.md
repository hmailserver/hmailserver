---
title: "API example: Enabling a domain"
slug: com_example_domain_enable
parent: com_objects
index: 0
is_book: false
---

This example shows how to enable an existing domain in your hMailServer installation. The script is written in VBA.

# Enabling a domain

``

```
Dim obApp
   Set obApp = CreateObject("hMailServer.Application")

   ' Authenticate. Without doing this, we won't have permission
   ' to change any server settings or add any objects to the
   ' installation.   
   Call obApp.Authenticate("Administrator", "your-main-hmailserver-password")
   
   ' Locate the domain we want to add the account to
   Dim obDomain
   Set obDomain = obApp.Domains.ItemByName("example.com")

   obDomain.Active = True
   obDomain.Save
```
