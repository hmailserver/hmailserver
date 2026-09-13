# Fuzzing the SPF parser and macro expander

Both of these read bytes chosen by whichever domain a message claims to be from:
the parser reads a TXT record, and the macro expander reads a term's target and,
for an `exp` modifier, a second TXT record that nothing has parsed before it.

What the grammar tests, `SPFMacroExpanderTester` and the 203 conformance cases
say is what those bytes are *decided* to mean. What a fuzzer says is that some
decision is always reached, on any input, without reading past the end of it.

## Running

Needs clang and its fuzzer runtime — on Ubuntu, `libclang-rt-<version>-dev`.

```
cmake -S . -B out/fuzz -G Ninja -DCMAKE_BUILD_TYPE=Debug \
   -DHM_FUZZ=ON -DCMAKE_CXX_COMPILER=clang++
cmake --build out/fuzz

out/fuzz/fuzz-spf-record          <findings dir> hmailserver/source/Server/Portable/Fuzz/corpus/record
out/fuzz/fuzz-spf-macro-expander  <findings dir> hmailserver/source/Server/Portable/Fuzz/corpus/macro-expander
```

The first directory is where new inputs are written; the second is the corpus and
is only read. `-max_total_time=<seconds>` bounds a run.

Deliberately not a ctest case. A fuzzer has no pass, only a not-yet-failed, so a
run over a fixed number of inputs would add nothing to the suites that already
run on every build - and it is slow enough to be worth choosing to do.

## The corpus

`corpus/` holds the SPF records and macro-strings of the vendored conformance
suite, pulled out of `rfc7208-tests.yml`. They are a better starting point than
anything written by hand, because the suite already put them on the corners of
the grammar; they are still a test fixture rather than the real world.

A corpus of real records is what would settle the two questions the grammar is
knowingly lenient about, both noted in `SMTP/SPF/README.md`: a zero digit transformer
(`%{d0}`), which this accepts, and an unterminated `%{`, which it rejects. The
suite tests neither, so neither is settled by anything here.

## The macro expander's input

The expansion depends on the sender and the HELO argument as much as on the
macro-string, and both come off the wire. So the input is split: a length-prefixed
client address, sender, HELO argument and domain are taken off the front, and what
is left is the macro-string. That reaches the cases where the two interact - a
transformer counting the parts of a local part that has none - and not only the
ones where the macro-string alone is odd.
