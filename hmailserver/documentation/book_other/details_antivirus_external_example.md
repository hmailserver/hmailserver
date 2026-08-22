---
title: "External anti virus sample settings"
slug: details_antivirus_external_example
parent: book_other
index: 0
is_book: false
---

## Overview

This page contains a list of virus command line examples which works with hMailServer.

### AVG Free 7

> Command line: "C:\Program Files\Grisoft\AVG Free\avgscan.exe" /EXT=* /NOBOOT /NOMEM /SCAN /NOSELF /NOHIMEM /ARC "%FILE%"

> Return value: 6

### AVG Free 8

> Note that earlier versions of AVG Free 8's avgscanx.exe did not return a value indicating whether a virus was found or not. To be able to use the command line below, make sure you have an up to date version of AVG Free 8. As of 2008-07-07, the version published on [http://free.avg.com/](http://free.avg.com/) is up to date.
> 
> 
> Command line: "C:\Program Files\AVG\AVG8\avgscanx.exe" /EXT=* /NOBOOT /NOMEM /SCAN="%FILE%" /NOSELF /NOHIMEM /ARC

> Return value: 5

### AVG Free 9.0

<p style="margin-left: 40px">Command line: "C:\Program Files\AVG\AVG8\avgscanx.exe" /EXT=* /NOBOOT /NOMEM /SCAN="%FILE%" /NOSELF /NOHIMEM /ARC <br>
<br>
Return value: 5</p>

### AVG 2016 Biz edition

<p style="margin-left: 40px;">Command Line "C:\Program Files (x86)\AVG\Av\avgscana.exe" /EXT=* /SCAN="%FILE%" /ARC</p>

<p style="margin-left: 40px;">Return Value: 5<br>
<br>
 </p>

### To test that your AntiVirus install is working

<p style="margin-left: 40px;">Use the test button in hMailsevrer&gt;&gt; AntiVirus &gt;&gt; External Virus Scanner, which will test using the EICAR simulated virus.</p>

<p style="margin-left: 40px;">Or use one of the many web sites who will send the EICAR simulated virus, including <a href="http://www.aleph-tec.com/eicar/index.php">http://www.aleph-tec.com/eicar/index.php</a>. </p>

 
