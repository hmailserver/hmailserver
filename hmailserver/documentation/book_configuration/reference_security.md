---
id: 453
title: "Security"
slug: reference_security
parent: book_configuration
index: 0
is_book: false
updated: 2014-10-11
---

This document describes the settings listed under Advanced -> Security.

### SSL/TLS ciphers

Use this setting to override the ciphers which hMailServer will allow for encrypting SSL/TLS connections. This should be entered in OpenSSL cipher list format.

### Verify remote server SSL/TLS certificates

During testing, you may want to disable certificate verification completely. To do this, de-select this option.

Keeping certificate verification enabled in production is strongly recommended.
