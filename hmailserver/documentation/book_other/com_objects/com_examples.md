---
title: "COM API: Examples"
slug: com_examples
parent: com_objects
index: 0
is_book: false
---

### Script examples

The below examples are written in VBA. To use them, copy them to a text file, save the file with the extension .vbs and then double-click on the file in the Windows Explorer to run it. Before doing this, make sure that you fully understand the purpose of the script and what effect it may have on your hMailServer installation. We strongly recommend that you run scripts in an test environment before running them on a hMailServer installation used in production.

- [Changing password for an account](?page=com_example_account_change_password)
- [Copying a message from one IMAP folder to another](?page=com_example_copy_message_to_imap_folder)
- [Creating an account](?page=com_example_account_create)
- [Deleting an account](?page=com_example_account_delete)
- [Enabling a domain](?page=com_example_domain_enable)
- [Listing IMAP folders belonging to a specific account](?page=com_example_folder_list)
- [Sending a message](?page=com_example_message_send)

### Trigger examples

Triggers are used to perform certain actions when an event occur in hMailServer. For instance, you can automatically execute a script when a message is delivered or when a backup is performed. The below scripts are examples of such.

- [Print Hello World to the hMailServer event log](?page=com_example_trigger_helloworld)
- [Remove all Received headers from messages](?page=com_example_trigger_remove_headers)
