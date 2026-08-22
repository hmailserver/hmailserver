---
id: 172
title: "Single server, static IP address"
slug: scenario_single_server_static_ip
parent: scenarios
index: 0
is_book: false
updated: 2007-07-30
---

## Scenario:

You have a single server you want to use as email server home. You Internet service provider (ISP) has given you a static IP address. The below tutorial assumes that the domain you want to host email for is named *mydomain.com*.

### Step 1: Checking the requirements

Before you continue, you should check that your Internet service provider (ISP) allows you to run your own email server. You should also check that your ISP has not blocked port 25 for incoming traffic. You also needs to know whether they are blocking outgoing traffic on port 25.

### Step 2: Configuring the DNS server

To be able to receive email from other servers, you must set up MX records for your domain. The MX records are entries in the DNS server that tells other computers on the Internet which computer (host name) is hosting the email for your domain. 

If you have access to a web interface that lets you modify DNS settings, you can set up your MX records yourself. If you don't have access, you should contact the company that registered your domain and ask them to set up the MX records for your domain.

1. Create an A record named *mail.something.com*.
2. Configure the A record *mail.something.com* so that it points at your computers IP address.
3. Add a MX record that has the value *mail.something.com* for your domain.

### Step 3: Changing firewall settings

If you have a firewall (which you hopefully have) you need to modify its settings to allow hMailServer to receive email. Email is normally sent and received on TCP/IP port 25. This means that you must configure your firewall to allow incoming and outgoing traffic on TCP/IP port 25. If you're behind some kind of router, you need to configure the router to forward all traffic on port 25 to the computer where hMailServer is running.

### Step 4: Install hMailServer

1. Download the latest hMailServer version
2. Run through the installation wizard.
3. Start hMailServer Administrator.
4. Click *Add Domain...*
5. Enter the name of your domain and click Save.
6. Click *Add account...* and add a new email account.
7. Go to *Settings->Protocols->SMTP* and choose the *Delivery of email tab*.
8. In the Host name setting, enter the host name of your computer, in this example *something.no-ip.com*

### Step 5: Configuring outgoing email

If your Internet service provider is blocking outgoing traffic on port 25, hMailServer will not be able to deliver email to other servers since all SMTP servers normally only receives email on port 25. If your ISP is blocking outgoing traffic on port 25, the easiest solution is normally to configure hMailServer to forward all outgoing email through your ISP's SMTP server. To do this, follow these steps:

1. Start hMailServer Administrator
2. Go to the SMTP setitngs and choose Delivery of email.
3. In the SMTP relayer setting, enter the name of your ISP's smtp server, for example *smtp.myisp.com*.
4. If your ISP requires a username / password when sending email through their server, select Server requires authentication and enter the username and password.

### Step 6: Configuring your email client

In your email client, add a new account. Enter the following information:

- Hostname - The host name or IP address of the computer where hMailServer runs
- Username - Your full email address. Same as the account address you added in hMailServer Administrator.
- Password - The password you defined in hMailServer Administrator
