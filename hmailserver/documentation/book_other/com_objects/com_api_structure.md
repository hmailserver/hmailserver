---
id: 228
title: "COM API: API structure"
slug: com_api_structure
parent: com_objects
index: 0
is_book: false
updated: 2007-05-19
---

# Overview

In most aspects, the hMailServer behaves as any other COM API. However, there are a few things which are good to know before starting to write code which utilizes the hMailServer COM API.

## Accessing an object

Regardless of which hMailServer object you want to access, you need to navigate to it via the hMailServer.Application object. For example, to access a specific domain, you need to create an instance of the hMailServer.Application object, retrieve a collection of all domains using the Domains property and then locate the specific domain. It is not possible to create an instance of the hMailServer.Domains collection to retrieve the specific domain. The reason for this behavior is the COM API authentication mechanisms.

## Exception handling

The hMailServer COM API throws exception in the following cases:

- When you try to create a new object with the same name as an existing one. For example, if you try to create two accounts with the same address, an exception will be thrown.
- If a database error occurs. If the connection to the database server is lost, or another database problem occurs, an exception will be thrown.
- If client permissions isn't sufficient. Before you can access objects in the hMailServer COM API, you must first authenticate (using Authenticate on hMailServer.Application). If you try to access an object after this which you do not have permission to access, an exception will be thrown. For example, if you authenticate as a user with user-level rights, an exception will be thrown if you try to access global server settings.
- An exception will be thrown if you try to access a nonexistant item in a collection. For example, calling ItemByName() on the hMailServer.Domains collection will throw an exception if the domain cannot be found.

## Creating new objects

To create a new object, you typically run Add() on the parent collection. For example, to add a new domain execute Add() on the Domains collection. The Add() method will return a new object. This new object will only exist in memory, and you need to execute the Save() method on the object to save it in the database. When you execute Save(), the object is saved in the database and added to the collection.

 
