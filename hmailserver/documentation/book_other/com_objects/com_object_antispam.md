---
title: "AntiSpam object"
slug: com_object_antispam
parent: com_objects
index: 0
is_book: false
---

### Description

The AntiSpam object contains all server-wide settings related to anti-spam.

### Methods

<div class="api_method_name">ClearGreyListingTriplets()</div>

<div class="api_description">Clear greylisting triplets.</div>

<div class="api_method_name">DKIMVerify(string File)</div>

<div class="api_description">Verifies the DKIM-Signature of the specified file. Returns true if neutral or pass.</div>

<div class="api_method_name">TestSpamAssassinConnection(string Hostname, long Port, string ResultText)</div>

<div class="api_description">Tests to connect to SpamAssassin</div>

### Properties

<div class="api_method_name">bool AddHeaderReason</div>

<div class="api_description">Add header X-hMailServer-Reason when spam message is found.<br></div>

<div class="api_method_name">bool AddHeaderSpam</div>

<div class="api_description">Add header X-hMailServer-Spam when spam message is found.<br></div>

<div class="api_method_name">bool BypassGreylistingOnMailFromMX</div>

<div class="api_description">Bypass grey listing if mail comes from domain A or MX record.<br></div>

<div class="api_method_name">bool BypassGreylistingOnSPFSuccess</div>

<div class="api_description">Bypass grey listing if SPF verifies successfully.<br></div>

<div class="api_method_name">bool CheckHostInHelo</div>

<div class="api_description">Check host specified in HELO command.<br></div>

<div class="api_method_name">long CheckHostInHeloScore</div>

<div class="api_description">Check host specified in HELO command (SpamScore).<br></div>

<div class="api_method_name">bool CheckPTR</div>

<div class="api_description">Check rDNS/PTR record.<br></div>

<div class="api_method_name">long CheckPTRScore</div>

<div class="api_description">Check rDNS/PTR record (SpamScore).<br></div>

<div class="api_method_name">bool DKIMVerificationEnabled</div>

<div class="api_description">Enable DKIM header verification.<br></div>

<div class="api_method_name">long DKIMVerificationFailureScore</div>

<div class="api_description">DKIM verification failure score.<br></div>

<div class="api_method_name"><a href="?page=com_object_dnsblacklists">DNSBlackLists</a> DNSBlackLists</div>

<div class="api_description">Antispam: DNS black lists collection.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool GreyListingEnabled</div>

<div class="api_description">Use grey listing.<br></div>

<div class="api_method_name">long GreyListingFinalDelete</div>

<div class="api_description">GreyListing - Delete used triplet after this time (hours).<br></div>

<div class="api_method_name">long GreyListingInitialDelay</div>

<div class="api_description">GreyListing - Delay before accepting messages (minutes).<br></div>

<div class="api_method_name">long GreyListingInitialDelete</div>

<div class="api_description">GreyListing - Delete unused triplet after this time (hours).<br></div>

<div class="api_method_name"><a href="?page=com_object_greylistingwhiteaddresses">GreyListingWhiteAddresses</a> GreyListingWhiteAddresses</div>

<div class="api_description">White list for greylisting.<br> <i>(read-only)</i></div>

<div class="api_method_name">long MaximumMessageSize</div>

<div class="api_description">Maximum message size to run anti-spam on.<br></div>

<div class="api_method_name">bool PrependSubject</div>

<div class="api_description">Prepend message subject.<br></div>

<div class="api_method_name">string PrependSubjectText</div>

<div class="api_description">Text to prepend to message subject.<br></div>

<div class="api_method_name">bool SpamAssassinEnabled</div>

<div class="api_description">Antispam: Enable SpamAssassin integration<br></div>

<div class="api_method_name">string SpamAssassinHost</div>

<div class="api_description">Antispam: SpamAssassin host name<br></div>

<div class="api_method_name">bool SpamAssassinMergeScore</div>

<div class="api_description">Antispam: Merge score from SpamAssassin.<br></div>

<div class="api_method_name">long SpamAssassinPort</div>

<div class="api_description">Antispam: SpamAssassin port<br></div>

<div class="api_method_name">long SpamAssassinScore</div>

<div class="api_description">Antispam: SpamAssassin (score).<br></div>

<div class="api_method_name">long SpamDeleteThreshold</div>

<div class="api_description">Spam delete threshold<br></div>

<div class="api_method_name">long SpamMarkThreshold</div>

<div class="api_description">Spam mark threshold<br></div>

<div class="api_method_name"><a href="?page=com_object_surblservers">SURBLServers</a> SURBLServers</div>

<div class="api_description">SURBL servers collection.<br> <i>(read-only)</i></div>

<div class="api_method_name">long TarpitCount</div>

<div class="api_description">Tarpit count (Obsolete: No longer used).<br></div>

<div class="api_method_name">long TarpitDelay</div>

<div class="api_description">Tarpit delay (Obsolete: No longer used).<br></div>

<div class="api_method_name">bool UseMXChecks</div>

<div class="api_description">Antispam: Use MX checks.<br></div>

<div class="api_method_name">long UseMXChecksScore</div>

<div class="api_description">Antispam: Use MX checks (score).<br></div>

<div class="api_method_name">bool UseSPF</div>

<div class="api_description">Antispam: Use SPF.<br></div>

<div class="api_method_name">long UseSPFScore</div>

<div class="api_description">Antispam: Use SPF (score).<br></div>

<div class="api_method_name"><a href="?page=com_object_whitelistaddresses">WhiteListAddresses</a> WhiteListAddresses</div>

<div class="api_description">Global white list.<br> <i>(read-only)</i></div>
