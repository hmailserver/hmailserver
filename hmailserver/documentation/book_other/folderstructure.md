---
title: "hMailServer folder structure"
slug: folderstructure
parent: book_other
index: 0
is_book: false
---

<table width="100%" border="0">
    <tbody>
        <tr>
            <td valign="top">/Addons</td>
            <td valign="top">Addon scripts and utilities</td>
            <td> 	</td>
        </tr>
        <tr>
            <td valign="top">/Bin</td>
            <td valign="top">hMailServer binaries. The actual server and hMailAdmin</td>
        </tr>
        <tr>
            <td valign="top">/Data</td>
            <td valign="top">E-mail messages. The files in the Data root are messages 			that have not been delivered yet. Sub folders contain 			delivered messages. E-mail messages have the extension .hma or .eml, depending on in which hMailServer version they were created.<br></td>
        </tr>
        <tr>
            <td valign="top">/DBScripts</td>
            <td valign="top">SQL scripts that are used when upgrading from one 			version to another. The files have the following  			naming standard: UpgradeXXXXtoYYYYZZZZ.sql. XXXX 			is the old version, YYYY is the new version and 			ZZZZ is the type of database server.</td>
        </tr>
        <tr>
            <td valign="top">/Events</td>
            <td valign="top">VBA and JavaScript scripts used by hMailServer.</td>
        </tr>
        <tr>
            <td valign="top">/Languages</td>
            <td valign="top">Translations of the hMailServer user interface.</td>
        </tr>
        <tr>
            <td valign="top">/Logs</td>
            <td valign="top">Log files created by hMailServer.</td>
        </tr>
        <tr>
            <td valign="top">/MySQL</td>
            <td valign="top">The MySQL server</td>
        </tr>
        <tr>
            <td valign="top">/PHPWebAdmin</td>
            <td valign="top">The PHP web administration interface for hMailServer.</td>
        </tr>
        <tr>
            <td valign="top">/Temp</td>
            <td valign="top">Temporary directory used by hMailServer. For example used 			when hMailServer extracts attachments that needs to be virus 			scanned. When hMailServer extracts an attachment for virus scanning, it is given the extension .tmp.<br></td>
        </tr>
    </tbody>
</table>
