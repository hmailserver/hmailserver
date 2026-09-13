# The RFC 7208 conformance suite

This directory holds the openspf.org test suite for RFC 7208, and what is needed
to run hMailServer's SPF evaluator against it.

## The vendored suite

| File | |
|---|---|
| `rfc7208-tests.yml` | The suite. Release 2014.04, 203 cases in 16 sections. |
| `rfc7208-tests.LICENSE` | Its licence: three-clause BSD, © Stuart D Gathman, Julian Mehnle, Scott Kitterman. |
| `rfc7208-tests.CHANGES` | Its changelog, which says what each release of the suite changed. |

The suite was published at <http://www.openspf.org/Test_Suite>. That site no
longer serves it; these copies were taken from pyspf, which carries the suite
and its own driver for it:

```
https://raw.githubusercontent.com/sdgathman/pyspf/master/test/rfc7208-tests.yml
https://raw.githubusercontent.com/sdgathman/pyspf/master/test/rfc7208-tests.LICENSE
https://raw.githubusercontent.com/sdgathman/pyspf/master/test/rfc7208-tests.CHANGES
```

The three files are vendored unchanged. The licence asks that the copyright
notice be retained, which is what `rfc7208-tests.LICENSE` is for; keep it
alongside the suite, and do not edit `rfc7208-tests.yml` — a local fix to a case
would quietly stop this from being the suite everyone else runs.

## The generated table

Nothing in hMailServer reads YAML, and adding a parser to the server build in
order to read a test fixture would be the wrong trade. So the suite is
transcribed into a C++ table instead:

```
python3 Generate-SPFConformanceSuite.py
```

That rewrites `SPFConformanceSuite.cpp`, which is **committed**. Committing it is
what keeps Python and PyYAML out of the MSVC build: the generator only has to
run again when the vendored suite is updated. It is deterministic - the same
input produces the same file - so a regeneration that changes nothing produces
no diff.

Two parts of the transcription are decisions rather than copying, and the
generator's own comments give them in full:

- **Records of type SPF.** The suite predates RFC 7208 and publishes most of its
  policies as records of the deprecated type SPF (RR type 99), expecting the
  driver to present them as the TXT records that an evaluator following RFC 7208
  would find. It does not expect that everywhere: the "Selecting records"
  section exists to check that type SPF is ignored. The rule that satisfies
  both, and the one the suite's own driver uses, is per host - a host's
  type-SPF records stand in for a TXT record only where that host publishes no
  TXT record of its own. The generator applies it, so the emitted table holds no
  record of type SPF at all.
- **Timeouts.** A zone can be marked as not answering. In the suite's driver the
  effect depends on where the marker sits among the zone's records; in this file
  it is always last, which the generator asserts, so the table can carry it as a
  flag on the zone.

## Running the suite

The runner is part of the portable build, which builds the Windows-free part of
the server with gcc or clang and runs it under the address and
undefined-behaviour sanitizers:

```
cmake -S . -B out/portable -G Ninja -DCMAKE_BUILD_TYPE=Debug -DHM_SANITIZE=ON
cmake --build out/portable
ctest --test-dir out/portable --output-on-failure
```

`SPFConformanceLookup` answers an evaluation's DNS queries out of one section of
the table, through the `SPFDnsLookup` interface the evaluator takes, so a case
runs in microseconds and never reaches a network.

A failing case is named by the key it is written under in `rfc7208-tests.yml`.
Its description, and the commentary on what it is for and why RFC 7208 says what
it does, are there; they are deliberately not duplicated into the table.
