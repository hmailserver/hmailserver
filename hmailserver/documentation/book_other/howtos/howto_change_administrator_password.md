---
title: "HOWTO: Changing the administrator password"
slug: howto_change_administrator_password
parent: howtos
index: 0
is_book: false
---

## Background

<p>Starting with hMailServer 4.3, a main administrator password exists for hMailServer. The purpose of this is to prevent unauthorized users from making changes to your hMailServer installation. During the installation of hMailServer 4.3, you specify the password you wish to use. A hash of this password is then stored in hMailServer.ini (The <span class="Code">AdministratorPassword</span> setting under the <span class="Code">Security</span> section ). Like account passwords, the administrator password is hashed using the algorithm configured under <a href="?page=reference_advanced">Settings -> Advanced -> Password hashing</a> (Argon2id by default).</p>

## Changing the password

### Method 1 - Using hMailServer Administrator

If you know the current administrator password, or any other password to an account  with server-level administration permissions, you can change the password using hMailServer Administrator.

1. Start hMailServer Administrator and log on.
2. Go to Settings -> Advanced
3. Enter the new Administration password and save the changes.

### Method  2 - By reinstalling

1. Remove the *AdministratorPassword* setting from hMailServer.ini.
2. Re-install the same hMailServer version you have already installed. During the installation you will be prompted to specify a new password.

 
