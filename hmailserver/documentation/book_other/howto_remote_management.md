---
id: 217
title: "Remote management"
slug: howto_remote_management
parent: book_other
index: 0
is_book: false
updated: 2009-07-03
---

# Overview

There are two different common methods to manage a hMailServer installation remotely. The first is to use PHPWebAdmin. PHPWebAdmin is a web based administration package which offers almost the same functionality as hMailServer (editing of rules is not available in PHPWebAdmin). PHPWebAdmin requires a web server with PHP support to run, but it makes it possible to manage your hMailServer installation from anywhere on the Internet. The other method is to install the hMailServer client tools and connect remotely to the hMailServer installation. Due to port blocking in firewalls, this method only works over a LAN or a VPN. Theoretically, it would be possible to open up these ports, but this is not recommended due to security issues this would give.

## Using PHPWebAdmin

Please see the [installation tutorial for PHPWebAdmin](?page=howto_install_phpwebadmin).

## Using hMailServer Administrator

To connect to an hMailServer instance running on another computer in your network, run the hMailServer installation program on the computer you want to connect from. During the installation, select to install the client tools. After installation of the client tools, start hMailServer Administrator. In the connect dialog, select the "localhost" item, and click edit, and enter the appropriate information.

### Common problems

<p>When using hMailServer Administrator to connect to a hMailServer instance running on another computer, it's quite common that you get an <em>Access denied</em> message. hMailServer Administrator uses DCOM/RPC to communicate with the hMailServer instance on the other computer, and for this to work properly you may need to modify settings on your client or server. Due to limitations in this Microsoft technology, it's not possible for hMailServer Administrator to determine the original cause of the <em>Access denied</em> message, so you need to manually check the settings. If you get an <em>Access denied error, or an </em><span class="postbody"><em>Automation Error</em> </span>when trying to connect, check that...</p>

<ul>
    <li>the <em>Remote Procedure Call (RPC) </em>service is running on both the client and the server.</li>
    <li>your firewall is not blocking DCOM ports. For more information on this subject, see Microsoft <a href="http://support.microsoft.com/kb/217351">KB217351</a>.</li>
    <li>the following registry entries have the value Y
    <ul>
        <li><span class="postbody"> HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Ole\EnableDCOM </span></li>
        <li><span class="postbody"> HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Ole\EnableRemoteConnect </span></li>
    </ul>
    </li>
    <li>that DCOM is enabled on both the client and server as described in Microsoft KB <a href="http://support.microsoft.com/kb/313273">313273</a>:</li>
    <li>that you are a member of the Administrators group on both the client and the server.</li>
    <li>that, if your computer is a part of a workgroup and not a domain, that a Windows account with the same name and password exists on the server and the client, and you're logged on as this user.</li>
</ul>
