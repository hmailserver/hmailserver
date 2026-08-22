---
title: "Display language"
slug: howto_set_language
parent: book_quickstartguide
index: 9
is_book: false
---

## Overview

In the hMailServer installation, only two languages are included - swedish and english. It is however possible to configure hMailServer to use a range of other languages.

### What translations are available?

To see what translations are available, go to the [hMailServer Translation Status page](http://www.hmailserver.com/devnet/?page=translation_status). The page shows you a list of languages and the amount of the user interface which has been translated to this language. The translation to several languages are incomplete to 100%. This means that some parts of the user interface will still be shown in english. For instance, if only 70% of the user interface is translated, 30% of the user interface will be shown in english.

It's recommended not to use a translation unless than more than 97% of the user interface is translated. While there are no technical problems with using a language which is only partially translated, it may be confusing when some strings are in one language and other strings in another language.

### How do I install a new translation?

In the future this functionality is likely to be built-in into hMailServer Administrator, but at the moment it's a manual process.

<ol>
    <li>Go to the <a href="http://www.hmailserver.com/devnet/?page=translation_status">hMailServer Translation Status page</a> and select the translation you want to use.</li>
    <li>When you see the list of all translated strings, select all of them and copy them to the clip board. </li>
    <li>Start Notepad, and paste all the strings into that program.</li>
    <li>Save the file in the hMailServer Languages directory, typically <span class="Code">C:\Program Files\hMailServer\Languages</span>. Name the file &lt;language&gt;.ini, for example <span class="Code">italian.ini</span> or <span class="Code">german.ini</span>. The file should be saved in Unicode format.</li>
    <li>Open <span class="Code">hMailServer.ini</span>, typically located under <span class="Code">C:\Program Files\hMailServer\Bin</span>.</li>
    <li>Locate the line which contains <span class="Code">ValidLanguages=english,swedish</span> and add italian to this line. Notice that every language is separated by comma. For example:<br>
    <span class="Code">ValidLanguages=english,swedish,italian</span></li>
    <li>Restart the hMailServer service.</li>
</ol>

Now the hMailServer service is aware of the, in this example, italian translation.

 

### How do I use the new translation?

In hMailServer Administrator, choose *Select language* from the *File* menu.

<p>To configure WebAdmin to use this language, specify it in the WebAdmin config file, <span class="Code">config.php</span>.</p>
