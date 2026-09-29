# SPF

An implementation of RFC 7208. `SPF::Test` is the entry point; everything else is
reachable only from there and from the tests.

| File | What it does |
|---|---|
| `SPFSyntax` | The ABNF of section 12 and the macro rules of section 7.1 |
| `SPFRecord` | Parses a record into directives and modifiers |
| `SPFRecordLocator` | Finds and parses the record a domain publishes, sections 4.3-4.5 |
| `SPFMacroExpander` | Macro expansion, section 7 |
| `SPFAddress` | An IP address in the forms SPF needs |
| `SPFEvaluator` | `check_host()`, section 4 |
| `SPFDnsResolver` | The DNS seam, over `DNSResolver` |
| `SPFResult` | The seven results of section 2.6 |

`SPFDnsLookup` is the seam the evaluator asks DNS through. `SPFDnsResolver`
implements it for the server; `SPFTestLookup` and `Conformance/ConformanceLookup`
implement it for tests.

## Why some things are their own

**`SPFAddress` rather than `IPAddress`.** SPF needs three things `IPAddress` does
not answer: an IPv6 address as 32 dotted nibbles for `%{i}`, the shortened form for
`%{c}`, and the address literals of section 12, which are spelled more strictly
than a resolver insists on. It also has to work in the portable build, and
`IPAddress` is built on Boost.Asio.

**`SPFResult` in its own header.** `SPF` is the service entry point and brings the
singleton and the wide string type with it. The evaluator, the record locator and
the conformance suite are all built by the portable build and need to name a
result.

**`DNSResolver::GetRecordsOfType`.** The typed methods beside it each add
behaviour SPF must not have: `GetIpAddresses` merges A and AAAA and gates AAAA on
this server's IPv6 configuration, where sections 5.3 and 5.4 select by the family
the client connected over; `GetPTRRecords` builds a reverse-mapping name from an
address, which an evaluation has already built; `GetMXRecords` reports the null MX
of RFC 7505 the way it reports a failed query, where section 5.4 reads it as a
domain with no exchangers.

## How the pieces fit

A record is parsed whole before any of it is used, which section 4.6 requires: a
mistake after a mechanism that would have matched is still a permerror.

A term's target is stored as written. Expansion depends on the message, and
section 7.1 does not re-parse what an expansion produces - a name that comes out
unusable is a name that does not exist, so the mechanism does not match.

Section 7.3's transformers rejoin the parts with dots whatever delimiter they were
split at, which is the point of naming one: the section's own example has `%{l-}`
turn `strong-bad` into `strong.bad`.

Section 4.6.4's limits are per check, not per record, so one counter sees every
query an evaluation makes including the macro expander's. It counts *terms* whose
queries answer nothing, not queries: one `mx` over five exchangers with no address
of the client's family is one such term.

## Testing

`Conformance/` holds the openspf.org suite for RFC 7208: 203 cases, all decided
and all agreeing. It runs in the portable build under the sanitizers and inside
`hMailServer.exe` through `Utilities.RunTestSuite`, so every CI run executes it.

The testers beside it cover what the suite does not. Each was found by breaking a
behaviour and checking whether any case noticed:

* A lookup that fails. Sections 5.3 and 5.4 make a failed A, AAAA or MX query a
  temperror, but the suite only has a case for `exists`.
* Section 4.6.4's term counting. The suite's `mech-over-limit` spends its budget
  on names that do not exist, so it trips the void limit first, and
  `redirect-loop` recurses rather than counting.
* What the resolver hands back. Every address in the suite's zones is one.
* That section 6.2 explains a fail and only a fail. All 22 cases that assert an
  explanation are fails.
* The grammar rule by rule, which `SPFRecordTester` does: the suite has no case
  for a zero-length prefix on `ip6`, or for `a/24//64` against `a/24/64`.

Failures are returned from a tester rather than reported, because where they
belong differs between the two hosts, and a run should list all of them.

`Portable/Fuzz/` fuzzes the parser and the macro expander.

## Open questions

Two places where the grammar is deliberately lenient and the suite tests neither:
a zero digit transformer (`%{d0}`), which is accepted, and an unterminated `%{`,
which is rejected. A corpus of real records is what would settle them.

## Traps

A record may hold a NUL or a byte above 0x7f, so records cross `SPFDnsLookup` as
counted bytes. Two of the suite's policies end in a NUL.

`MimeCode` folds a header value at 76 characters on the last space, and
`Application` turns folding on. A test asserting on `Authentication-Results` must
unfold it first, or it will pass for one sender and fail for another depending on
how long the fields ahead of the one it wants happen to be.

`DOMAIN` is a macro in MSVC's `math.h`. The portable build cannot see that.
