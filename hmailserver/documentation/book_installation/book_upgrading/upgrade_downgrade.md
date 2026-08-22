---
title: "Downgrading"
slug: upgrade_downgrade
parent: book_upgrading
index: 100
is_book: false
---

## Overview

There is no functionality in hMailServer which can be used to perform downgrades. When you upgrade from one version to the next, changes are made to the database structure. The old version of hMailServer will not be able to run towards a database using the new database structure, and vice versa. The hMailServer installation package includes database scripts that perform these database changes, but the database scripts are one-way, The scripts can upgrade the database but not downgrade. There are manual ways to downgrade a hMailServer installation, but in many cases these will not work.

### Restoring a backup

If you have upgraded and want to downgrade to the previous version again, one way to do this is to uninstall hMailServer completely, install the old version and then restore the backup. This approach obviously assumes that you have working backups from the old version. A potential downside with this approach is that any email you received in the newer version will be lost (unless your user have already downloaded them using POP3).

### Manually reverting the database changes

The database changes made by the upgrade scripts can in some cases be manually reverted by a person with knowledge in SQL syntax. The person need to manually go through the upgrade scripts starting from the end and determine what they do. He or she then needs to create new scripts which reverts the change in the datbase. In many cases, this approach won't be possible. For instance, if a feature which existed in the old version has been removed in the new version, the data required for this feature may have been dropped when the upgrade was made.

### Alternatives

If you experience a problem in the current version of hMailServer, the best way to proceed is trying to solve the problem. Sometimes users feel that they must downgrade if they run into a problem, when the problem they experience isn't really related to hMailServer. The problem may be related to some modified feature in hMailServer, or it may be a bug in which case it will be fixed if it's reported.
