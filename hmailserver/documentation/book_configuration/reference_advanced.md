---
title: "Advanced"
slug: reference_advanced
parent: book_configuration
index: 0
is_book: false
---

## General

### Default domain

<div class="indented">This setting allows you to specify what domain hMailServer should default to if a user does not specify the domain name when logging on. For example, if you specify that the default domain is example.com, and a person tries to log on using the user name "bob", hMailServer will assume that it is "bob@example.com" which is trying to lo gon.</div>

### Administration password

<div class="indented">This setting allows you to change the administrator password for your hMailServer installation. This password is required to log on hMailServer Administrator so it's important that you remember it.</div>

## Password hashing

<div class="indented">These settings control how account passwords (and the administrator password) are hashed before being stored. Hashes are self-describing, so changing these settings only affects passwords hashed from now on - existing hashes keep working until they are re-hashed.</div>

### Algorithm

<div class="indented">The key derivation function used to hash passwords. Argon2id is the recommended default: it deliberately uses a chunk of memory as well as CPU time for each hash, which makes large-scale password cracking (e.g. on GPUs) much more expensive. PBKDF2-SHA256 uses CPU time only, with no memory cost - it is lighter on the server but easier to attack at scale, so it's mainly provided for compatibility with environments that require FIPS-validated cryptography.</div>

### Memory cost (kilobytes)

<div class="indented">Only used by Argon2id. The amount of memory used per password hash. Set to 0 to use the recommended default, which is 19456 KiB (19 MiB). Valid range: 4096-1048576 KiB.</div>

### Iterations

<div class="indented">The number of iterations used per password hash. Set to 0 to use the recommended default for the selected algorithm - 2 for Argon2id, 600000 for PBKDF2-SHA256. Valid range: 1-20 for Argon2id, 10000-10000000 for PBKDF2-SHA256.</div>

### Upgrade stored passwords during logon

<div class="indented">When enabled, an account's stored password hash is transparently re-hashed with the current algorithm and cost the next time the account logs on successfully, if it was hashed with a different algorithm or a lower cost.</div>

<div class="indented">The administrator password is an exception: if it is still stored using an older scheme, it is replaced on the next successful logon even when this setting is off.</div>
