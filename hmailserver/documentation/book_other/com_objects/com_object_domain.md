---
title: "Domain object"
slug: com_object_domain
parent: com_objects
index: 0
is_book: false
---

### Description

The domain object represents one hMailServer domain.

### Methods

<div class="api_method_name">Delete()</div>

<div class="api_description">Deletes the domain from the database.</div>

<div class="api_method_name">Save()</div>

<div class="api_description">Saves changes of the object in the database.</div>

<div class="api_method_name">SynchronizeDirectory()</div>

<div class="api_description">OBSOLETE. Only included to make all clients continue working.</div>

### Properties

<div class="api_method_name"><a href="?page=com_object_accounts">Accounts</a> Accounts</div>

<div class="api_description">The accounts collection for the domain.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool Active</div>

<div class="api_description">The enabled-state of the domain.<br></div>

<div class="api_method_name">string ADDomainName</div>

<div class="api_description">The active directory domain this domain will be synchronised to.<br></div>

<div class="api_method_name">bool AddSignaturesToLocalMail</div>

<div class="api_description">Add signatures to local email<br></div>

<div class="api_method_name">bool AddSignaturesToReplies</div>

<div class="api_description">Add signatures to replies on email<br></div>

<div class="api_method_name"><a href="?page=com_object_aliases">Aliases</a> Aliases</div>

<div class="api_description">The aliases collection for the domain.<br> <i>(read-only)</i></div>

<div class="api_method_name">long AllocatedSize</div>

<div class="api_description">Allocated space for this domain.<br> <i>(read-only)</i></div>

<div class="api_method_name">bool AntiSpamEnableGreylisting</div>

<div class="api_description">Enable greylisting for this domain.<br></div>

<div class="api_method_name"><a href="?page=com_object_distributionlists">DistributionLists</a> DistributionLists</div>

<div class="api_description">The distribution lists collection for the domain.<br> <i>(read-only)</i></div>

<div class="api_method_name">eDKIMCanonicalizationMethod DKIMBodyCanonicalizationMethod</div>

<div class="api_description">Gets or sets the body canonicalization method.<br></div>

<div class="api_method_name">eDKIMCanonicalizationMethod DKIMHeaderCanonicalizationMethod</div>

<div class="api_description">Gets or sets the header canonicalization method.<br></div>

<div class="api_method_name">string DKIMPrivateKeyFile</div>

<div class="api_description">Gets or sets the DKIM private key file to use for this domain.<br></div>

<div class="api_method_name">string DKIMSelector</div>

<div class="api_description">Gets or sets the DKIM selector to use for this domain.<br></div>

<div class="api_method_name">bool DKIMSignAliasesEnabled</div>

<div class="api_description">Enable DKIM signing for aliases.<br></div>

<div class="api_method_name">bool DKIMSignEnabled</div>

<div class="api_description">Enable DKIM signing for this domain.<br></div>

<div class="api_method_name">eDKIMAlgorithm DKIMSigningAlgorithm</div>

<div class="api_description">Gets or sets the signing algorithm.<br></div>

<div class="api_method_name"><a href="?page=com_object_domainaliases">DomainAliases</a> DomainAliases</div>

<div class="api_description">The aliases of the domain name.<br> <i>(read-only)</i></div>

<div class="api_method_name">long ID</div>

<div class="api_description">The unique database identifier for the domain.<br> <i>(read-only)</i></div>

<div class="api_method_name">long MaxAccountSize</div>

<div class="api_description">Maximum size of accounts created in this domain.<br></div>

<div class="api_method_name">long MaxMessageSize</div>

<div class="api_description">Maximum message size when sending from this domain.<br></div>

<div class="api_method_name">long MaxNumberOfAccounts</div>

<div class="api_description">Maximum number of accounts which can be added to this domain.<br></div>

<div class="api_method_name">bool MaxNumberOfAccountsEnabled</div>

<div class="api_description">Enable maximum number of accounts.<br></div>

<div class="api_method_name">long MaxNumberOfAliases</div>

<div class="api_description">Maximum number of aliases which can be added to this domain.<br></div>

<div class="api_method_name">bool MaxNumberOfAliasesEnabled</div>

<div class="api_description">Enable maximum number of aliases.<br></div>

<div class="api_method_name">long MaxNumberOfDistributionLists</div>

<div class="api_description">Maximum number of distribution lists which can be added to this domain.<br></div>

<div class="api_method_name">bool MaxNumberOfDistributionListsEnabled</div>

<div class="api_description">Enable maximum number of distribution lists.<br></div>

<div class="api_method_name">long MaxSize</div>

<div class="api_description">Maximum size of this domain in MB.<br></div>

<div class="api_method_name">string Name</div>

<div class="api_description">Name of the domain.<br></div>

<div class="api_method_name">string PlusAddressingCharacter</div>

<div class="api_description">Plus addressing character.<br></div>

<div class="api_method_name">bool PlusAddressingEnabled</div>

<div class="api_description">Use plus addressing for this domain.<br></div>

<div class="api_method_name">string Postmaster</div>

<div class="api_description">The postmaster for this domain. E-mail sent to an nonexistent address will be redirected to this address.<br></div>

<div class="api_method_name">bool SignatureEnabled</div>

<div class="api_description">Enable the signature<br></div>

<div class="api_method_name">string SignatureHTML</div>

<div class="api_description">The HTML version of the signature<br></div>

<div class="api_method_name">eDomainSignatureMethod SignatureMethod</div>

<div class="api_description">The domain signature method<br></div>

<div class="api_method_name">string SignaturePlainText</div>

<div class="api_description">The plain text version of the signature<br></div>

<div class="api_method_name">long Size</div>

<div class="api_description">Current size of this domain in MB.<br> <i>(read-only)</i></div>
