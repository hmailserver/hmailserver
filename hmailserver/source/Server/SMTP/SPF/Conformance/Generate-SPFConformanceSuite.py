# Copyright (c) 2010 Martin Knafve / hMailServer.com.
# http://www.hmailserver.com

"""Turns rfc7208-tests.yml into the C++ table in SPFConformanceSuite.cpp.

The test suite is distributed as YAML. Nothing in hMailServer reads YAML, and
adding a parser to the server build to read a test fixture would be the wrong
trade, so the suite is transcribed into a C++ table instead. The table is
committed, which is what keeps Python out of the MSVC build: this script only
has to run again when the vendored suite is updated.

    python3 Generate-SPFConformanceSuite.py

It rewrites SPFConformanceSuite.cpp in place and prints what it produced.
Requires PyYAML.

Two parts of the transcription are decisions rather than copying, and both are
taken from the way the suite's own driver (testspf.py in pyspf) reads the file.

Record type SPF
---------------
The suite predates RFC 7208 and publishes most of its policies as records of
the deprecated type SPF (RR type 99) rather than as TXT records. RFC 7208
section 3.1 removed type SPF, and an evaluator following it queries TXT only.
The suite's own note says its drivers are expected to duplicate SPF records to
TXT, except in the "Selecting records" section, which exists to check that an
implementation ignores type SPF.

The rule that satisfies both, and the one testspf.py uses, is per host: a
host's type-SPF records are copied to TXT only if that host publishes no TXT
record of its own. A "TXT: NONE" entry counts as publishing one - it carries no
record itself, it is how the suite says "this host has no TXT record, and do
not invent one".

This script applies that rule, so the emitted table holds no record of type
SPF at all, and the stub resolver does not have to know that the type ever
existed.

Timeouts
--------
A bare "TIMEOUT" entry in a host's record list means queries to that host time
out. In testspf.py the effect depends on where in the list the entry sits: the
lookup walks the records in order and fails only if it reaches the TIMEOUT
without having passed a record of the type being queried. In this file every
TIMEOUT is the last entry, which this script asserts, so the position carries
no information and the table can hold it as a flag on the zone: a query for a
type the zone has no record of times out instead of returning an empty answer.
"""

import collections
import io
import os
import sys

try:
   import yaml
except ImportError:
   sys.exit("PyYAML is required: pip3 install pyyaml")

HERE = os.path.dirname(os.path.abspath(__file__))
SUITE = os.path.join(HERE, "rfc7208-tests.yml")
OUTPUT = os.path.join(HERE, "SPFConformanceSuite.cpp")

# The suite spells the results of RFC 7208 section 2.6 in lower case.
RESULTS = {
   "none": "None",
   "neutral": "Neutral",
   "pass": "Pass",
   "fail": "Fail",
   "softfail": "SoftFail",
   "temperror": "TempError",
   "permerror": "PermError",
}

# Record types the table can hold. Type SPF is absent by design; see above.
RECORD_TYPES = ("A", "AAAA", "CNAME", "MX", "PTR", "TXT")

# The explanation the suite's driver installs as the receiver's own default,
# and compares against when a case expects no explanation from the record.
DEFAULT_EXPLANATION = "DEFAULT"


def as_strings(value):
   """A TXT or SPF record's character-strings, as a list."""
   if isinstance(value, str):
      return [value]
   return list(value)


def host_records(host, entries):
   """The records a host answers with, and whether it times out.

   Mirrors getrdata() in testspf.py, and drops what an RFC 7208 evaluator
   never sees: records of type SPF, and the "NONE" sentinel.
   """
   records = []
   generated = []
   duplicate_spf_to_txt = True
   times_out = False

   for index, entry in enumerate(entries):
      if entry == "TIMEOUT":
         # Every TIMEOUT in the file is last, which is what lets the table
         # hold it as a flag rather than as a position in the record list.
         if index != len(entries) - 1:
            sys.exit("%s: TIMEOUT is not the last record; the flag on Zone "
                     "cannot express that" % host)
         times_out = True
         continue

      (record_type, value), = entry.items()
      preference = None

      if record_type not in RECORD_TYPES and record_type != "SPF":
         sys.exit("%s: unknown record type %s" % (host, record_type))

      if record_type in ("TXT", "SPF"):
         strings = as_strings(value)
      elif record_type == "MX":
         # [preference, host name].
         preference, strings = value[0], [value[1]]
      else:
         strings = [value]

      if record_type == "TXT":
         duplicate_spf_to_txt = False
      elif record_type == "SPF":
         if strings == ["NONE"]:
            sys.exit("%s: SPF: NONE is not handled" % host)
         if duplicate_spf_to_txt:
            generated.append(("TXT", strings, None))
         # Never emitted as itself: an RFC 7208 evaluator does not query
         # type SPF.
         continue

      if record_type == "TXT" and strings == ["NONE"]:
         # Says only that the host publishes no TXT record.
         continue

      records.append((record_type, strings, preference))

   if duplicate_spf_to_txt:
      records.extend(generated)

   # DNSResolver hands MX records to its callers in preference order, so the
   # table holds them that way too. The sort is stable, so hosts sharing a
   # preference keep the order the suite wrote them in. The MX records take
   # the positions the MX records already occupied, which leaves the rest of
   # the list where it was.
   positions = [i for i, record in enumerate(records) if record[0] == "MX"]
   in_order = sorted((records[i] for i in positions), key=lambda r: r[2])
   for position, record in zip(positions, in_order):
      records[position] = record

   return records, times_out


def c_string(value):
   """A C string literal holding the bytes of value.

   The suite's records are byte strings, not text: several hold bytes above
   0x7f deliberately, because RFC 7208 section 3.1 restricts a policy to
   7-bit ASCII and an evaluator has to reject the rest. Those bytes are
   written as hex escapes, and the literal is broken around each one so that
   a following hex digit cannot be read as part of the escape.
   """
   pieces = []
   plain = []

   def flush():
      if plain:
         pieces.append('"%s"' % "".join(plain))
         del plain[:]

   for byte in value.encode("utf-8"):
      character = chr(byte)

      if byte < 0x20 or byte >= 0x7F:
         flush()
         pieces.append('"\\x%02x"' % byte)
      elif character in ('"', "\\"):
         plain.append("\\" + character)
      elif character == "?":
         # So that no run of question marks can form a trigraph.
         plain.append("\\?")
      else:
         plain.append(character)

   flush()

   return " ".join(pieces) if pieces else '""'


def c_text(value, what):
   """A C string literal for a value the table holds as a C string.

   Only a record's character-strings carry a length alongside them, because
   only a record is allowed to hold a NUL octet. Everything else - a host name,
   a case's sender, an explanation - would lose anything from the NUL on
   without saying so, so it may not contain one.
   """
   if "\0" in value:
      sys.exit("%s: holds a NUL octet, which a C string cannot carry" % what)

   return c_string(value)


def main():
   with open(SUITE, "rb") as stream:
      sections = list(yaml.safe_load_all(stream))

   out = io.StringIO()
   counts = collections.Counter()
   case_total = 0

   out.write("""// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// GENERATED FILE - do not edit.
//
// Produced by Generate-SPFConformanceSuite.py from rfc7208-tests.yml, the
// openspf.org test suite for RFC 7208. See README.md in this directory for
// where the suite comes from and how to regenerate this file, and the
// generator for the two places where the transcription is a decision rather
// than a copy: how records of the deprecated type SPF become TXT records, and
// how a timed-out zone is expressed.
//
// A failing case is identified by its name, which is the key it is written
// under in rfc7208-tests.yml; the description and commentary are there rather
// than duplicated here.

#include "stdafx.h"

#include "SPFConformanceSuite.h"

namespace HM
{
   namespace SPFConformance
   {
      const char *
      ResultName(Result result)
      {
         switch (result)
         {
         case Result::None:
            return "none";
         case Result::Neutral:
            return "neutral";
         case Result::Pass:
            return "pass";
         case Result::Fail:
            return "fail";
         case Result::SoftFail:
            return "softfail";
         case Result::TempError:
            return "temperror";
         case Result::PermError:
            return "permerror";
         }

         return "?";
      }

      namespace
      {
""")

   section_rows = []

   for section_index, section in enumerate(sections):
      if sorted(section.keys()) not in (["description", "tests", "zonedata"],
                                        ["comment", "description", "tests", "zonedata"]):
         sys.exit("unexpected keys in section %d: %s" % (section_index, sorted(section.keys())))

      prefix = "s%d" % section_index
      out.write("         // %s\n" % section["description"])

      zone_rows = []
      for zone_index, (host, entries) in enumerate(section["zonedata"].items()):
         records, times_out = host_records(host, entries)
         counts.update(record[0] for record in records)
         if times_out:
            counts["TIMEOUT"] += 1

         record_rows = []
         for record_index, (record_type, strings, _) in enumerate(records):
            if not strings:
               # A TXT record made of no character-strings at all. There is no
               # array to point at, and nothing would read it.
               record_rows.append("            { RecordType::%s, 0, 0 },"
                                  % record_type)
               continue

            name = "%s_z%d_r%d" % (prefix, zone_index, record_index)
            out.write("         const CharacterString %s[] = { %s };\n" %
                      (name, ", ".join(
                         "{ %s, %d }" % (c_string(s), len(s.encode("utf-8")))
                         for s in strings)))
            record_rows.append("            { RecordType::%s, %s, %d },"
                               % (record_type, name, len(strings)))

         records_name = "%s_z%d_records" % (prefix, zone_index)
         if record_rows:
            out.write("         const Record %s[] = {\n%s\n         };\n"
                      % (records_name, "\n".join(record_rows)))
         else:
            # A zone can hold no record at all once type SPF is dropped, and
            # a zero-length array is not valid C++.
            out.write("         const Record %s[] = { { RecordType::TXT, 0, 0 } };\n"
                      % records_name)

         zone_rows.append("            { %s, %s, %d, %s },"
                          % (c_text(host, "zone %s" % host), records_name,
                             len(records), "true" if times_out else "false"))

      zones_name = "%s_zones" % prefix
      out.write("         const Zone %s[] = {\n%s\n         };\n\n"
                % (zones_name, "\n".join(zone_rows)))

      case_rows = []
      for case_index, (name, case) in enumerate(section["tests"].items()):
         unexpected = set(case.keys()) - {
            "comment", "description", "explanation", "helo", "host",
            "mailfrom", "result", "spec", "strict"}
         if unexpected:
            sys.exit("%s: unexpected keys %s" % (name, sorted(unexpected)))

         # "strict" appears on one case and selects a mode private to pyspf,
         # the implementation the suite grew up with. It says nothing about
         # RFC 7208, so it is dropped; the case runs like any other, and its
         # expected result does not depend on the mode.
         results = case["result"]
         if isinstance(results, str):
            results = [results]
         for result in results:
            if result not in RESULTS:
               sys.exit("%s: unknown result %s" % (name, result))

         results_name = "%s_c%d_results" % (prefix, case_index)
         out.write("         const Result %s[] = { %s };\n"
                   % (results_name,
                      ", ".join("Result::" + RESULTS[r] for r in results)))

         explanation = case.get("explanation")
         if explanation is None:
            explanation_literal = "0"
         elif explanation == DEFAULT_EXPLANATION:
            # The case asserts that the record supplied no explanation and the
            # receiver fell back on its own. hMailServer reports that as an
            # empty explanation.
            explanation_literal = '""'
         else:
            explanation_literal = c_text(explanation, "%s explanation" % name)

         case_rows.append("            { %s, %s, %s, %s, %s,\n"
                          "              %s, %d, %s },"
                          % (c_text(name, name),
                             c_text(str(case["spec"]), "%s spec" % name),
                             c_text(case["host"], "%s host" % name),
                             c_text(case["mailfrom"], "%s sender" % name),
                             c_text(case["helo"], "%s HELO host" % name),
                             results_name, len(results), explanation_literal))
         case_total += 1

      cases_name = "%s_cases" % prefix
      out.write("         const Case %s[] = {\n%s\n         };\n\n"
                % (cases_name, "\n".join(case_rows)))

      section_rows.append("         { %s,\n           %s, %d, %s, %d },"
                          % (c_text(section["description"], "section description"),
                             zones_name, len(section["zonedata"]),
                             cases_name, len(section["tests"])))

   out.write("      }\n\n")
   out.write("      extern const Section Sections[] = {\n%s\n      };\n\n"
             % "\n".join(section_rows))
   out.write("      extern const int SectionCount = %d;\n" % len(sections))
   out.write("      extern const int CaseCount = %d;\n" % case_total)
   out.write("   }\n}\n")

   with open(OUTPUT, "w", newline="\n") as stream:
      stream.write(out.getvalue())

   print("%s: %d sections, %d cases, %d zones" %
         (os.path.basename(OUTPUT), len(sections), case_total,
          sum(len(s["zonedata"]) for s in sections)))
   print("records: %s" % ", ".join("%s %d" % (k, counts[k])
                                   for k in sorted(counts)))


if __name__ == "__main__":
   main()
