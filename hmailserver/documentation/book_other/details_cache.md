---
id: 85
title: "Caching"
slug: details_cache
parent: book_other
index: 0
is_book: false
updated: 2009-07-03
---

## Overview

The communication between hMailServer and the database may be a bottleneck when the load is high. So, in order to improve performance, a cache was implemented on hMailServer; the cache avoids some database access by caching database objects in hMailServer's process memory. If an object is cached in memory, hMailServer does not have to connect to the database to read the object again. The caching feature is found in the advanced settings in hMailAdmin.

### How does it work?

When hMailServer needs to read an object from the database, it first looks in the cache to see whether the object exists there. If it is, hMailServer checks how long it was since the object was read. If it was longer than the time-to-live (TTL) defined for that object type, the object is removed from the cache. If the object does not exist in the cache, or if it exists in the cache but is too old, hMailServer reads the object from the database and then updates the cache. If an object is modified or deleted using PHPWebAdmin or hMailAdmin, the object will immediately be updated in the cache. If an object is modified or deleted directly in the database by some third-party tool, the object will be updated in memory when its age exceeds the TTL.   

  

Only an object of a specific type can be cached. For example, when you cache the domain object, only the domain object is cached. Sub-objects such as accounts and aliases are not cached along with the domain.

### The hit rate value

The hit rate tells you about the success of the cache, by showing how often hMailServer needs to ask the database for a specific cached-object type. If the hit-rate for an object type is 70%, it means that hMailServer found that object type in the cache 70% of the time, and had to ask the database for it only 30% of the time. Increasing the TTL of an object usually increases the hit-rate. When, while looking for an object, hMailServer can't locate it neither in the cache not in the database, that does not count as an cache-miss, because the object could never be in the cache in the first place.

### Changing time-to-live

The default TTL for objects is 60 seconds. It is hard to give a universal value of TTL that works for all server setups. For example, if you updated domains directly in the database, and did that often, 10 seconds might be a good value. If you update domains directly in the database, but do it very seldom, 60 seconds might be better. If you only update domains using PHPWebAdmin or hMailAdmin, you can change the TTL to 3600 seconds (1 hour). hMailServer will then only have to read domains from the database once an hour.

### Benefits

- Reading an object from memory requires less CPU than reading it from the database.
- When hMailServer reads an object from memory, the database server does not get involved. This reduces the database server's CPU usage.

### Drawbacks

- Memory usage is increased slightly, however, only very slightly.
- If an object is modified or deleted from the database using a third-party tool, hMailServer won't know of it until the time-to-live for the object in memory has passed.

## Keeping cache coherence

If an object that is changed directly in the database also exists in the cache, hMailServer will use the object in the cache until it expires. This can be troublesome in certain circumstances. Ideally, hMailServer would always use the most recent data that is on the database; however, lowering the TTL increases database access. There are, nevertheless, three ways to ensure that hMailServer always gets the most up-to-date data from the database:

<ul>
    <li><u>Disable the cache</u>. If there is no cache, hMailServer will directly access the database, and so will get the updated data.</li>
    <li><u>Don't update the database directly</u>. If you only use hMailAdmin or COM, the cache will always be kept coherent. In this case, using a very high TTL is absolutely safe. This is actually the preferred approach, as you will reap the benefits of having the cache, while avoiding most of its drawbacks (except for memory consumption).</li>
    <li><u>If you absolutely need to directly update the database, disable the cache</u>. If you disable the cache prior to updating the database directly, and re-enable the cache afterwards - both of which can be done via COM - you will never have any coherence problems; this might be a better approach than waiting for the objects to expire, depending on the objects that you updated. Of course, a more radical approach is to stop the hMailServer service while the database is being manually updated, and restart it in the end.</li>
</ul>

 
