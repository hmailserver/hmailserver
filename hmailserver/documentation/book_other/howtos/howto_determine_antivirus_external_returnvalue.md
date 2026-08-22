---
title: "External virus scanner - Return value"
slug: howto_determine_antivirus_external_returnvalue
parent: howtos
index: 0
is_book: false
---

## Overview

If you intend to use an external antivirus scanner in hMailServer, you must configure the return value in hMailAdmin. There are two ways to determine what value to enter. Either read the documentation for your antivirus scanner or test it yourself. There now follows a description of how to test it yourself. Note that you should never perform the test on an hMailServer system that is in production, since there's a risk that even email without viruses would be deleted during the testing.   

  

The return value is normally a value between 1 and 255.

## Determining the value

### Step 1: Enable the virus scanner

1. Start hMailAdmin
2. Browse to the settings for the external virus scanner.
3. Select *Use external virus scanner*.
4. Specify the full path to your virus scanner.
5. Enter return value 250.

### Step 2: Turn on logging and check return values

1. Turn on application and debug logging.
2. Send an email that contains the eicar test virus.
3. Check the return value in the log file.
4. Send an email that does not contain the eicar test virus.
5. Check the return value in the log file.

In hMailAdmin, you should specify the return value that was given in the logs when you sent the eicar test virus. If you find that the return value is the same when you attach the eicar test virus and when you don't, then there is a problem. In that case, you should not specify any value. Report this to the forum and you should get assistance.
