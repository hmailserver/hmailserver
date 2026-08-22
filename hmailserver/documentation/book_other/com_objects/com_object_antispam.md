---
id: 398
title: "AntiSpam object"
slug: com_object_antispam
parent: com_objects
index: 0
is_book: false
updated: 2017-02-14
---

### Description

The AntiSpam object contains all server-wide settings related to anti-spam.

### Methods

<div class="api_method_name">ClearGreyListingTriplets()</div>

<div class="api_description"> Clear greylisting triplets.</div>

### Properties

<div class="api_method_name">bool AddHeaderReason</div>

<div class="api_description"> Add header X-hMailServer-Reason when spam message is found.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool AddHeaderSpam</div>

<div class="api_description"> Add header X-hMailServer-Spam when spam message is found.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool BypassGreylistingOnSPFSuccess</div>

<div class="api_description"> Bypasses Greylisting if IP sending mail is sucessfully confirmed to meet the SPF record.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool BypassGreylistingOnMailFromMX</div>

<div class="api_description"> Bypasses Greylisting if IP sending mail is in MX record.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool CheckHostInHelo</div>

<div class="api_description"> Check host specified in HELO command.</div>

<div class="api_description"> </div>

<div class="api_method_name">long CheckHostInHeloScore</div>

<div class="api_description"> Check host specified in HELO command (SpamScore).</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_dnsblacklists">DNSBlackLists</a> DNSBlackLists</div>

<div class="api_description"> Antispam: DNS black lists collection.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">bool GreyListingEnabled</div>

<div class="api_description"> Use grey listing.</div>

<div class="api_description"> </div>

<div class="api_method_name">long GreyListingFinalDelete</div>

<div class="api_description"> GreyListing - Delete used triplet after this time (hours).</div>

<div class="api_description"> </div>

<div class="api_method_name">long GreyListingInitialDelay</div>

<div class="api_description"> GreyListing - Delay before accepting messages (minutes).</div>

<div class="api_description"> </div>

<div class="api_method_name">long GreyListingInitialDelete</div>

<div class="api_description"> GreyListing - Delete unused triplet after this time (hours).</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_greylistingwhiteaddresses">GreyListingWhiteAddresses</a> GreyListingWhiteAddresses</div>

<div class="api_description"> White list for greylisting.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">long MaximumMessageSize</div>

<div class="api_description"> Maximum message size to run anti-spam on.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool PrependSubject</div>

<div class="api_description"> Prepend message subject.</div>

<div class="api_description"> </div>

<div class="api_method_name">string PrependSubjectText</div>

<div class="api_description"> Text to prepend to message subject.</div>

<div class="api_description"> </div>

<div class="api_method_name">bool SpamAssassinEnabled</div>

<div class="api_description"> Antispam: Enable SpamAssassin integration</div>

<div class="api_description"> </div>

<div class="api_method_name">string SpamAssassinHost</div>

<div class="api_description"> Antispam: SpamAssassin host name</div>

<div class="api_description"> </div>

<div class="api_method_name">bool SpamAssassinMergeScore</div>

<div class="api_description"> Antispam: Merge score from SpamAssassin.</div>

<div class="api_description"> </div>

<div class="api_method_name">long SpamAssassinPort</div>

<div class="api_description"> Antispam: SpamAssassin port</div>

<div class="api_description"> </div>

<div class="api_method_name">long SpamAssassinScore</div>

<div class="api_description"> Antispam: SpamAssassin (score).</div>

<div class="api_description"> </div>

<div class="api_method_name">long SpamDeleteThreshold</div>

<div class="api_description"> Spam delete threshold</div>

<div class="api_description"> </div>

<div class="api_method_name">long SpamMarkThreshold</div>

<div class="api_description"> Spam mark threshold</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_surblservers">SURBLServers</a> SURBLServers</div>

<div class="api_description"> SURBL servers collection.<br>
<i>(read-only)</i></div>

<div class="api_description"> </div>

<div class="api_method_name">bool UseMXChecks</div>

<div class="api_description"> Antispam: Use MX checks.</div>

<div class="api_description"> </div>

<div class="api_method_name">long UseMXChecksScore</div>

<div class="api_description"> Antispam: Use MX checks (score).</div>

<div class="api_description"> </div>

<div class="api_method_name">bool UseSPF</div>

<div class="api_description"> Antispam: Use SPF.</div>

<div class="api_description"> </div>

<div class="api_method_name">long UseSPFScore</div>

<div class="api_description"> Antispam: Use SPF (score).</div>

<div class="api_description"> </div>

<div class="api_method_name"><a href="?page=com_object_whitelistaddresses">WhiteListAddresses</a> WhiteListAddresses</div>

<div class="api_description"> Global white list.<br>
<i>(read-only)</i></div>

 
