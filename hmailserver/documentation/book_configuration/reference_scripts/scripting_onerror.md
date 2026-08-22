---
id: 422
title: "OnError"
slug: scripting_onerror
parent: reference_scripts
index: 0
is_book: false
updated: 2017-07-19
---

## Overview

This event is executed directly when an error occurs. The event is executed before any global rules are executed

## Signature

OnError(iSeverity, iCode, sSource, sDescription)

## Parameters

- iSeverity - Error severity. 1 - Critical, 2 - High, 3 - Medium, 4 - Low
- iCode - The hMailServer error code.
- sSource - The component which reported the error.
- sDescription - A description of the reported error.

## Result values

- None
