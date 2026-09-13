// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// GENERATED FILE - do not edit. Produced by Generate-SPFConformanceSuite.py from
// rfc7208-tests.yml, the openspf.org test suite for RFC 7208. README.md says where
// the suite comes from and how to regenerate this file.

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
         // Initial processing
         const Record s0_z0_records[] = { { RecordType::TXT, 0, 0 } };
         const CharacterString s0_z1_r0[] = { { "v=spf1 -all exp=exp.example.net", 31 } };
         const Record s0_z1_records[] = {
            { RecordType::TXT, s0_z1_r0, 1 },
         };
         const CharacterString s0_z2_r0[] = { { "v=spf1 -all exp=exp.example.net", 31 } };
         const Record s0_z2_records[] = {
            { RecordType::TXT, s0_z2_r0, 1 },
         };
         const CharacterString s0_z3_r0[] = { { "%{l}", 4 } };
         const Record s0_z3_records[] = {
            { RecordType::TXT, s0_z3_r0, 1 },
         };
         const CharacterString s0_z4_r0[] = { { "v=spf1 -all", 11 } };
         const Record s0_z4_records[] = {
            { RecordType::TXT, s0_z4_r0, 1 },
         };
         const CharacterString s0_z5_r0[] = { { "v=spf1 a:" "\xc3" "\xaf" "\xc2" "\xbb" "\xc2" "\xbf" "garbage.example.net -all", 39 } };
         const Record s0_z5_records[] = {
            { RecordType::TXT, s0_z5_r0, 1 },
         };
         const CharacterString s0_z6_r0[] = { { "v=spf1 " "\xc2" "\x80" "a:example.net -all", 27 } };
         const Record s0_z6_records[] = {
            { RecordType::TXT, s0_z6_r0, 1 },
         };
         const CharacterString s0_z7_r0[] = { { "v=spf1 a:example.net " "\xc2" "\x96" "all", 26 } };
         const Record s0_z7_records[] = {
            { RecordType::TXT, s0_z7_r0, 1 },
         };
         const CharacterString s0_z8_r0[] = { { "v=spf1 a:example.net -all", 25 } };
         const CharacterString s0_z8_r1[] = { { "\xc2" "\x96", 2 } };
         const Record s0_z8_records[] = {
            { RecordType::TXT, s0_z8_r0, 1 },
            { RecordType::TXT, s0_z8_r1, 1 },
         };
         const CharacterString s0_z9_r0[] = { { "192.0.2.3", 9 } };
         const CharacterString s0_z9_r1[] = { { "v=spf1 a:ctrl.example.com" "\x0d" "ptr -all", 34 } };
         const Record s0_z9_records[] = {
            { RecordType::A, s0_z9_r0, 1 },
            { RecordType::TXT, s0_z9_r1, 1 },
         };
         const CharacterString s0_z10_r0[] = { { "v=spf1 a  -all", 14 } };
         const Record s0_z10_records[] = {
            { RecordType::TXT, s0_z10_r0, 1 },
         };
         const CharacterString s0_z11_r0[] = { { "v=spf1 a -all ", 14 } };
         const Record s0_z11_records[] = {
            { RecordType::TXT, s0_z11_r0, 1 },
         };
         const CharacterString s0_z12_r0[] = { { "v=spf1 ip4:", 11 }, { "192.0.2.5 -all", 14 } };
         const Record s0_z12_records[] = {
            { RecordType::TXT, s0_z12_r0, 2 },
            { RecordType::TXT, 0, 0 },
         };
         const CharacterString s0_z13_r0[] = { { "v=spf1 ip4:192.0.2.5" "\x0a" " include:spf.protection.outlook.com ~all", 61 } };
         const Record s0_z13_records[] = {
            { RecordType::TXT, s0_z13_r0, 1 },
         };
         const Zone s0_zones[] = {
            { "example.com", s0_z0_records, 0, true },
            { "example.net", s0_z1_records, 1, false },
            { "a.example.net", s0_z2_records, 1, false },
            { "exp.example.net", s0_z3_records, 1, false },
            { "a12345678901234567890123456789012345678901234567890123456789012.example.com", s0_z4_records, 1, false },
            { "hosed.example.com", s0_z5_records, 1, false },
            { "hosed2.example.com", s0_z6_records, 1, false },
            { "hosed3.example.com", s0_z7_records, 1, false },
            { "nothosed.example.com", s0_z8_records, 2, false },
            { "ctrl.example.com", s0_z9_records, 2, false },
            { "fine.example.com", s0_z10_records, 1, false },
            { "trail.example.com", s0_z11_records, 1, false },
            { "null.example.com", s0_z12_records, 2, false },
            { "badip.example.com", s0_z13_records, 1, false },
         };

         const Result s0_c0_results[] = { Result::None };
         const Result s0_c1_results[] = { Result::Fail };
         const Result s0_c2_results[] = { Result::None };
         const Result s0_c3_results[] = { Result::None };
         const Result s0_c4_results[] = { Result::None };
         const Result s0_c5_results[] = { Result::Fail };
         const Result s0_c6_results[] = { Result::None };
         const Result s0_c7_results[] = { Result::PermError };
         const Result s0_c8_results[] = { Result::PermError };
         const Result s0_c9_results[] = { Result::PermError };
         const Result s0_c10_results[] = { Result::Fail };
         const Result s0_c11_results[] = { Result::PermError };
         const Result s0_c12_results[] = { Result::Fail };
         const Result s0_c13_results[] = { Result::Fail };
         const Result s0_c14_results[] = { Result::Pass };
         const Result s0_c15_results[] = { Result::PermError };
         const Case s0_cases[] = {
            { "toolonglabel", "4.3/1", "1.2.3.5", "lyme.eater@A123456789012345678901234567890123456789012345678901234567890123.example.com", "mail.example.net",
              s0_c0_results, 1, 0 },
            { "longlabel", "4.3/1", "1.2.3.5", "lyme.eater@A12345678901234567890123456789012345678901234567890123456789012.example.com", "mail.example.net",
              s0_c1_results, 1, 0 },
            { "emptylabel", "4.3/1", "1.2.3.5", "lyme.eater@A...example.com", "mail.example.net",
              s0_c2_results, 1, 0 },
            { "helo-not-fqdn", "4.3/1", "1.2.3.5", "", "A2345678",
              s0_c3_results, 1, 0 },
            { "helo-domain-literal", "4.3/1", "1.2.3.5", "", "[1.2.3.5]",
              s0_c4_results, 1, 0 },
            { "nolocalpart", "4.3/2", "1.2.3.4", "@example.net", "mail.example.net",
              s0_c5_results, 1, "postmaster" },
            { "domain-literal", "4.3/1", "1.2.3.5", "foo@[1.2.3.5]", "OEMCOMPUTER",
              s0_c6_results, 1, 0 },
            { "non-ascii-policy", "3.1/1", "1.2.3.4", "foobar@hosed.example.com", "hosed",
              s0_c7_results, 1, 0 },
            { "non-ascii-mech", "3.1/1", "1.2.3.4", "foobar@hosed2.example.com", "hosed",
              s0_c8_results, 1, 0 },
            { "non-ascii-result", "3.1/1", "1.2.3.4", "foobar@hosed3.example.com", "hosed",
              s0_c9_results, 1, 0 },
            { "non-ascii-non-spf", "4.5/1", "1.2.3.4", "foobar@nothosed.example.com", "hosed",
              s0_c10_results, 1, "" },
            { "control-char-policy", "4.6.1/2", "192.0.2.3", "foobar@ctrl.example.com", "hosed",
              s0_c11_results, 1, 0 },
            { "two-spaces", "4.6.1", "1.2.3.4", "actually@fine.example.com", "hosed",
              s0_c12_results, 1, 0 },
            { "trailing-space", "4.5/2", "192.0.2.5", "silly@trail.example.com", "hosed",
              s0_c13_results, 1, 0 },
            { "null-text", "3.3", "192.0.2.5", "silly@null.example.com", "hosed",
              s0_c14_results, 1, 0 },
            { "badip4", "4.6.1/2", "192.0.2.5", "oops@badip.example.com", "foobar",
              s0_c15_results, 1, 0 },
         };

         // Record lookup
         const CharacterString s1_z0_r0[] = { { "v=spf1 -all", 11 } };
         const Record s1_z0_records[] = {
            { RecordType::TXT, s1_z0_r0, 1 },
         };
         const CharacterString s1_z1_r0[] = { { "v=spf1 -all", 11 } };
         const Record s1_z1_records[] = {
            { RecordType::TXT, s1_z1_r0, 1 },
         };
         const Record s1_z2_records[] = { { RecordType::TXT, 0, 0 } };
         const CharacterString s1_z3_r0[] = { { "v=spf1 -all", 11 } };
         const Record s1_z3_records[] = {
            { RecordType::TXT, s1_z3_r0, 1 },
         };
         const Record s1_z4_records[] = { { RecordType::TXT, 0, 0 } };
         const Record s1_z5_records[] = { { RecordType::TXT, 0, 0 } };
         const Record s1_z6_records[] = { { RecordType::TXT, 0, 0 } };
         const Zone s1_zones[] = {
            { "both.example.net", s1_z0_records, 1, false },
            { "txtonly.example.net", s1_z1_records, 1, false },
            { "spfonly.example.net", s1_z2_records, 0, false },
            { "spftimeout.example.net", s1_z3_records, 1, true },
            { "txttimeout.example.net", s1_z4_records, 0, true },
            { "nospftxttimeout.example.net", s1_z5_records, 0, true },
            { "alltimeout.example.net", s1_z6_records, 0, true },
         };

         const Result s1_c0_results[] = { Result::Fail };
         const Result s1_c1_results[] = { Result::Fail };
         const Result s1_c2_results[] = { Result::None };
         const Result s1_c3_results[] = { Result::Fail };
         const Result s1_c4_results[] = { Result::TempError };
         const Result s1_c5_results[] = { Result::TempError };
         const Result s1_c6_results[] = { Result::TempError };
         const Case s1_cases[] = {
            { "both", "4.4/1", "1.2.3.4", "foo@both.example.net", "mail.example.net",
              s1_c0_results, 1, 0 },
            { "txtonly", "4.4/1", "1.2.3.4", "foo@txtonly.example.net", "mail.example.net",
              s1_c1_results, 1, 0 },
            { "spfonly", "4.4/1", "1.2.3.4", "foo@spfonly.example.net", "mail.example.net",
              s1_c2_results, 1, 0 },
            { "spftimeout", "4.4/1", "1.2.3.4", "foo@spftimeout.example.net", "mail.example.net",
              s1_c3_results, 1, 0 },
            { "txttimeout", "4.4/1", "1.2.3.4", "foo@txttimeout.example.net", "mail.example.net",
              s1_c4_results, 1, 0 },
            { "nospftxttimeout", "4.4/1", "1.2.3.4", "foo@nospftxttimeout.example.net", "mail.example.net",
              s1_c5_results, 1, 0 },
            { "alltimeout", "4.4/2", "1.2.3.4", "foo@alltimeout.example.net", "mail.example.net",
              s1_c6_results, 1, 0 },
         };

         // Selecting records
         const CharacterString s2_z0_r0[] = { { "mail.example1.com", 17 } };
         const CharacterString s2_z0_r1[] = { { "v=spf10", 7 } };
         const CharacterString s2_z0_r2[] = { { "v=spf1 mx", 9 } };
         const Record s2_z0_records[] = {
            { RecordType::MX, s2_z0_r0, 1 },
            { RecordType::TXT, s2_z0_r1, 1 },
            { RecordType::TXT, s2_z0_r2, 1 },
         };
         const CharacterString s2_z1_r0[] = { { "v=spf1", 6 } };
         const Record s2_z1_records[] = {
            { RecordType::TXT, s2_z1_r0, 1 },
         };
         const CharacterString s2_z2_r0[] = { { "v=spf1", 6 }, { "mx", 2 } };
         const Record s2_z2_records[] = {
            { RecordType::TXT, s2_z2_r0, 2 },
         };
         const CharacterString s2_z3_r0[] = { { "1.2.3.4", 7 } };
         const Record s2_z3_records[] = {
            { RecordType::A, s2_z3_r0, 1 },
         };
         const CharacterString s2_z4_r0[] = { { "v=spf1 -all", 11 } };
         const Record s2_z4_records[] = {
            { RecordType::TXT, s2_z4_r0, 1 },
         };
         const CharacterString s2_z5_r0[] = { { "v=spf1 -all", 11 } };
         const CharacterString s2_z5_r1[] = { { "v=spf1 +all", 11 } };
         const Record s2_z5_records[] = {
            { RecordType::TXT, s2_z5_r0, 1 },
            { RecordType::TXT, s2_z5_r1, 1 },
         };
         const CharacterString s2_z6_r0[] = { { "v=spf1 -all", 11 } };
         const CharacterString s2_z6_r1[] = { { "V=sPf1 +all", 11 } };
         const Record s2_z6_records[] = {
            { RecordType::TXT, s2_z6_r0, 1 },
            { RecordType::TXT, s2_z6_r1, 1 },
         };
         const CharacterString s2_z7_r0[] = { { "v=spf1 -all", 11 } };
         const CharacterString s2_z7_r1[] = { { "v=spf1 -all", 11 } };
         const Record s2_z7_records[] = {
            { RecordType::TXT, s2_z7_r0, 1 },
            { RecordType::TXT, s2_z7_r1, 1 },
         };
         const CharacterString s2_z8_r0[] = { { "v=spf1 +all", 11 } };
         const Record s2_z8_records[] = {
            { RecordType::TXT, s2_z8_r0, 1 },
         };
         const CharacterString s2_z9_r0[] = { { "v=SpF1 ~all", 11 } };
         const Record s2_z9_records[] = {
            { RecordType::TXT, s2_z9_r0, 1 },
         };
         const Zone s2_zones[] = {
            { "example3.com", s2_z0_records, 3, false },
            { "example1.com", s2_z1_records, 1, false },
            { "example2.com", s2_z2_records, 1, false },
            { "mail.example1.com", s2_z3_records, 1, false },
            { "example4.com", s2_z4_records, 1, false },
            { "example5.com", s2_z5_records, 2, false },
            { "example6.com", s2_z6_records, 2, false },
            { "example7.com", s2_z7_records, 2, false },
            { "example8.com", s2_z8_records, 1, false },
            { "example9.com", s2_z9_records, 1, false },
         };

         const Result s2_c0_results[] = { Result::None };
         const Result s2_c1_results[] = { Result::Neutral };
         const Result s2_c2_results[] = { Result::Pass };
         const Result s2_c3_results[] = { Result::Fail };
         const Result s2_c4_results[] = { Result::PermError };
         const Result s2_c5_results[] = { Result::PermError };
         const Result s2_c6_results[] = { Result::PermError, Result::Fail };
         const Result s2_c7_results[] = { Result::Pass };
         const Result s2_c8_results[] = { Result::None };
         const Result s2_c9_results[] = { Result::SoftFail };
         const Case s2_cases[] = {
            { "nospace1", "4.5/4", "1.2.3.4", "foo@example2.com", "mail.example1.com",
              s2_c0_results, 1, 0 },
            { "empty", "4.5/4", "1.2.3.4", "foo@example1.com", "mail1.example1.com",
              s2_c1_results, 1, 0 },
            { "nospace2", "4.5/4", "1.2.3.4", "foo@example3.com", "mail.example1.com",
              s2_c2_results, 1, 0 },
            { "spfoverride", "4.5/5", "1.2.3.4", "foo@example4.com", "mail.example1.com",
              s2_c3_results, 1, 0 },
            { "multitxt1", "4.5/5", "1.2.3.4", "foo@example5.com", "mail.example1.com",
              s2_c4_results, 1, 0 },
            { "multitxt2", "4.5/6", "1.2.3.4", "foo@example6.com", "mail.example1.com",
              s2_c5_results, 1, 0 },
            { "multispf1", "4.5/6", "1.2.3.4", "foo@example7.com", "mail.example1.com",
              s2_c6_results, 2, 0 },
            { "multispf2", "4.5/6", "1.2.3.4", "foo@example8.com", "mail.example1.com",
              s2_c7_results, 1, 0 },
            { "nospf", "4.5/7", "1.2.3.4", "foo@mail.example1.com", "mail.example1.com",
              s2_c8_results, 1, 0 },
            { "case-insensitive", "4.5/6", "1.2.3.4", "foo@example9.com", "mail.example1.com",
              s2_c9_results, 1, 0 },
         };

         // Record evaluation
         const CharacterString s3_z0_r0[] = { { "1.2.3.4", 7 } };
         const Record s3_z0_records[] = {
            { RecordType::A, s3_z0_r0, 1 },
         };
         const CharacterString s3_z1_r0[] = { { "v=spf1 ip4:1.2.3.4 -all moo", 27 } };
         const Record s3_z1_records[] = {
            { RecordType::TXT, s3_z1_r0, 1 },
         };
         const CharacterString s3_z2_r0[] = { { "v=spf1 moo.cow-far_out=man:dog/cat ip4:1.2.3.4 -all", 51 } };
         const Record s3_z2_records[] = {
            { RecordType::TXT, s3_z2_r0, 1 },
         };
         const CharacterString s3_z3_r0[] = { { "v=spf1 moo.cow/far_out=man:dog/cat ip4:1.2.3.4 -all", 51 } };
         const Record s3_z3_records[] = {
            { RecordType::TXT, s3_z3_r0, 1 },
         };
         const CharacterString s3_z4_r0[] = { { "v=spf1 moo.cow:far_out=man:dog/cat ip4:1.2.3.4 -all", 51 } };
         const Record s3_z4_records[] = {
            { RecordType::TXT, s3_z4_r0, 1 },
         };
         const CharacterString s3_z5_r0[] = { { "v=spf1 redirect=t5.example.com ~all", 35 } };
         const Record s3_z5_records[] = {
            { RecordType::TXT, s3_z5_r0, 1 },
         };
         const CharacterString s3_z6_r0[] = { { "v=spf1 ip4:1.2.3.4 redirect=t2.example.com", 42 } };
         const Record s3_z6_records[] = {
            { RecordType::TXT, s3_z6_r0, 1 },
         };
         const CharacterString s3_z7_r0[] = { { "v=spf1 ip4:1.2.3.4", 18 } };
         const Record s3_z7_records[] = {
            { RecordType::TXT, s3_z7_r0, 1 },
         };
         const CharacterString s3_z8_r0[] = { { "v=spf1 ip4:1.2.3.4 redirect:t2.example.com", 42 } };
         const Record s3_z8_records[] = {
            { RecordType::TXT, s3_z8_r0, 1 },
         };
         const CharacterString s3_z9_r0[] = { { "v=spf1 a:foo-bar -all", 21 } };
         const Record s3_z9_records[] = {
            { RecordType::TXT, s3_z9_r0, 1 },
         };
         const CharacterString s3_z10_r0[] = { { "v=spf1 a:mail.example...com -all", 32 } };
         const Record s3_z10_records[] = {
            { RecordType::TXT, s3_z10_r0, 1 },
         };
         const CharacterString s3_z11_r0[] = { { "v=spf1 a:a123456789012345678901234567890123456789012345678901234567890123.example.com -all", 90 } };
         const Record s3_z11_records[] = {
            { RecordType::TXT, s3_z11_r0, 1 },
         };
         const CharacterString s3_z12_r0[] = { { "v=spf1 a:%{H}.bar -all", 22 } };
         const Record s3_z12_records[] = {
            { RecordType::TXT, s3_z12_r0, 1 },
         };
         const Zone s3_zones[] = {
            { "mail.example.com", s3_z0_records, 1, false },
            { "t1.example.com", s3_z1_records, 1, false },
            { "t2.example.com", s3_z2_records, 1, false },
            { "t3.example.com", s3_z3_records, 1, false },
            { "t4.example.com", s3_z4_records, 1, false },
            { "t5.example.com", s3_z5_records, 1, false },
            { "t6.example.com", s3_z6_records, 1, false },
            { "t7.example.com", s3_z7_records, 1, false },
            { "t8.example.com", s3_z8_records, 1, false },
            { "t9.example.com", s3_z9_records, 1, false },
            { "t10.example.com", s3_z10_records, 1, false },
            { "t11.example.com", s3_z11_records, 1, false },
            { "t12.example.com", s3_z12_records, 1, false },
         };

         const Result s3_c0_results[] = { Result::PermError };
         const Result s3_c1_results[] = { Result::Pass };
         const Result s3_c2_results[] = { Result::PermError };
         const Result s3_c3_results[] = { Result::PermError };
         const Result s3_c4_results[] = { Result::SoftFail };
         const Result s3_c5_results[] = { Result::Fail };
         const Result s3_c6_results[] = { Result::Neutral };
         const Result s3_c7_results[] = { Result::PermError };
         const Result s3_c8_results[] = { Result::PermError };
         const Result s3_c9_results[] = { Result::Fail, Result::PermError };
         const Result s3_c10_results[] = { Result::Fail, Result::PermError };
         const Result s3_c11_results[] = { Result::Fail, Result::PermError };
         const Case s3_cases[] = {
            { "detect-errors-anywhere", "4.6", "1.2.3.4", "foo@t1.example.com", "mail.example.com",
              s3_c0_results, 1, 0 },
            { "modifier-charset-good", "4.6.1/2", "1.2.3.4", "foo@t2.example.com", "mail.example.com",
              s3_c1_results, 1, 0 },
            { "modifier-charset-bad1", "4.6.1/4", "1.2.3.4", "foo@t3.example.com", "mail.example.com",
              s3_c2_results, 1, 0 },
            { "modifier-charset-bad2", "4.6.1/4", "1.2.3.4", "foo@t4.example.com", "mail.example.com",
              s3_c3_results, 1, 0 },
            { "redirect-after-mechanisms1", "4.6.3", "1.2.3.4", "foo@t5.example.com", "mail.example.com",
              s3_c4_results, 1, 0 },
            { "redirect-after-mechanisms2", "4.6.3", "1.2.3.5", "foo@t6.example.com", "mail.example.com",
              s3_c5_results, 1, 0 },
            { "default-result", "4.7/1", "1.2.3.5", "foo@t7.example.com", "mail.example.com",
              s3_c6_results, 1, 0 },
            { "redirect-is-modifier", "4.6.1/4", "1.2.3.4", "foo@t8.example.com", "mail.example.com",
              s3_c7_results, 1, 0 },
            { "invalid-domain", "7.1/2", "1.2.3.4", "foo@t9.example.com", "mail.example.com",
              s3_c8_results, 1, 0 },
            { "invalid-domain-empty-label", "4.3/1, 4.8/5, 5/10/3", "1.2.3.4", "foo@t10.example.com", "mail.example.com",
              s3_c9_results, 2, 0 },
            { "invalid-domain-long", "4.3/1, 4.8/5, 5/10/3", "1.2.3.4", "foo@t11.example.com", "mail.example.com",
              s3_c10_results, 2, 0 },
            { "invalid-domain-long-via-macro", "4.3/1, 4.8/5, 5/10/3", "1.2.3.4", "foo@t12.example.com", "%%%%%%%%%%%%%%%%%%%%%%",
              s3_c11_results, 2, 0 },
         };

         // ALL mechanism syntax
         const CharacterString s4_z0_r0[] = { { "1.2.3.4", 7 } };
         const Record s4_z0_records[] = {
            { RecordType::A, s4_z0_r0, 1 },
         };
         const CharacterString s4_z1_r0[] = { { "v=spf1 -all.", 12 } };
         const Record s4_z1_records[] = {
            { RecordType::TXT, s4_z1_r0, 1 },
         };
         const CharacterString s4_z2_r0[] = { { "v=spf1 -all:foobar", 18 } };
         const Record s4_z2_records[] = {
            { RecordType::TXT, s4_z2_r0, 1 },
         };
         const CharacterString s4_z3_r0[] = { { "v=spf1 -all/8", 13 } };
         const Record s4_z3_records[] = {
            { RecordType::TXT, s4_z3_r0, 1 },
         };
         const CharacterString s4_z4_r0[] = { { "v=spf1 \?all", 11 } };
         const Record s4_z4_records[] = {
            { RecordType::TXT, s4_z4_r0, 1 },
         };
         const CharacterString s4_z5_r0[] = { { "v=spf1 all -all", 15 } };
         const Record s4_z5_records[] = {
            { RecordType::TXT, s4_z5_r0, 1 },
         };
         const Zone s4_zones[] = {
            { "mail.example.com", s4_z0_records, 1, false },
            { "e1.example.com", s4_z1_records, 1, false },
            { "e2.example.com", s4_z2_records, 1, false },
            { "e3.example.com", s4_z3_records, 1, false },
            { "e4.example.com", s4_z4_records, 1, false },
            { "e5.example.com", s4_z5_records, 1, false },
         };

         const Result s4_c0_results[] = { Result::PermError };
         const Result s4_c1_results[] = { Result::PermError };
         const Result s4_c2_results[] = { Result::PermError };
         const Result s4_c3_results[] = { Result::Neutral };
         const Result s4_c4_results[] = { Result::Pass };
         const Case s4_cases[] = {
            { "all-dot", "5.1/1", "1.2.3.4", "foo@e1.example.com", "mail.example.com",
              s4_c0_results, 1, 0 },
            { "all-arg", "5.1/1", "1.2.3.4", "foo@e2.example.com", "mail.example.com",
              s4_c1_results, 1, 0 },
            { "all-cidr", "5.1/1", "1.2.3.4", "foo@e3.example.com", "mail.example.com",
              s4_c2_results, 1, 0 },
            { "all-neutral", "5.1/1", "1.2.3.4", "foo@e4.example.com", "mail.example.com",
              s4_c3_results, 1, 0 },
            { "all-double", "5.1/1", "1.2.3.4", "foo@e5.example.com", "mail.example.com",
              s4_c4_results, 1, 0 },
         };

         // PTR mechanism syntax
         const CharacterString s5_z0_r0[] = { { "1.2.3.4", 7 } };
         const CharacterString s5_z0_r1[] = { { "2001:db8::1", 11 } };
         const Record s5_z0_records[] = {
            { RecordType::A, s5_z0_r0, 1 },
            { RecordType::AAAA, s5_z0_r1, 1 },
         };
         const CharacterString s5_z1_r0[] = { { "v=spf1 ptr/0 -all", 17 } };
         const Record s5_z1_records[] = {
            { RecordType::TXT, s5_z1_r0, 1 },
         };
         const CharacterString s5_z2_r0[] = { { "v=spf1 ptr:example.com -all", 27 } };
         const Record s5_z2_records[] = {
            { RecordType::TXT, s5_z2_r0, 1 },
         };
         const CharacterString s5_z3_r0[] = { { "e3.example.com", 14 } };
         const CharacterString s5_z3_r1[] = { { "e4.example.com", 14 } };
         const CharacterString s5_z3_r2[] = { { "mail.example.com", 16 } };
         const Record s5_z3_records[] = {
            { RecordType::PTR, s5_z3_r0, 1 },
            { RecordType::PTR, s5_z3_r1, 1 },
            { RecordType::PTR, s5_z3_r2, 1 },
         };
         const CharacterString s5_z4_r0[] = { { "e3.example.com", 14 } };
         const Record s5_z4_records[] = {
            { RecordType::PTR, s5_z4_r0, 1 },
         };
         const CharacterString s5_z5_r0[] = { { "mail.Example.com", 16 } };
         const Record s5_z5_records[] = {
            { RecordType::PTR, s5_z5_r0, 1 },
         };
         const CharacterString s5_z6_r0[] = { { "1.2.3.4", 7 } };
         const CharacterString s5_z6_r1[] = { { "CAFE:BABE::1", 12 } };
         const CharacterString s5_z6_r2[] = { { "v=spf1 ptr -all", 15 } };
         const Record s5_z6_records[] = {
            { RecordType::A, s5_z6_r0, 1 },
            { RecordType::AAAA, s5_z6_r1, 1 },
            { RecordType::TXT, s5_z6_r2, 1 },
         };
         const CharacterString s5_z7_r0[] = { { "v=spf1 ptr -all", 15 } };
         const Record s5_z7_records[] = {
            { RecordType::TXT, s5_z7_r0, 1 },
         };
         const CharacterString s5_z8_r0[] = { { "v=spf1 ptr:", 11 } };
         const Record s5_z8_records[] = {
            { RecordType::TXT, s5_z8_r0, 1 },
         };
         const CharacterString s5_z9_r0[] = { { "v=spf1 ptr:example.Com -all", 27 } };
         const Record s5_z9_records[] = {
            { RecordType::TXT, s5_z9_r0, 1 },
         };
         const CharacterString s5_z10_r0[] = { { "v=spf1 ptr", 10 } };
         const Record s5_z10_records[] = {
            { RecordType::TXT, s5_z10_r0, 1 },
         };
         const CharacterString s5_z11_r0[] = { { "loop4.example.com.", 18 } };
         const Record s5_z11_records[] = {
            { RecordType::PTR, s5_z11_r0, 1 },
         };
         const CharacterString s5_z12_r0[] = { { "CNAME.example.com.", 18 } };
         const Record s5_z12_records[] = {
            { RecordType::CNAME, s5_z12_r0, 1 },
         };
         const CharacterString s5_z13_r0[] = { { "CNAME.example.com.", 18 } };
         const Record s5_z13_records[] = {
            { RecordType::CNAME, s5_z13_r0, 1 },
         };
         const Zone s5_zones[] = {
            { "mail.example.com", s5_z0_records, 2, false },
            { "e1.example.com", s5_z1_records, 1, false },
            { "e2.example.com", s5_z2_records, 1, false },
            { "4.3.2.1.in-addr.arpa", s5_z3_records, 3, false },
            { "1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.E.B.A.B.E.F.A.C.ip6.arpa", s5_z4_records, 1, false },
            { "1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.8.b.D.0.1.0.0.2.ip6.arpa", s5_z5_records, 1, false },
            { "e3.example.com", s5_z6_records, 3, false },
            { "e4.example.com", s5_z7_records, 1, false },
            { "e5.example.com", s5_z8_records, 1, false },
            { "e6.example.com", s5_z9_records, 1, false },
            { "loop.example.com", s5_z10_records, 1, false },
            { "4.2.0.192.in-addr.arpa", s5_z11_records, 1, false },
            { "loop4.example.com", s5_z12_records, 1, false },
            { "cname.example.com", s5_z13_records, 1, false },
         };

         const Result s5_c0_results[] = { Result::PermError };
         const Result s5_c1_results[] = { Result::Pass };
         const Result s5_c2_results[] = { Result::Pass };
         const Result s5_c3_results[] = { Result::Fail };
         const Result s5_c4_results[] = { Result::Pass };
         const Result s5_c5_results[] = { Result::PermError };
         const Result s5_c6_results[] = { Result::Pass };
         const Result s5_c7_results[] = { Result::Neutral };
         const Case s5_cases[] = {
            { "ptr-cidr", "5.5/2", "1.2.3.4", "foo@e1.example.com", "mail.example.com",
              s5_c0_results, 1, 0 },
            { "ptr-match-target", "5.5/5", "1.2.3.4", "foo@e2.example.com", "mail.example.com",
              s5_c1_results, 1, 0 },
            { "ptr-match-implicit", "5.5/5", "1.2.3.4", "foo@e3.example.com", "mail.example.com",
              s5_c2_results, 1, 0 },
            { "ptr-nomatch-invalid", "5.5/5", "1.2.3.4", "foo@e4.example.com", "mail.example.com",
              s5_c3_results, 1, 0 },
            { "ptr-match-ip6", "5.5/5", "CAFE:BABE::1", "foo@e3.example.com", "mail.example.com",
              s5_c4_results, 1, 0 },
            { "ptr-empty-domain", "5.5/2", "1.2.3.4", "foo@e5.example.com", "mail.example.com",
              s5_c5_results, 1, 0 },
            { "ptr-case-change", "5.5/2", "2001:db8::1", "bar@e6.example.com", "mail.example.com",
              s5_c6_results, 1, 0 },
            { "ptr-cname-loop", "5.5/7", "192.0.2.4", "postmaster@loop.example.com", "loop.example.com",
              s5_c7_results, 1, 0 },
         };

         // A mechanism syntax
         const CharacterString s6_z0_r0[] = { { "1.2.3.4", 7 } };
         const Record s6_z0_records[] = {
            { RecordType::A, s6_z0_r0, 1 },
         };
         const CharacterString s6_z1_r0[] = { { "v=spf1 a/0 -all", 15 } };
         const Record s6_z1_records[] = {
            { RecordType::TXT, s6_z1_r0, 1 },
         };
         const CharacterString s6_z2_r0[] = { { "1.1.1.1", 7 } };
         const CharacterString s6_z2_r1[] = { { "1234::2", 7 } };
         const CharacterString s6_z2_r2[] = { { "v=spf1 a/0 -all", 15 } };
         const Record s6_z2_records[] = {
            { RecordType::A, s6_z2_r0, 1 },
            { RecordType::AAAA, s6_z2_r1, 1 },
            { RecordType::TXT, s6_z2_r2, 1 },
         };
         const CharacterString s6_z3_r0[] = { { "1234::1", 7 } };
         const CharacterString s6_z3_r1[] = { { "v=spf1 a//0 -all", 16 } };
         const Record s6_z3_records[] = {
            { RecordType::AAAA, s6_z3_r0, 1 },
            { RecordType::TXT, s6_z3_r1, 1 },
         };
         const CharacterString s6_z4_r0[] = { { "1.1.1.1", 7 } };
         const CharacterString s6_z4_r1[] = { { "v=spf1 a//0 -all", 16 } };
         const Record s6_z4_records[] = {
            { RecordType::A, s6_z4_r0, 1 },
            { RecordType::TXT, s6_z4_r1, 1 },
         };
         const CharacterString s6_z5_r0[] = { { "1234::1", 7 } };
         const CharacterString s6_z5_r1[] = { { "1.1.1.1", 7 } };
         const CharacterString s6_z5_r2[] = { { "v=spf1 a -all", 13 } };
         const Record s6_z5_records[] = {
            { RecordType::AAAA, s6_z5_r0, 1 },
            { RecordType::A, s6_z5_r1, 1 },
            { RecordType::TXT, s6_z5_r2, 1 },
         };
         const CharacterString s6_z6_r0[] = { { "v=spf1 a:foo.example.com" "\x00", 25 } };
         const Record s6_z6_records[] = {
            { RecordType::TXT, s6_z6_r0, 1 },
         };
         const CharacterString s6_z7_r0[] = { { "v=spf1 a:111.222.33.44", 22 } };
         const Record s6_z7_records[] = {
            { RecordType::TXT, s6_z7_r0, 1 },
         };
         const CharacterString s6_z8_r0[] = { { "v=spf1 a:abc.123", 16 } };
         const Record s6_z8_records[] = {
            { RecordType::TXT, s6_z8_r0, 1 },
         };
         const CharacterString s6_z9_r0[] = { { "v=spf1 a:museum", 15 } };
         const Record s6_z9_records[] = {
            { RecordType::TXT, s6_z9_r0, 1 },
         };
         const CharacterString s6_z10_r0[] = { { "v=spf1 a:museum.", 16 } };
         const Record s6_z10_records[] = {
            { RecordType::TXT, s6_z10_r0, 1 },
         };
         const CharacterString s6_z11_r0[] = { { "v=spf1 a//33 -all", 17 } };
         const Record s6_z11_records[] = {
            { RecordType::TXT, s6_z11_r0, 1 },
         };
         const CharacterString s6_z12_r0[] = { { "v=spf1 a/33 -all", 16 } };
         const Record s6_z12_records[] = {
            { RecordType::TXT, s6_z12_r0, 1 },
         };
         const CharacterString s6_z13_r0[] = { { "v=spf1 a//129 -all", 18 } };
         const Record s6_z13_records[] = {
            { RecordType::TXT, s6_z13_r0, 1 },
         };
         const CharacterString s6_z14_r0[] = { { "1.2.3.5", 7 } };
         const CharacterString s6_z14_r1[] = { { "2001:db8:1234::dead:beef", 24 } };
         const CharacterString s6_z14_r2[] = { { "v=spf1 a/24//64 -all", 20 } };
         const Record s6_z14_records[] = {
            { RecordType::A, s6_z14_r0, 1 },
            { RecordType::AAAA, s6_z14_r1, 1 },
            { RecordType::TXT, s6_z14_r2, 1 },
         };
         const CharacterString s6_z15_r0[] = { { "1.2.3.5", 7 } };
         const CharacterString s6_z15_r1[] = { { "2001:db8:1234::dead:beef", 24 } };
         const CharacterString s6_z15_r2[] = { { "v=spf1 a/24/64 -all", 19 } };
         const Record s6_z15_records[] = {
            { RecordType::A, s6_z15_r0, 1 },
            { RecordType::AAAA, s6_z15_r1, 1 },
            { RecordType::TXT, s6_z15_r2, 1 },
         };
         const CharacterString s6_z16_r0[] = { { "1.2.3.5", 7 } };
         const CharacterString s6_z16_r1[] = { { "2001:db8:1234::dead:beef", 24 } };
         const CharacterString s6_z16_r2[] = { { "v=spf1 a/24 -all", 16 } };
         const Record s6_z16_records[] = {
            { RecordType::A, s6_z16_r0, 1 },
            { RecordType::AAAA, s6_z16_r1, 1 },
            { RecordType::TXT, s6_z16_r2, 1 },
         };
         const CharacterString s6_z17_r0[] = { { "1.2.3.5", 7 } };
         const CharacterString s6_z17_r1[] = { { "2001:db8:1234::dead:beef", 24 } };
         const CharacterString s6_z17_r2[] = { { "v=spf1 a//64 -all", 17 } };
         const Record s6_z17_records[] = {
            { RecordType::A, s6_z17_r0, 1 },
            { RecordType::AAAA, s6_z17_r1, 1 },
            { RecordType::TXT, s6_z17_r2, 1 },
         };
         const CharacterString s6_z18_r0[] = { { "v=spf1 a:example.com:8080", 25 } };
         const Record s6_z18_records[] = {
            { RecordType::TXT, s6_z18_r0, 1 },
         };
         const CharacterString s6_z19_r0[] = { { "v=spf1 a:foo.example.com/24", 27 } };
         const Record s6_z19_records[] = {
            { RecordType::TXT, s6_z19_r0, 1 },
         };
         const CharacterString s6_z20_r0[] = { { "1.1.1.1", 7 } };
         const CharacterString s6_z20_r1[] = { { "1.2.3.5", 7 } };
         const Record s6_z20_records[] = {
            { RecordType::A, s6_z20_r0, 1 },
            { RecordType::A, s6_z20_r1, 1 },
         };
         const CharacterString s6_z21_r0[] = { { "v=spf1 a:foo:bar/baz.example.com", 32 } };
         const Record s6_z21_records[] = {
            { RecordType::TXT, s6_z21_r0, 1 },
         };
         const CharacterString s6_z22_r0[] = { { "1.2.3.4", 7 } };
         const Record s6_z22_records[] = {
            { RecordType::A, s6_z22_r0, 1 },
         };
         const CharacterString s6_z23_r0[] = { { "v=spf1 a:example.-com", 21 } };
         const Record s6_z23_records[] = {
            { RecordType::TXT, s6_z23_r0, 1 },
         };
         const CharacterString s6_z24_r0[] = { { "v=spf1 a:", 9 } };
         const Record s6_z24_records[] = {
            { RecordType::TXT, s6_z24_r0, 1 },
         };
         const CharacterString s6_z25_r0[] = { { "v=spf1 a:foo.example.xn--zckzah -all", 36 } };
         const Record s6_z25_records[] = {
            { RecordType::TXT, s6_z25_r0, 1 },
         };
         const CharacterString s6_z26_r0[] = { { "1.2.3.4", 7 } };
         const Record s6_z26_records[] = {
            { RecordType::A, s6_z26_r0, 1 },
         };
         const Zone s6_zones[] = {
            { "mail.example.com", s6_z0_records, 1, false },
            { "e1.example.com", s6_z1_records, 1, false },
            { "e2.example.com", s6_z2_records, 3, false },
            { "e2a.example.com", s6_z3_records, 2, false },
            { "e2b.example.com", s6_z4_records, 2, false },
            { "ipv6.example.com", s6_z5_records, 3, false },
            { "e3.example.com", s6_z6_records, 1, false },
            { "e4.example.com", s6_z7_records, 1, false },
            { "e5.example.com", s6_z8_records, 1, false },
            { "e5a.example.com", s6_z9_records, 1, false },
            { "e5b.example.com", s6_z10_records, 1, false },
            { "e6.example.com", s6_z11_records, 1, false },
            { "e6a.example.com", s6_z12_records, 1, false },
            { "e7.example.com", s6_z13_records, 1, false },
            { "e8.example.com", s6_z14_records, 3, false },
            { "e8e.example.com", s6_z15_records, 3, false },
            { "e8a.example.com", s6_z16_records, 3, false },
            { "e8b.example.com", s6_z17_records, 3, false },
            { "e9.example.com", s6_z18_records, 1, false },
            { "e10.example.com", s6_z19_records, 1, false },
            { "foo.example.com", s6_z20_records, 2, false },
            { "e11.example.com", s6_z21_records, 1, false },
            { "foo:bar/baz.example.com", s6_z22_records, 1, false },
            { "e12.example.com", s6_z23_records, 1, false },
            { "e13.example.com", s6_z24_records, 1, false },
            { "e14.example.com", s6_z25_records, 1, false },
            { "foo.example.xn--zckzah", s6_z26_records, 1, false },
         };

         const Result s6_c0_results[] = { Result::Fail };
         const Result s6_c1_results[] = { Result::PermError };
         const Result s6_c2_results[] = { Result::PermError };
         const Result s6_c3_results[] = { Result::Pass };
         const Result s6_c4_results[] = { Result::PermError };
         const Result s6_c5_results[] = { Result::Pass };
         const Result s6_c6_results[] = { Result::Fail };
         const Result s6_c7_results[] = { Result::Fail };
         const Result s6_c8_results[] = { Result::Pass };
         const Result s6_c9_results[] = { Result::Pass };
         const Result s6_c10_results[] = { Result::PermError };
         const Result s6_c11_results[] = { Result::Fail };
         const Result s6_c12_results[] = { Result::Pass };
         const Result s6_c13_results[] = { Result::Fail };
         const Result s6_c14_results[] = { Result::Fail };
         const Result s6_c15_results[] = { Result::Fail };
         const Result s6_c16_results[] = { Result::Pass };
         const Result s6_c17_results[] = { Result::Pass };
         const Result s6_c18_results[] = { Result::Fail };
         const Result s6_c19_results[] = { Result::PermError };
         const Result s6_c20_results[] = { Result::PermError };
         const Result s6_c21_results[] = { Result::PermError };
         const Result s6_c22_results[] = { Result::Pass };
         const Result s6_c23_results[] = { Result::PermError };
         const Result s6_c24_results[] = { Result::PermError };
         const Result s6_c25_results[] = { Result::PermError };
         const Result s6_c26_results[] = { Result::Pass };
         const Result s6_c27_results[] = { Result::Pass };
         const Result s6_c28_results[] = { Result::PermError };
         const Case s6_cases[] = {
            { "a-cidr6", "5.3/2", "1.2.3.4", "foo@e6.example.com", "mail.example.com",
              s6_c0_results, 1, 0 },
            { "a-bad-cidr4", "5.3/2", "1.2.3.4", "foo@e6a.example.com", "mail.example.com",
              s6_c1_results, 1, 0 },
            { "a-bad-cidr6", "5.3/2", "1.2.3.4", "foo@e7.example.com", "mail.example.com",
              s6_c2_results, 1, 0 },
            { "a-dual-cidr-ip4-match", "5.3/2", "1.2.3.4", "foo@e8.example.com", "mail.example.com",
              s6_c3_results, 1, 0 },
            { "a-dual-cidr-ip4-err", "5.3/2", "1.2.3.4", "foo@e8e.example.com", "mail.example.com",
              s6_c4_results, 1, 0 },
            { "a-dual-cidr-ip6-match", "5.3/2", "2001:db8:1234::cafe:babe", "foo@e8.example.com", "mail.example.com",
              s6_c5_results, 1, 0 },
            { "a-dual-cidr-ip4-default", "5.3/2", "1.2.3.4", "foo@e8b.example.com", "mail.example.com",
              s6_c6_results, 1, 0 },
            { "a-dual-cidr-ip6-default", "5.3/2", "2001:db8:1234::cafe:babe", "foo@e8a.example.com", "mail.example.com",
              s6_c7_results, 1, 0 },
            { "a-multi-ip1", "5.3/3", "1.2.3.4", "foo@e10.example.com", "mail.example.com",
              s6_c8_results, 1, 0 },
            { "a-multi-ip2", "5.3/3", "1.2.3.4", "foo@e10.example.com", "mail.example.com",
              s6_c9_results, 1, 0 },
            { "a-bad-domain", "7.1/2", "1.2.3.4", "foo@e9.example.com", "mail.example.com",
              s6_c10_results, 1, 0 },
            { "a-nxdomain", "5.3/3", "1.2.3.4", "foo@e1.example.com", "mail.example.com",
              s6_c11_results, 1, 0 },
            { "a-cidr4-0", "5.3/3", "1.2.3.4", "foo@e2.example.com", "mail.example.com",
              s6_c12_results, 1, 0 },
            { "a-cidr4-0-ip6", "5.3/3", "1234::1", "foo@e2.example.com", "mail.example.com",
              s6_c13_results, 1, 0 },
            { "a-cidr6-0-ip4", "5.3/3", "1.2.3.4", "foo@e2a.example.com", "mail.example.com",
              s6_c14_results, 1, 0 },
            { "a-cidr6-0-ip4mapped", "5.3/3", "::FFFF:1.2.3.4", "foo@e2a.example.com", "mail.example.com",
              s6_c15_results, 1, 0 },
            { "a-cidr6-0-ip6", "5.3/3", "1234::1", "foo@e2a.example.com", "mail.example.com",
              s6_c16_results, 1, 0 },
            { "a-ip6-dualstack", "5.3/3", "1234::1", "foo@ipv6.example.com", "mail.example.com",
              s6_c17_results, 1, 0 },
            { "a-cidr6-0-nxdomain", "5.3/3", "1234::1", "foo@e2b.example.com", "mail.example.com",
              s6_c18_results, 1, 0 },
            { "a-null", "7.1/2", "1.2.3.5", "foo@e3.example.com", "mail.example.com",
              s6_c19_results, 1, 0 },
            { "a-numeric", "7.1/2", "1.2.3.4", "foo@e4.example.com", "mail.example.com",
              s6_c20_results, 1, 0 },
            { "a-numeric-toplabel", "7.1/2", "1.2.3.4", "foo@e5.example.com", "mail.example.com",
              s6_c21_results, 1, 0 },
            { "a-dash-in-toplabel", "7.1/2", "1.2.3.4", "foo@e14.example.com", "mail.example.com",
              s6_c22_results, 1, 0 },
            { "a-bad-toplabel", "7.1/2", "1.2.3.4", "foo@e12.example.com", "mail.example.com",
              s6_c23_results, 1, 0 },
            { "a-only-toplabel", "7.1/2", "1.2.3.4", "foo@e5a.example.com", "mail.example.com",
              s6_c24_results, 1, 0 },
            { "a-only-toplabel-trailing-dot", "7.1/2", "1.2.3.4", "foo@e5b.example.com", "mail.example.com",
              s6_c25_results, 1, 0 },
            { "a-colon-domain", "7.1/2", "1.2.3.4", "foo@e11.example.com", "mail.example.com",
              s6_c26_results, 1, 0 },
            { "a-colon-domain-ip4mapped", "7.1/2", "::FFFF:1.2.3.4", "foo@e11.example.com", "mail.example.com",
              s6_c27_results, 1, 0 },
            { "a-empty-domain", "5.3/2", "1.2.3.4", "foo@e13.example.com", "mail.example.com",
              s6_c28_results, 1, 0 },
         };

         // Include mechanism semantics and syntax
         const CharacterString s7_z0_r0[] = { { "1.2.3.4", 7 } };
         const Record s7_z0_records[] = {
            { RecordType::A, s7_z0_r0, 1 },
         };
         const CharacterString s7_z1_r0[] = { { "v=spf1 ip4:1.2.3.5 -all", 23 } };
         const Record s7_z1_records[] = {
            { RecordType::TXT, s7_z1_r0, 1 },
         };
         const CharacterString s7_z2_r0[] = { { "v=spf1 ip4:1.2.3.6 ~all", 23 } };
         const Record s7_z2_records[] = {
            { RecordType::TXT, s7_z2_r0, 1 },
         };
         const CharacterString s7_z3_r0[] = { { "v=spf1 ip4:1.2.3.7 \?all", 23 } };
         const Record s7_z3_records[] = {
            { RecordType::TXT, s7_z3_r0, 1 },
         };
         const Record s7_z4_records[] = { { RecordType::TXT, 0, 0 } };
         const CharacterString s7_z5_r0[] = { { "v=spfl am not an SPF record", 27 } };
         const Record s7_z5_records[] = {
            { RecordType::TXT, s7_z5_r0, 1 },
         };
         const CharacterString s7_z6_r0[] = { { "v=spf1 include:ip5.example.com ~all", 35 } };
         const Record s7_z6_records[] = {
            { RecordType::TXT, s7_z6_r0, 1 },
         };
         const CharacterString s7_z7_r0[] = { { "v=spf1 include:ip6.example.com all", 34 } };
         const Record s7_z7_records[] = {
            { RecordType::TXT, s7_z7_r0, 1 },
         };
         const CharacterString s7_z8_r0[] = { { "v=spf1 include:ip7.example.com -all", 35 } };
         const Record s7_z8_records[] = {
            { RecordType::TXT, s7_z8_r0, 1 },
         };
         const CharacterString s7_z9_r0[] = { { "v=spf1 include:ip8.example.com -all", 35 } };
         const Record s7_z9_records[] = {
            { RecordType::TXT, s7_z9_r0, 1 },
         };
         const CharacterString s7_z10_r0[] = { { "v=spf1 include:e6.example.com -all", 34 } };
         const Record s7_z10_records[] = {
            { RecordType::TXT, s7_z10_r0, 1 },
         };
         const CharacterString s7_z11_r0[] = { { "v=spf1 include +all", 19 } };
         const Record s7_z11_records[] = {
            { RecordType::TXT, s7_z11_r0, 1 },
         };
         const CharacterString s7_z12_r0[] = { { "v=spf1 include:erehwon.example.com -all", 39 } };
         const Record s7_z12_records[] = {
            { RecordType::TXT, s7_z12_r0, 1 },
         };
         const CharacterString s7_z13_r0[] = { { "v=spf1 include: -all", 20 } };
         const Record s7_z13_records[] = {
            { RecordType::TXT, s7_z13_r0, 1 },
         };
         const CharacterString s7_z14_r0[] = { { "v=spf1 include:ip5.example.com/24 -all", 38 } };
         const Record s7_z14_records[] = {
            { RecordType::TXT, s7_z14_r0, 1 },
         };
         const Zone s7_zones[] = {
            { "mail.example.com", s7_z0_records, 1, false },
            { "ip5.example.com", s7_z1_records, 1, false },
            { "ip6.example.com", s7_z2_records, 1, false },
            { "ip7.example.com", s7_z3_records, 1, false },
            { "ip8.example.com", s7_z4_records, 0, true },
            { "erehwon.example.com", s7_z5_records, 1, false },
            { "e1.example.com", s7_z6_records, 1, false },
            { "e2.example.com", s7_z7_records, 1, false },
            { "e3.example.com", s7_z8_records, 1, false },
            { "e4.example.com", s7_z9_records, 1, false },
            { "e5.example.com", s7_z10_records, 1, false },
            { "e6.example.com", s7_z11_records, 1, false },
            { "e7.example.com", s7_z12_records, 1, false },
            { "e8.example.com", s7_z13_records, 1, false },
            { "e9.example.com", s7_z14_records, 1, false },
         };

         const Result s7_c0_results[] = { Result::SoftFail };
         const Result s7_c1_results[] = { Result::Pass };
         const Result s7_c2_results[] = { Result::Fail };
         const Result s7_c3_results[] = { Result::TempError };
         const Result s7_c4_results[] = { Result::PermError };
         const Result s7_c5_results[] = { Result::PermError };
         const Result s7_c6_results[] = { Result::PermError };
         const Result s7_c7_results[] = { Result::PermError };
         const Result s7_c8_results[] = { Result::PermError };
         const Case s7_cases[] = {
            { "include-fail", "5.2/9", "1.2.3.4", "foo@e1.example.com", "mail.example.com",
              s7_c0_results, 1, 0 },
            { "include-softfail", "5.2/9", "1.2.3.4", "foo@e2.example.com", "mail.example.com",
              s7_c1_results, 1, 0 },
            { "include-neutral", "5.2/9", "1.2.3.4", "foo@e3.example.com", "mail.example.com",
              s7_c2_results, 1, 0 },
            { "include-temperror", "5.2/9", "1.2.3.4", "foo@e4.example.com", "mail.example.com",
              s7_c3_results, 1, 0 },
            { "include-permerror", "5.2/9", "1.2.3.4", "foo@e5.example.com", "mail.example.com",
              s7_c4_results, 1, 0 },
            { "include-syntax-error", "5.2/1", "1.2.3.4", "foo@e6.example.com", "mail.example.com",
              s7_c5_results, 1, 0 },
            { "include-cidr", "5.2/1", "1.2.3.4", "foo@e9.example.com", "mail.example.com",
              s7_c6_results, 1, 0 },
            { "include-none", "5.2/9", "1.2.3.4", "foo@e7.example.com", "mail.example.com",
              s7_c7_results, 1, 0 },
            { "include-empty-domain", "5.2/1", "1.2.3.4", "foo@e8.example.com", "mail.example.com",
              s7_c8_results, 1, 0 },
         };

         // MX mechanism syntax
         const CharacterString s8_z0_r0[] = { { "1.2.3.4", 7 } };
         const CharacterString s8_z0_r1[] = { { "", 0 } };
         const CharacterString s8_z0_r2[] = { { "v=spf1 mx", 9 } };
         const Record s8_z0_records[] = {
            { RecordType::A, s8_z0_r0, 1 },
            { RecordType::MX, s8_z0_r1, 1 },
            { RecordType::TXT, s8_z0_r2, 1 },
         };
         const CharacterString s8_z1_r0[] = { { "e1.example.com", 14 } };
         const CharacterString s8_z1_r1[] = { { "v=spf1 mx/0 -all", 16 } };
         const Record s8_z1_records[] = {
            { RecordType::MX, s8_z1_r0, 1 },
            { RecordType::TXT, s8_z1_r1, 1 },
         };
         const CharacterString s8_z2_r0[] = { { "1.1.1.1", 7 } };
         const CharacterString s8_z2_r1[] = { { "1234::2", 7 } };
         const CharacterString s8_z2_r2[] = { { "e2.example.com", 14 } };
         const CharacterString s8_z2_r3[] = { { "v=spf1 mx/0 -all", 16 } };
         const Record s8_z2_records[] = {
            { RecordType::A, s8_z2_r0, 1 },
            { RecordType::AAAA, s8_z2_r1, 1 },
            { RecordType::MX, s8_z2_r2, 1 },
            { RecordType::TXT, s8_z2_r3, 1 },
         };
         const CharacterString s8_z3_r0[] = { { "1234::1", 7 } };
         const CharacterString s8_z3_r1[] = { { "e2a.example.com", 15 } };
         const CharacterString s8_z3_r2[] = { { "v=spf1 mx//0 -all", 17 } };
         const Record s8_z3_records[] = {
            { RecordType::AAAA, s8_z3_r0, 1 },
            { RecordType::MX, s8_z3_r1, 1 },
            { RecordType::TXT, s8_z3_r2, 1 },
         };
         const CharacterString s8_z4_r0[] = { { "1.1.1.1", 7 } };
         const CharacterString s8_z4_r1[] = { { "e2b.example.com", 15 } };
         const CharacterString s8_z4_r2[] = { { "v=spf1 mx//0 -all", 17 } };
         const Record s8_z4_records[] = {
            { RecordType::A, s8_z4_r0, 1 },
            { RecordType::MX, s8_z4_r1, 1 },
            { RecordType::TXT, s8_z4_r2, 1 },
         };
         const CharacterString s8_z5_r0[] = { { "v=spf1 mx:foo.example.com" "\x00", 26 } };
         const Record s8_z5_records[] = {
            { RecordType::TXT, s8_z5_r0, 1 },
         };
         const CharacterString s8_z6_r0[] = { { "1.2.3.4", 7 } };
         const CharacterString s8_z6_r1[] = { { "v=spf1 mx", 9 } };
         const Record s8_z6_records[] = {
            { RecordType::A, s8_z6_r0, 1 },
            { RecordType::TXT, s8_z6_r1, 1 },
         };
         const CharacterString s8_z7_r0[] = { { "v=spf1 mx:abc.123", 17 } };
         const Record s8_z7_records[] = {
            { RecordType::TXT, s8_z7_r0, 1 },
         };
         const CharacterString s8_z8_r0[] = { { "v=spf1 mx//33 -all", 18 } };
         const Record s8_z8_records[] = {
            { RecordType::TXT, s8_z8_r0, 1 },
         };
         const CharacterString s8_z9_r0[] = { { "v=spf1 mx/33 -all", 17 } };
         const Record s8_z9_records[] = {
            { RecordType::TXT, s8_z9_r0, 1 },
         };
         const CharacterString s8_z10_r0[] = { { "v=spf1 mx//129 -all", 19 } };
         const Record s8_z10_records[] = {
            { RecordType::TXT, s8_z10_r0, 1 },
         };
         const CharacterString s8_z11_r0[] = { { "v=spf1 mx:example.com:8080", 26 } };
         const Record s8_z11_records[] = {
            { RecordType::TXT, s8_z11_r0, 1 },
         };
         const CharacterString s8_z12_r0[] = { { "v=spf1 mx:foo.example.com/24", 28 } };
         const Record s8_z12_records[] = {
            { RecordType::TXT, s8_z12_r0, 1 },
         };
         const CharacterString s8_z13_r0[] = { { "foo1.example.com", 16 } };
         const Record s8_z13_records[] = {
            { RecordType::MX, s8_z13_r0, 1 },
         };
         const CharacterString s8_z14_r0[] = { { "1.1.1.1", 7 } };
         const CharacterString s8_z14_r1[] = { { "1.2.3.5", 7 } };
         const Record s8_z14_records[] = {
            { RecordType::A, s8_z14_r0, 1 },
            { RecordType::A, s8_z14_r1, 1 },
         };
         const CharacterString s8_z15_r0[] = { { "v=spf1 mx:foo:bar/baz.example.com", 33 } };
         const Record s8_z15_records[] = {
            { RecordType::TXT, s8_z15_r0, 1 },
         };
         const CharacterString s8_z16_r0[] = { { "foo:bar/baz.example.com", 23 } };
         const CharacterString s8_z16_r1[] = { { "1.2.3.4", 7 } };
         const Record s8_z16_records[] = {
            { RecordType::MX, s8_z16_r0, 1 },
            { RecordType::A, s8_z16_r1, 1 },
         };
         const CharacterString s8_z17_r0[] = { { "v=spf1 mx:example.-com", 22 } };
         const Record s8_z17_records[] = {
            { RecordType::TXT, s8_z17_r0, 1 },
         };
         const CharacterString s8_z18_r0[] = { { "v=spf1 mx: -all", 15 } };
         const Record s8_z18_records[] = {
            { RecordType::TXT, s8_z18_r0, 1 },
         };
         const Zone s8_zones[] = {
            { "mail.example.com", s8_z0_records, 3, false },
            { "e1.example.com", s8_z1_records, 2, false },
            { "e2.example.com", s8_z2_records, 4, false },
            { "e2a.example.com", s8_z3_records, 3, false },
            { "e2b.example.com", s8_z4_records, 3, false },
            { "e3.example.com", s8_z5_records, 1, false },
            { "e4.example.com", s8_z6_records, 2, false },
            { "e5.example.com", s8_z7_records, 1, false },
            { "e6.example.com", s8_z8_records, 1, false },
            { "e6a.example.com", s8_z9_records, 1, false },
            { "e7.example.com", s8_z10_records, 1, false },
            { "e9.example.com", s8_z11_records, 1, false },
            { "e10.example.com", s8_z12_records, 1, false },
            { "foo.example.com", s8_z13_records, 1, false },
            { "foo1.example.com", s8_z14_records, 2, false },
            { "e11.example.com", s8_z15_records, 1, false },
            { "foo:bar/baz.example.com", s8_z16_records, 2, false },
            { "e12.example.com", s8_z17_records, 1, false },
            { "e13.example.com", s8_z18_records, 1, false },
         };

         const Result s8_c0_results[] = { Result::Fail };
         const Result s8_c1_results[] = { Result::PermError };
         const Result s8_c2_results[] = { Result::PermError };
         const Result s8_c3_results[] = { Result::Pass };
         const Result s8_c4_results[] = { Result::Pass };
         const Result s8_c5_results[] = { Result::PermError };
         const Result s8_c6_results[] = { Result::Fail };
         const Result s8_c7_results[] = { Result::Pass };
         const Result s8_c8_results[] = { Result::Fail };
         const Result s8_c9_results[] = { Result::Fail };
         const Result s8_c10_results[] = { Result::Fail };
         const Result s8_c11_results[] = { Result::Pass };
         const Result s8_c12_results[] = { Result::Fail };
         const Result s8_c13_results[] = { Result::PermError };
         const Result s8_c14_results[] = { Result::PermError };
         const Result s8_c15_results[] = { Result::Pass };
         const Result s8_c16_results[] = { Result::Pass };
         const Result s8_c17_results[] = { Result::PermError };
         const Result s8_c18_results[] = { Result::Neutral };
         const Result s8_c19_results[] = { Result::Neutral };
         const Result s8_c20_results[] = { Result::PermError };
         const Case s8_cases[] = {
            { "mx-cidr6", "5.4/2", "1.2.3.4", "foo@e6.example.com", "mail.example.com",
              s8_c0_results, 1, 0 },
            { "mx-bad-cidr4", "5.4/2", "1.2.3.4", "foo@e6a.example.com", "mail.example.com",
              s8_c1_results, 1, 0 },
            { "mx-bad-cidr6", "5.4/2", "1.2.3.4", "foo@e7.example.com", "mail.example.com",
              s8_c2_results, 1, 0 },
            { "mx-multi-ip1", "5.4/3", "1.2.3.4", "foo@e10.example.com", "mail.example.com",
              s8_c3_results, 1, 0 },
            { "mx-multi-ip2", "5.4/3", "1.2.3.4", "foo@e10.example.com", "mail.example.com",
              s8_c4_results, 1, 0 },
            { "mx-bad-domain", "7.1/2", "1.2.3.4", "foo@e9.example.com", "mail.example.com",
              s8_c5_results, 1, 0 },
            { "mx-nxdomain", "5.4/3", "1.2.3.4", "foo@e1.example.com", "mail.example.com",
              s8_c6_results, 1, 0 },
            { "mx-cidr4-0", "5.4/3", "1.2.3.4", "foo@e2.example.com", "mail.example.com",
              s8_c7_results, 1, 0 },
            { "mx-cidr4-0-ip6", "5.4/3", "1234::1", "foo@e2.example.com", "mail.example.com",
              s8_c8_results, 1, 0 },
            { "mx-cidr6-0-ip4", "5.4/3", "1.2.3.4", "foo@e2a.example.com", "mail.example.com",
              s8_c9_results, 1, 0 },
            { "mx-cidr6-0-ip4mapped", "5.4/3", "::FFFF:1.2.3.4", "foo@e2a.example.com", "mail.example.com",
              s8_c10_results, 1, 0 },
            { "mx-cidr6-0-ip6", "5.3/3", "1234::1", "foo@e2a.example.com", "mail.example.com",
              s8_c11_results, 1, 0 },
            { "mx-cidr6-0-nxdomain", "5.4/3", "1234::1", "foo@e2b.example.com", "mail.example.com",
              s8_c12_results, 1, 0 },
            { "mx-null", "7.1/2", "1.2.3.5", "foo@e3.example.com", "mail.example.com",
              s8_c13_results, 1, 0 },
            { "mx-numeric-top-label", "7.1/2", "1.2.3.4", "foo@e5.example.com", "mail.example.com",
              s8_c14_results, 1, 0 },
            { "mx-colon-domain", "7.1/2", "1.2.3.4", "foo@e11.example.com", "mail.example.com",
              s8_c15_results, 1, 0 },
            { "mx-colon-domain-ip4mapped", "7.1/2", "::FFFF:1.2.3.4", "foo@e11.example.com", "mail.example.com",
              s8_c16_results, 1, 0 },
            { "mx-bad-toplab", "7.1/2", "1.2.3.4", "foo@e12.example.com", "mail.example.com",
              s8_c17_results, 1, 0 },
            { "mx-empty", "5.4/3", "1.2.3.4", "", "mail.example.com",
              s8_c18_results, 1, 0 },
            { "mx-implicit", "5.4/4", "1.2.3.4", "foo@e4.example.com", "mail.example.com",
              s8_c19_results, 1, 0 },
            { "mx-empty-domain", "5.2/1", "1.2.3.4", "foo@e13.example.com", "mail.example.com",
              s8_c20_results, 1, 0 },
         };

         // EXISTS mechanism syntax
         const CharacterString s9_z0_r0[] = { { "1.2.3.4", 7 } };
         const Record s9_z0_records[] = {
            { RecordType::A, s9_z0_r0, 1 },
         };
         const CharacterString s9_z1_r0[] = { { "CAFE:BABE::4", 12 } };
         const Record s9_z1_records[] = {
            { RecordType::AAAA, s9_z1_r0, 1 },
         };
         const Record s9_z2_records[] = { { RecordType::TXT, 0, 0 } };
         const CharacterString s9_z3_r0[] = { { "v=spf1 exists:", 14 } };
         const Record s9_z3_records[] = {
            { RecordType::TXT, s9_z3_r0, 1 },
         };
         const CharacterString s9_z4_r0[] = { { "v=spf1 exists", 13 } };
         const Record s9_z4_records[] = {
            { RecordType::TXT, s9_z4_r0, 1 },
         };
         const CharacterString s9_z5_r0[] = { { "v=spf1 exists:mail.example.com/24", 33 } };
         const Record s9_z5_records[] = {
            { RecordType::TXT, s9_z5_r0, 1 },
         };
         const CharacterString s9_z6_r0[] = { { "v=spf1 exists:mail.example.com", 30 } };
         const Record s9_z6_records[] = {
            { RecordType::TXT, s9_z6_r0, 1 },
         };
         const CharacterString s9_z7_r0[] = { { "v=spf1 exists:mail6.example.com -all", 36 } };
         const Record s9_z7_records[] = {
            { RecordType::TXT, s9_z7_r0, 1 },
         };
         const CharacterString s9_z8_r0[] = { { "v=spf1 exists:err.example.com -all", 34 } };
         const Record s9_z8_records[] = {
            { RecordType::TXT, s9_z8_r0, 1 },
         };
         const Zone s9_zones[] = {
            { "mail.example.com", s9_z0_records, 1, false },
            { "mail6.example.com", s9_z1_records, 1, false },
            { "err.example.com", s9_z2_records, 0, true },
            { "e1.example.com", s9_z3_records, 1, false },
            { "e2.example.com", s9_z4_records, 1, false },
            { "e3.example.com", s9_z5_records, 1, false },
            { "e4.example.com", s9_z6_records, 1, false },
            { "e5.example.com", s9_z7_records, 1, false },
            { "e6.example.com", s9_z8_records, 1, false },
         };

         const Result s9_c0_results[] = { Result::PermError };
         const Result s9_c1_results[] = { Result::PermError };
         const Result s9_c2_results[] = { Result::PermError };
         const Result s9_c3_results[] = { Result::Pass };
         const Result s9_c4_results[] = { Result::Pass };
         const Result s9_c5_results[] = { Result::Fail };
         const Result s9_c6_results[] = { Result::TempError };
         const Case s9_cases[] = {
            { "exists-empty-domain", "5.7/2", "1.2.3.4", "foo@e1.example.com", "mail.example.com",
              s9_c0_results, 1, 0 },
            { "exists-implicit", "5.7/2", "1.2.3.4", "foo@e2.example.com", "mail.example.com",
              s9_c1_results, 1, 0 },
            { "exists-cidr", "5.7/2", "1.2.3.4", "foo@e3.example.com", "mail.example.com",
              s9_c2_results, 1, 0 },
            { "exists-ip4", "5.7/3", "1.2.3.4", "foo@e4.example.com", "mail.example.com",
              s9_c3_results, 1, 0 },
            { "exists-ip6", "5.7/3", "CAFE:BABE::3", "foo@e4.example.com", "mail.example.com",
              s9_c4_results, 1, 0 },
            { "exists-ip6only", "5.7/3", "CAFE:BABE::3", "foo@e5.example.com", "mail.example.com",
              s9_c5_results, 1, 0 },
            { "exists-dnserr", "5/8", "CAFE:BABE::3", "foo@e6.example.com", "mail.example.com",
              s9_c6_results, 1, 0 },
         };

         // IP4 mechanism syntax
         const CharacterString s10_z0_r0[] = { { "1.2.3.4", 7 } };
         const Record s10_z0_records[] = {
            { RecordType::A, s10_z0_r0, 1 },
         };
         const CharacterString s10_z1_r0[] = { { "v=spf1 ip4:1.1.1.1/0 -all", 25 } };
         const Record s10_z1_records[] = {
            { RecordType::TXT, s10_z1_r0, 1 },
         };
         const CharacterString s10_z2_r0[] = { { "v=spf1 ip4:1.2.3.4/32 -all", 26 } };
         const Record s10_z2_records[] = {
            { RecordType::TXT, s10_z2_r0, 1 },
         };
         const CharacterString s10_z3_r0[] = { { "v=spf1 ip4:1.2.3.4/33 -all", 26 } };
         const Record s10_z3_records[] = {
            { RecordType::TXT, s10_z3_r0, 1 },
         };
         const CharacterString s10_z4_r0[] = { { "v=spf1 ip4:1.2.3.4/032 -all", 27 } };
         const Record s10_z4_records[] = {
            { RecordType::TXT, s10_z4_r0, 1 },
         };
         const CharacterString s10_z5_r0[] = { { "v=spf1 ip4", 10 } };
         const Record s10_z5_records[] = {
            { RecordType::TXT, s10_z5_r0, 1 },
         };
         const CharacterString s10_z6_r0[] = { { "v=spf1 ip4:1.2.3.4//32", 22 } };
         const Record s10_z6_records[] = {
            { RecordType::TXT, s10_z6_r0, 1 },
         };
         const CharacterString s10_z7_r0[] = { { "v=spf1 -ip4:1.2.3.4 ip6:::FFFF:1.2.3.4", 38 } };
         const Record s10_z7_records[] = {
            { RecordType::TXT, s10_z7_r0, 1 },
         };
         const CharacterString s10_z8_r0[] = { { "v=spf1 ip4:1.2.3.4:8080", 23 } };
         const Record s10_z8_records[] = {
            { RecordType::TXT, s10_z8_r0, 1 },
         };
         const CharacterString s10_z9_r0[] = { { "v=spf1 ip4:1.2.3", 16 } };
         const Record s10_z9_records[] = {
            { RecordType::TXT, s10_z9_r0, 1 },
         };
         const Zone s10_zones[] = {
            { "mail.example.com", s10_z0_records, 1, false },
            { "e1.example.com", s10_z1_records, 1, false },
            { "e2.example.com", s10_z2_records, 1, false },
            { "e3.example.com", s10_z3_records, 1, false },
            { "e4.example.com", s10_z4_records, 1, false },
            { "e5.example.com", s10_z5_records, 1, false },
            { "e6.example.com", s10_z6_records, 1, false },
            { "e7.example.com", s10_z7_records, 1, false },
            { "e8.example.com", s10_z8_records, 1, false },
            { "e9.example.com", s10_z9_records, 1, false },
         };

         const Result s10_c0_results[] = { Result::Pass };
         const Result s10_c1_results[] = { Result::Pass };
         const Result s10_c2_results[] = { Result::PermError };
         const Result s10_c3_results[] = { Result::PermError };
         const Result s10_c4_results[] = { Result::PermError };
         const Result s10_c5_results[] = { Result::PermError };
         const Result s10_c6_results[] = { Result::PermError };
         const Result s10_c7_results[] = { Result::PermError };
         const Result s10_c8_results[] = { Result::Fail };
         const Case s10_cases[] = {
            { "cidr4-0", "5.6/2", "1.2.3.4", "foo@e1.example.com", "mail.example.com",
              s10_c0_results, 1, 0 },
            { "cidr4-32", "5.6/2", "1.2.3.4", "foo@e2.example.com", "mail.example.com",
              s10_c1_results, 1, 0 },
            { "cidr4-33", "5.6/2", "1.2.3.4", "foo@e3.example.com", "mail.example.com",
              s10_c2_results, 1, 0 },
            { "cidr4-032", "5.6/2", "1.2.3.4", "foo@e4.example.com", "mail.example.com",
              s10_c3_results, 1, 0 },
            { "bare-ip4", "5.6/2", "1.2.3.4", "foo@e5.example.com", "mail.example.com",
              s10_c4_results, 1, 0 },
            { "bad-ip4-port", "5.6/2", "1.2.3.4", "foo@e8.example.com", "mail.example.com",
              s10_c5_results, 1, 0 },
            { "bad-ip4-short", "5.6/4", "1.2.3.4", "foo@e9.example.com", "mail.example.com",
              s10_c6_results, 1, 0 },
            { "ip4-dual-cidr", "5.6/2", "1.2.3.4", "foo@e6.example.com", "mail.example.com",
              s10_c7_results, 1, 0 },
            { "ip4-mapped-ip6", "5/9/2", "::FFFF:1.2.3.4", "foo@e7.example.com", "mail.example.com",
              s10_c8_results, 1, 0 },
         };

         // IP6 mechanism syntax
         const CharacterString s11_z0_r0[] = { { "1.2.3.4", 7 } };
         const Record s11_z0_records[] = {
            { RecordType::A, s11_z0_r0, 1 },
         };
         const CharacterString s11_z1_r0[] = { { "v=spf1 -all ip6", 15 } };
         const Record s11_z1_records[] = {
            { RecordType::TXT, s11_z1_r0, 1 },
         };
         const CharacterString s11_z2_r0[] = { { "v=spf1 ip6:::1.1.1.1/0", 22 } };
         const Record s11_z2_records[] = {
            { RecordType::TXT, s11_z2_r0, 1 },
         };
         const CharacterString s11_z3_r0[] = { { "v=spf1 ip6:::1.1.1.1/129", 24 } };
         const Record s11_z3_records[] = {
            { RecordType::TXT, s11_z3_r0, 1 },
         };
         const CharacterString s11_z4_r0[] = { { "v=spf1 ip6:::1.1.1.1//33", 24 } };
         const Record s11_z4_records[] = {
            { RecordType::TXT, s11_z4_r0, 1 },
         };
         const CharacterString s11_z5_r0[] = { { "v=spf1 ip6:Cafe:Babe:8000::/33", 30 } };
         const Record s11_z5_records[] = {
            { RecordType::TXT, s11_z5_r0, 1 },
         };
         const CharacterString s11_z6_r0[] = { { "v=spf1 ip6::CAFE::BABE", 22 } };
         const Record s11_z6_records[] = {
            { RecordType::TXT, s11_z6_r0, 1 },
         };
         const Zone s11_zones[] = {
            { "mail.example.com", s11_z0_records, 1, false },
            { "e1.example.com", s11_z1_records, 1, false },
            { "e2.example.com", s11_z2_records, 1, false },
            { "e3.example.com", s11_z3_records, 1, false },
            { "e4.example.com", s11_z4_records, 1, false },
            { "e5.example.com", s11_z5_records, 1, false },
            { "e6.example.com", s11_z6_records, 1, false },
         };

         const Result s11_c0_results[] = { Result::PermError };
         const Result s11_c1_results[] = { Result::Neutral };
         const Result s11_c2_results[] = { Result::Neutral };
         const Result s11_c3_results[] = { Result::Pass };
         const Result s11_c4_results[] = { Result::PermError };
         const Result s11_c5_results[] = { Result::PermError };
         const Result s11_c6_results[] = { Result::Pass };
         const Result s11_c7_results[] = { Result::Neutral };
         const Result s11_c8_results[] = { Result::PermError };
         const Case s11_cases[] = {
            { "bare-ip6", "5.6/2", "1.2.3.4", "foo@e1.example.com", "mail.example.com",
              s11_c0_results, 1, 0 },
            { "cidr6-0-ip4", "5/9/2", "1.2.3.4", "foo@e2.example.com", "mail.example.com",
              s11_c1_results, 1, 0 },
            { "cidr6-ip4", "5/9/2", "::FFFF:1.2.3.4", "foo@e2.example.com", "mail.example.com",
              s11_c2_results, 1, 0 },
            { "cidr6-0", "5/8", "DEAF:BABE::CAB:FEE", "foo@e2.example.com", "mail.example.com",
              s11_c3_results, 1, 0 },
            { "cidr6-129", "5.6/2", "1.2.3.4", "foo@e3.example.com", "mail.example.com",
              s11_c4_results, 1, 0 },
            { "cidr6-bad", "5.6/2", "1.2.3.4", "foo@e4.example.com", "mail.example.com",
              s11_c5_results, 1, 0 },
            { "cidr6-33", "5.6/2", "CAFE:BABE:8000::", "foo@e5.example.com", "mail.example.com",
              s11_c6_results, 1, 0 },
            { "cidr6-33-ip4", "5.6/2", "1.2.3.4", "foo@e5.example.com", "mail.example.com",
              s11_c7_results, 1, 0 },
            { "ip6-bad1", "5.6/2", "1.2.3.4", "foo@e6.example.com", "mail.example.com",
              s11_c8_results, 1, 0 },
         };

         // Semantics of exp and other modifiers
         const CharacterString s12_z0_r0[] = { { "1.2.3.4", 7 } };
         const Record s12_z0_records[] = {
            { RecordType::A, s12_z0_r0, 1 },
         };
         const CharacterString s12_z1_r0[] = { { "v=spf1 exp=exp1.example.com redirect=e2.example.com", 51 } };
         const Record s12_z1_records[] = {
            { RecordType::TXT, s12_z1_r0, 1 },
         };
         const CharacterString s12_z2_r0[] = { { "v=spf1 -all", 11 } };
         const Record s12_z2_records[] = {
            { RecordType::TXT, s12_z2_r0, 1 },
         };
         const CharacterString s12_z3_r0[] = { { "v=spf1 exp=exp1.example.com redirect=e4.example.com", 51 } };
         const Record s12_z3_records[] = {
            { RecordType::TXT, s12_z3_r0, 1 },
         };
         const CharacterString s12_z4_r0[] = { { "v=spf1 -all exp=exp2.example.com", 32 } };
         const Record s12_z4_records[] = {
            { RecordType::TXT, s12_z4_r0, 1 },
         };
         const CharacterString s12_z5_r0[] = { { "No-see-um", 9 } };
         const Record s12_z5_records[] = {
            { RecordType::TXT, s12_z5_r0, 1 },
         };
         const CharacterString s12_z6_r0[] = { { "See me.", 7 } };
         const Record s12_z6_records[] = {
            { RecordType::TXT, s12_z6_r0, 1 },
         };
         const CharacterString s12_z7_r0[] = { { "Correct!", 8 } };
         const Record s12_z7_records[] = {
            { RecordType::TXT, s12_z7_r0, 1 },
         };
         const CharacterString s12_z8_r0[] = { { "%{l} in implementation", 22 } };
         const Record s12_z8_records[] = {
            { RecordType::TXT, s12_z8_r0, 1 },
         };
         const CharacterString s12_z9_r0[] = { { "v=spf1 1up=foo", 14 } };
         const Record s12_z9_records[] = {
            { RecordType::TXT, s12_z9_r0, 1 },
         };
         const CharacterString s12_z10_r0[] = { { "v=spf1 =all", 11 } };
         const Record s12_z10_records[] = {
            { RecordType::TXT, s12_z10_r0, 1 },
         };
         const CharacterString s12_z11_r0[] = { { "v=spf1 include:e3.example.com -all exp=exp3.example.com", 55 } };
         const Record s12_z11_records[] = {
            { RecordType::TXT, s12_z11_r0, 1 },
         };
         const CharacterString s12_z12_r0[] = { { "v=spf1 -all exp=exp4.example.com", 32 } };
         const Record s12_z12_records[] = {
            { RecordType::TXT, s12_z12_r0, 1 },
         };
         const CharacterString s12_z13_r0[] = { { "v=spf1 -all foo=%abc", 20 } };
         const Record s12_z13_records[] = {
            { RecordType::TXT, s12_z13_r0, 1 },
         };
         const CharacterString s12_z14_r0[] = { { "v=spf1 redirect=erehwon.example.com", 35 } };
         const Record s12_z14_records[] = {
            { RecordType::TXT, s12_z14_r0, 1 },
         };
         const CharacterString s12_z15_r0[] = { { "v=spf1 -all exp=e11msg.example.com", 34 } };
         const Record s12_z15_records[] = {
            { RecordType::TXT, s12_z15_r0, 1 },
         };
         const CharacterString s12_z16_r0[] = { { "Answer a fool according to his folly.", 37 } };
         const CharacterString s12_z16_r1[] = { { "Do not answer a fool according to his folly.", 44 } };
         const Record s12_z16_records[] = {
            { RecordType::TXT, s12_z16_r0, 1 },
            { RecordType::TXT, s12_z16_r1, 1 },
         };
         const CharacterString s12_z17_r0[] = { { "v=spf1 exp= -all", 16 } };
         const Record s12_z17_records[] = {
            { RecordType::TXT, s12_z17_r0, 1 },
         };
         const CharacterString s12_z18_r0[] = { { "v=spf1 exp=e13msg.example.com -all", 34 } };
         const Record s12_z18_records[] = {
            { RecordType::TXT, s12_z18_r0, 1 },
         };
         const CharacterString s12_z19_r0[] = { { "The %{x}-files.", 15 } };
         const Record s12_z19_records[] = {
            { RecordType::TXT, s12_z19_r0, 1 },
         };
         const CharacterString s12_z20_r0[] = { { "v=spf1 exp=e13msg.example.com -all exp=e11msg.example.com", 57 } };
         const Record s12_z20_records[] = {
            { RecordType::TXT, s12_z20_r0, 1 },
         };
         const CharacterString s12_z21_r0[] = { { "v=spf1 redirect=e12.example.com -all redirect=e12.example.com", 61 } };
         const Record s12_z21_records[] = {
            { RecordType::TXT, s12_z21_r0, 1 },
         };
         const CharacterString s12_z22_r0[] = { { "v=spf1 exp=-all", 15 } };
         const Record s12_z22_records[] = {
            { RecordType::TXT, s12_z22_r0, 1 },
         };
         const CharacterString s12_z23_r0[] = { { "v=spf1 redirect=-all \?all", 25 } };
         const Record s12_z23_records[] = {
            { RecordType::TXT, s12_z23_r0, 1 },
         };
         const CharacterString s12_z24_r0[] = { { "v=spf1 \?all redirect=", 21 } };
         const Record s12_z24_records[] = {
            { RecordType::TXT, s12_z24_r0, 1 },
         };
         const CharacterString s12_z25_r0[] = { { "v=spf1 default=pass", 19 } };
         const Record s12_z25_records[] = {
            { RecordType::TXT, s12_z25_r0, 1 },
         };
         const CharacterString s12_z26_r0[] = { { "v=spf1 default=+", 16 } };
         const Record s12_z26_records[] = {
            { RecordType::TXT, s12_z26_r0, 1 },
         };
         const CharacterString s12_z27_r0[] = { { "v=spf1 exp=e21msg.example.com -all", 34 } };
         const Record s12_z27_records[] = {
            { RecordType::TXT, s12_z27_r0, 1 },
         };
         const Record s12_z28_records[] = { { RecordType::TXT, 0, 0 } };
         const CharacterString s12_z29_r0[] = { { "v=spf1 exp=mail.example.com -all", 32 } };
         const Record s12_z29_records[] = {
            { RecordType::TXT, s12_z29_r0, 1 },
         };
         const CharacterString s12_z30_r0[] = { { "v=spf1 exp=badexp.example.com -all", 34 } };
         const Record s12_z30_records[] = {
            { RecordType::TXT, s12_z30_r0, 1 },
         };
         const CharacterString s12_z31_r0[] = { { "\xc3" "\xaf" "\xc2" "\xbb" "\xc2" "\xbf" "Explanation", 17 } };
         const Record s12_z31_records[] = {
            { RecordType::TXT, s12_z31_r0, 1 },
         };
         const CharacterString s12_z32_r0[] = { { "v=spf1 exp=twoexp.example.com -all", 34 } };
         const Record s12_z32_records[] = {
            { RecordType::TXT, s12_z32_r0, 1 },
         };
         const CharacterString s12_z33_r0[] = { { "one", 3 } };
         const CharacterString s12_z33_r1[] = { { "two", 3 } };
         const Record s12_z33_records[] = {
            { RecordType::TXT, s12_z33_r0, 1 },
            { RecordType::TXT, s12_z33_r1, 1 },
         };
         const CharacterString s12_z34_r0[] = { { "v=spf1 a:erehwon.example.com a:foobar.com exp=nxdomain.com -all", 63 } };
         const Record s12_z34_records[] = {
            { RecordType::TXT, s12_z34_r0, 1 },
         };
         const CharacterString s12_z35_r0[] = { { "192.0.2.1", 9 } };
         const CharacterString s12_z35_r1[] = { { "v=spf1 redirect=testimplicit.example.com", 40 } };
         const Record s12_z35_records[] = {
            { RecordType::A, s12_z35_r0, 1 },
            { RecordType::TXT, s12_z35_r1, 1 },
         };
         const CharacterString s12_z36_r0[] = { { "192.0.2.2", 9 } };
         const CharacterString s12_z36_r1[] = { { "v=spf1 a -all", 13 } };
         const Record s12_z36_records[] = {
            { RecordType::A, s12_z36_r0, 1 },
            { RecordType::TXT, s12_z36_r1, 1 },
         };
         const Zone s12_zones[] = {
            { "mail.example.com", s12_z0_records, 1, false },
            { "e1.example.com", s12_z1_records, 1, false },
            { "e2.example.com", s12_z2_records, 1, false },
            { "e3.example.com", s12_z3_records, 1, false },
            { "e4.example.com", s12_z4_records, 1, false },
            { "exp1.example.com", s12_z5_records, 1, false },
            { "exp2.example.com", s12_z6_records, 1, false },
            { "exp3.example.com", s12_z7_records, 1, false },
            { "exp4.example.com", s12_z8_records, 1, false },
            { "e5.example.com", s12_z9_records, 1, false },
            { "e6.example.com", s12_z10_records, 1, false },
            { "e7.example.com", s12_z11_records, 1, false },
            { "e8.example.com", s12_z12_records, 1, false },
            { "e9.example.com", s12_z13_records, 1, false },
            { "e10.example.com", s12_z14_records, 1, false },
            { "e11.example.com", s12_z15_records, 1, false },
            { "e11msg.example.com", s12_z16_records, 2, false },
            { "e12.example.com", s12_z17_records, 1, false },
            { "e13.example.com", s12_z18_records, 1, false },
            { "e13msg.example.com", s12_z19_records, 1, false },
            { "e14.example.com", s12_z20_records, 1, false },
            { "e15.example.com", s12_z21_records, 1, false },
            { "e16.example.com", s12_z22_records, 1, false },
            { "e17.example.com", s12_z23_records, 1, false },
            { "e18.example.com", s12_z24_records, 1, false },
            { "e19.example.com", s12_z25_records, 1, false },
            { "e20.example.com", s12_z26_records, 1, false },
            { "e21.example.com", s12_z27_records, 1, false },
            { "e21msg.example.com", s12_z28_records, 0, true },
            { "e22.example.com", s12_z29_records, 1, false },
            { "nonascii.example.com", s12_z30_records, 1, false },
            { "badexp.example.com", s12_z31_records, 1, false },
            { "tworecs.example.com", s12_z32_records, 1, false },
            { "twoexp.example.com", s12_z33_records, 2, false },
            { "e23.example.com", s12_z34_records, 1, false },
            { "e24.example.com", s12_z35_records, 2, false },
            { "testimplicit.example.com", s12_z36_records, 2, false },
         };

         const Result s12_c0_results[] = { Result::PermError };
         const Result s12_c1_results[] = { Result::Fail };
         const Result s12_c2_results[] = { Result::PermError };
         const Result s12_c3_results[] = { Result::Fail };
         const Result s12_c4_results[] = { Result::Fail };
         const Result s12_c5_results[] = { Result::PermError };
         const Result s12_c6_results[] = { Result::PermError };
         const Result s12_c7_results[] = { Result::Fail };
         const Result s12_c8_results[] = { Result::Fail };
         const Result s12_c9_results[] = { Result::Fail };
         const Result s12_c10_results[] = { Result::Fail };
         const Result s12_c11_results[] = { Result::PermError };
         const Result s12_c12_results[] = { Result::Fail };
         const Result s12_c13_results[] = { Result::PermError };
         const Result s12_c14_results[] = { Result::PermError };
         const Result s12_c15_results[] = { Result::PermError };
         const Result s12_c16_results[] = { Result::PermError };
         const Result s12_c17_results[] = { Result::PermError };
         const Result s12_c18_results[] = { Result::Neutral };
         const Result s12_c19_results[] = { Result::Neutral };
         const Result s12_c20_results[] = { Result::Fail };
         const Result s12_c21_results[] = { Result::Fail };
         const Result s12_c22_results[] = { Result::Fail };
         const Result s12_c23_results[] = { Result::Pass };
         const Case s12_cases[] = {
            { "redirect-none", "6.1/4", "1.2.3.4", "foo@e10.example.com", "mail.example.com",
              s12_c0_results, 1, 0 },
            { "redirect-cancels-exp", "6.2/13", "1.2.3.4", "foo@e1.example.com", "mail.example.com",
              s12_c1_results, 1, "" },
            { "redirect-syntax-error", "6.1/2", "1.2.3.4", "foo@e17.example.com", "mail.example.com",
              s12_c2_results, 1, 0 },
            { "include-ignores-exp", "6.2/13", "1.2.3.4", "foo@e7.example.com", "mail.example.com",
              s12_c3_results, 1, "Correct!" },
            { "redirect-cancels-prior-exp", "6.2/13", "1.2.3.4", "foo@e3.example.com", "mail.example.com",
              s12_c4_results, 1, "See me." },
            { "invalid-modifier", "A/3", "1.2.3.4", "foo@e5.example.com", "mail.example.com",
              s12_c5_results, 1, 0 },
            { "empty-modifier-name", "A/3", "1.2.3.4", "foo@e6.example.com", "mail.example.com",
              s12_c6_results, 1, 0 },
            { "dorky-sentinel", "7.1/6", "1.2.3.4", "Macro Error@e8.example.com", "mail.example.com",
              s12_c7_results, 1, "Macro Error in implementation" },
            { "exp-multiple-txt", "6.2/4", "1.2.3.4", "foo@e11.example.com", "mail.example.com",
              s12_c8_results, 1, "" },
            { "exp-no-txt", "6.2/4", "1.2.3.4", "foo@e22.example.com", "mail.example.com",
              s12_c9_results, 1, "" },
            { "exp-dns-error", "6.2/4", "1.2.3.4", "foo@e21.example.com", "mail.example.com",
              s12_c10_results, 1, "" },
            { "exp-empty-domain", "6.2/4", "1.2.3.4", "foo@e12.example.com", "mail.example.com",
              s12_c11_results, 1, 0 },
            { "explanation-syntax-error", "6.2/4", "1.2.3.4", "foo@e13.example.com", "mail.example.com",
              s12_c12_results, 1, "" },
            { "exp-syntax-error", "6.2/1", "1.2.3.4", "foo@e16.example.com", "mail.example.com",
              s12_c13_results, 1, 0 },
            { "exp-twice", "6/2", "1.2.3.4", "foo@e14.example.com", "mail.example.com",
              s12_c14_results, 1, 0 },
            { "redirect-empty-domain", "6.2/4", "1.2.3.4", "foo@e18.example.com", "mail.example.com",
              s12_c15_results, 1, 0 },
            { "redirect-twice", "6/2", "1.2.3.4", "foo@e15.example.com", "mail.example.com",
              s12_c16_results, 1, 0 },
            { "unknown-modifier-syntax", "A/3", "1.2.3.4", "foo@e9.example.com", "mail.example.com",
              s12_c17_results, 1, 0 },
            { "default-modifier-obsolete", "6/3", "1.2.3.4", "foo@e19.example.com", "mail.example.com",
              s12_c18_results, 1, 0 },
            { "default-modifier-obsolete2", "6/3", "1.2.3.4", "foo@e20.example.com", "mail.example.com",
              s12_c19_results, 1, 0 },
            { "non-ascii-exp", "6.2/5", "1.2.3.4", "foobar@nonascii.example.com", "hosed",
              s12_c20_results, 1, "" },
            { "two-exp-records", "6.2/4", "1.2.3.4", "foobar@tworecs.example.com", "hosed",
              s12_c21_results, 1, "" },
            { "exp-void", "4.6.4/1, 6/2", "1.2.3.4", "foo@e23.example.com", "mail.example.com",
              s12_c22_results, 1, 0 },
            { "redirect-implicit", "6.1/4", "192.0.2.2", "bar@e24.example.com", "e24.example.com",
              s12_c23_results, 1, 0 },
         };

         // Macro expansion rules
         const CharacterString s13_z0_r0[] = { { "v=spf1 redirect=a.spf.example.com", 33 } };
         const Record s13_z0_records[] = {
            { RecordType::TXT, s13_z0_r0, 1 },
         };
         const CharacterString s13_z1_r0[] = { { "v=spf1 include:o.spf.example.com. ~all", 38 } };
         const Record s13_z1_records[] = {
            { RecordType::TXT, s13_z1_r0, 1 },
         };
         const CharacterString s13_z2_r0[] = { { "v=spf1 ip4:192.168.218.40", 25 } };
         const Record s13_z2_records[] = {
            { RecordType::TXT, s13_z2_r0, 1 },
         };
         const CharacterString s13_z3_r0[] = { { "192.168.218.40", 14 } };
         const Record s13_z3_records[] = {
            { RecordType::A, s13_z3_r0, 1 },
         };
         const CharacterString s13_z4_r0[] = { { "192.168.90.76", 13 } };
         const CharacterString s13_z4_r1[] = { { "v=spf1 redirect=%{d}.d.spf.example.com.", 39 } };
         const Record s13_z4_records[] = {
            { RecordType::A, s13_z4_r0, 1 },
            { RecordType::TXT, s13_z4_r1, 1 },
         };
         const CharacterString s13_z5_r0[] = { { "v=spf1 exp=msg.example.com. -all", 32 } };
         const Record s13_z5_records[] = {
            { RecordType::TXT, s13_z5_r0, 1 },
         };
         const CharacterString s13_z6_r0[] = { { "This is a test.", 15 } };
         const Record s13_z6_records[] = {
            { RecordType::TXT, s13_z6_r0, 1 },
         };
         const CharacterString s13_z7_r0[] = { { "v=spf1 -exists:%(ir).sbl.example.com \?all", 41 } };
         const Record s13_z7_records[] = {
            { RecordType::TXT, s13_z7_r0, 1 },
         };
         const CharacterString s13_z8_r0[] = { { "v=spf1 exists:foo%(ir).sbl.example.com \?all", 43 } };
         const Record s13_z8_records[] = {
            { RecordType::TXT, s13_z8_r0, 1 },
         };
         const CharacterString s13_z9_r0[] = { { "v=spf1 exists:foo%.sbl.example.com \?all", 39 } };
         const Record s13_z9_records[] = {
            { RecordType::TXT, s13_z9_r0, 1 },
         };
         const CharacterString s13_z10_r0[] = { { "v=spf1 a:macro%%percent%_%_space%-url-space.example.com -all", 60 } };
         const Record s13_z10_records[] = {
            { RecordType::TXT, s13_z10_r0, 1 },
         };
         const CharacterString s13_z11_r0[] = { { "1.2.3.4", 7 } };
         const Record s13_z11_records[] = {
            { RecordType::A, s13_z11_r0, 1 },
         };
         const CharacterString s13_z12_r0[] = { { "v=spf1 -all exp=%{r}.example.com", 32 } };
         const Record s13_z12_records[] = {
            { RecordType::TXT, s13_z12_r0, 1 },
         };
         const CharacterString s13_z13_r0[] = { { "v=spf1 -all exp=%{ir}.example.com", 33 } };
         const Record s13_z13_records[] = {
            { RecordType::TXT, s13_z13_r0, 1 },
         };
         const CharacterString s13_z14_r0[] = { { "Connections from %{c} not authorized.", 37 } };
         const Record s13_z14_records[] = {
            { RecordType::TXT, s13_z14_r0, 1 },
         };
         const CharacterString s13_z15_r0[] = { { "v=spf1 -all exp=foobar.%{o}.%{o}.%{o}.%{o}.%{o}.%{o}.%{o}.%{o}.example.com", 74 } };
         const Record s13_z15_records[] = {
            { RecordType::TXT, s13_z15_r0, 1 },
         };
         const CharacterString s13_z16_r0[] = { { "Congratulations!  That was tricky.", 34 } };
         const Record s13_z16_records[] = {
            { RecordType::TXT, s13_z16_r0, 1 },
         };
         const CharacterString s13_z17_r0[] = { { "v=spf1 -all exp=e4msg.example.com", 33 } };
         const Record s13_z17_records[] = {
            { RecordType::TXT, s13_z17_r0, 1 },
         };
         const CharacterString s13_z18_r0[] = { { "%{c} is queried as %{ir}.%{v}.arpa", 34 } };
         const Record s13_z18_records[] = {
            { RecordType::TXT, s13_z18_r0, 1 },
         };
         const CharacterString s13_z19_r0[] = { { "v=spf1 a:%{a}.example.com -all", 30 } };
         const Record s13_z19_records[] = {
            { RecordType::TXT, s13_z19_r0, 1 },
         };
         const CharacterString s13_z20_r0[] = { { "v=spf1 -all exp=e6msg.example.com", 33 } };
         const Record s13_z20_records[] = {
            { RecordType::TXT, s13_z20_r0, 1 },
         };
         const CharacterString s13_z21_r0[] = { { "connect from %{p}", 17 } };
         const Record s13_z21_records[] = {
            { RecordType::TXT, s13_z21_r0, 1 },
         };
         const CharacterString s13_z22_r0[] = { { "192.168.218.41", 14 } };
         const CharacterString s13_z22_r1[] = { { "192.168.218.42", 14 } };
         const CharacterString s13_z22_r2[] = { { "CAFE:BABE::2", 12 } };
         const CharacterString s13_z22_r3[] = { { "CAFE:BABE::3", 12 } };
         const Record s13_z22_records[] = {
            { RecordType::A, s13_z22_r0, 1 },
            { RecordType::A, s13_z22_r1, 1 },
            { RecordType::AAAA, s13_z22_r2, 1 },
            { RecordType::AAAA, s13_z22_r3, 1 },
         };
         const CharacterString s13_z23_r0[] = { { "mx.example.com", 14 } };
         const Record s13_z23_records[] = {
            { RecordType::PTR, s13_z23_r0, 1 },
         };
         const CharacterString s13_z24_r0[] = { { "mx.example.com", 14 } };
         const Record s13_z24_records[] = {
            { RecordType::PTR, s13_z24_r0, 1 },
         };
         const CharacterString s13_z25_r0[] = { { "mx.example.com", 14 } };
         const CharacterString s13_z25_r1[] = { { "mx.e7.example.com", 17 } };
         const Record s13_z25_records[] = {
            { RecordType::PTR, s13_z25_r0, 1 },
            { RecordType::PTR, s13_z25_r1, 1 },
         };
         const CharacterString s13_z26_r0[] = { { "mx.example.com", 14 } };
         const Record s13_z26_records[] = {
            { RecordType::PTR, s13_z26_r0, 1 },
         };
         const CharacterString s13_z27_r0[] = { { "mx.example.com", 14 } };
         const Record s13_z27_records[] = {
            { RecordType::PTR, s13_z27_r0, 1 },
         };
         const CharacterString s13_z28_r0[] = { { "192.168.218.42", 14 } };
         const Record s13_z28_records[] = {
            { RecordType::A, s13_z28_r0, 1 },
         };
         const CharacterString s13_z29_r0[] = { { "127.0.0.2", 9 } };
         const Record s13_z29_records[] = {
            { RecordType::A, s13_z29_r0, 1 },
         };
         const CharacterString s13_z30_r0[] = { { "127.0.0.2", 9 } };
         const Record s13_z30_records[] = {
            { RecordType::A, s13_z30_r0, 1 },
         };
         const CharacterString s13_z31_r0[] = { { "v=spf1 exists:%{p}.should.example.com ~exists:%{p}.ok.example.com", 65 } };
         const Record s13_z31_records[] = {
            { RecordType::TXT, s13_z31_r0, 1 },
         };
         const CharacterString s13_z32_r0[] = { { "v=spf1 -all exp=msg8.%{D2}", 26 } };
         const Record s13_z32_records[] = {
            { RecordType::TXT, s13_z32_r0, 1 },
         };
         const CharacterString s13_z33_r0[] = { { "http://example.com/why.html\?l=%{L}", 34 } };
         const Record s13_z33_records[] = {
            { RecordType::TXT, s13_z33_r0, 1 },
         };
         const CharacterString s13_z34_r0[] = { { "v=spf1 a:%{H} -all", 18 } };
         const Record s13_z34_records[] = {
            { RecordType::TXT, s13_z34_r0, 1 },
         };
         const CharacterString s13_z35_r0[] = { { "v=spf1 -include:_spfh.%{d2} ip4:1.2.3.0/24 -all", 47 } };
         const Record s13_z35_records[] = {
            { RecordType::TXT, s13_z35_r0, 1 },
         };
         const CharacterString s13_z36_r0[] = { { "v=spf1 -a:%{h} +all", 19 } };
         const Record s13_z36_records[] = {
            { RecordType::TXT, s13_z36_r0, 1 },
         };
         const CharacterString s13_z37_r0[] = { { "v=spf1 exists:%{i}.%{l2r-}.user.%{d2}", 37 } };
         const Record s13_z37_records[] = {
            { RecordType::TXT, s13_z37_r0, 1 },
         };
         const CharacterString s13_z38_r0[] = { { "127.0.0.2", 9 } };
         const Record s13_z38_records[] = {
            { RecordType::A, s13_z38_r0, 1 },
         };
         const CharacterString s13_z39_r0[] = { { "v=spf1 exists:%{l2r+-}.user.%{d2}", 33 } };
         const Record s13_z39_records[] = {
            { RecordType::TXT, s13_z39_r0, 1 },
         };
         const CharacterString s13_z40_r0[] = { { "127.0.0.2", 9 } };
         const Record s13_z40_records[] = {
            { RecordType::A, s13_z40_r0, 1 },
         };
         const Zone s13_zones[] = {
            { "example.com.d.spf.example.com", s13_z0_records, 1, false },
            { "a.spf.example.com", s13_z1_records, 1, false },
            { "o.spf.example.com", s13_z2_records, 1, false },
            { "msgbas2x.cos.example.com", s13_z3_records, 1, false },
            { "example.com", s13_z4_records, 2, false },
            { "exp.example.com", s13_z5_records, 1, false },
            { "msg.example.com", s13_z6_records, 1, false },
            { "e1.example.com", s13_z7_records, 1, false },
            { "e1e.example.com", s13_z8_records, 1, false },
            { "e1t.example.com", s13_z9_records, 1, false },
            { "e1a.example.com", s13_z10_records, 1, false },
            { "macro%percent  space%20url-space.example.com", s13_z11_records, 1, false },
            { "e2.example.com", s13_z12_records, 1, false },
            { "e3.example.com", s13_z13_records, 1, false },
            { "40.218.168.192.example.com", s13_z14_records, 1, false },
            { "somewhat.long.exp.example.com", s13_z15_records, 1, false },
            { "somewhat.long.exp.example.com.somewhat.long.exp.example.com.somewhat.long.exp.example.com.somewhat.long.exp.example.com.somewhat.long.exp.example.com.somewhat.long.exp.example.com.somewhat.long.exp.example.com.somewhat.long.exp.example.com.example.com", s13_z16_records, 1, false },
            { "e4.example.com", s13_z17_records, 1, false },
            { "e4msg.example.com", s13_z18_records, 1, false },
            { "e5.example.com", s13_z19_records, 1, false },
            { "e6.example.com", s13_z20_records, 1, false },
            { "e6msg.example.com", s13_z21_records, 1, false },
            { "mx.example.com", s13_z22_records, 4, false },
            { "40.218.168.192.in-addr.arpa", s13_z23_records, 1, false },
            { "41.218.168.192.in-addr.arpa", s13_z24_records, 1, false },
            { "42.218.168.192.in-addr.arpa", s13_z25_records, 2, false },
            { "1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.E.B.A.B.E.F.A.C.ip6.arpa", s13_z26_records, 1, false },
            { "3.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.E.B.A.B.E.F.A.C.ip6.arpa", s13_z27_records, 1, false },
            { "mx.e7.example.com", s13_z28_records, 1, false },
            { "mx.e7.example.com.should.example.com", s13_z29_records, 1, false },
            { "mx.example.com.ok.example.com", s13_z30_records, 1, false },
            { "e7.example.com", s13_z31_records, 1, false },
            { "e8.example.com", s13_z32_records, 1, false },
            { "msg8.example.com", s13_z33_records, 1, false },
            { "e9.example.com", s13_z34_records, 1, false },
            { "e10.example.com", s13_z35_records, 1, false },
            { "_spfh.example.com", s13_z36_records, 1, false },
            { "e11.example.com", s13_z37_records, 1, false },
            { "1.2.3.4.gladstone.philip.user.example.com", s13_z38_records, 1, false },
            { "e12.example.com", s13_z39_records, 1, false },
            { "bar.foo.user.example.com", s13_z40_records, 1, false },
         };

         const Result s13_c0_results[] = { Result::Pass };
         const Result s13_c1_results[] = { Result::Fail };
         const Result s13_c2_results[] = { Result::PermError };
         const Result s13_c3_results[] = { Result::PermError };
         const Result s13_c4_results[] = { Result::PermError };
         const Result s13_c5_results[] = { Result::PermError };
         const Result s13_c6_results[] = { Result::Pass };
         const Result s13_c7_results[] = { Result::Fail };
         const Result s13_c8_results[] = { Result::Fail };
         const Result s13_c9_results[] = { Result::Fail };
         const Result s13_c10_results[] = { Result::Fail };
         const Result s13_c11_results[] = { Result::PermError };
         const Result s13_c12_results[] = { Result::Fail };
         const Result s13_c13_results[] = { Result::Fail };
         const Result s13_c14_results[] = { Result::Fail };
         const Result s13_c15_results[] = { Result::Fail };
         const Result s13_c16_results[] = { Result::Pass, Result::SoftFail };
         const Result s13_c17_results[] = { Result::Fail };
         const Result s13_c18_results[] = { Result::Pass };
         const Result s13_c19_results[] = { Result::Fail };
         const Result s13_c20_results[] = { Result::Fail };
         const Result s13_c21_results[] = { Result::Fail };
         const Result s13_c22_results[] = { Result::Pass };
         const Result s13_c23_results[] = { Result::Pass };
         const Case s13_cases[] = {
            { "trailing-dot-domain", "7.1/16", "192.168.218.40", "test@example.com", "msgbas2x.cos.example.com",
              s13_c0_results, 1, 0 },
            { "trailing-dot-exp", "7.1", "192.168.218.40", "test@exp.example.com", "msgbas2x.cos.example.com",
              s13_c1_results, 1, "This is a test." },
            { "exp-only-macro-char", "7.1/8", "192.168.218.40", "test@e2.example.com", "msgbas2x.cos.example.com",
              s13_c2_results, 1, 0 },
            { "invalid-macro-char", "7.1/9", "192.168.218.40", "test@e1.example.com", "msgbas2x.cos.example.com",
              s13_c3_results, 1, 0 },
            { "invalid-embedded-macro-char", "7.1/9", "192.168.218.40", "test@e1e.example.com", "msgbas2x.cos.example.com",
              s13_c4_results, 1, 0 },
            { "invalid-trailing-macro-char", "7.1/9", "192.168.218.40", "test@e1t.example.com", "msgbas2x.cos.example.com",
              s13_c5_results, 1, 0 },
            { "macro-mania-in-domain", "7.1/3, 7.1/4", "1.2.3.4", "test@e1a.example.com", "mail.example.com",
              s13_c6_results, 1, 0 },
            { "exp-txt-macro-char", "7.1/20", "192.168.218.40", "test@e3.example.com", "msgbas2x.cos.example.com",
              s13_c7_results, 1, "Connections from 192.168.218.40 not authorized." },
            { "domain-name-truncation", "7.1/25", "192.168.218.40", "test@somewhat.long.exp.example.com", "msgbas2x.cos.example.com",
              s13_c8_results, 1, "Congratulations!  That was tricky." },
            { "v-macro-ip4", "7.1/6", "192.168.218.40", "test@e4.example.com", "msgbas2x.cos.example.com",
              s13_c9_results, 1, "192.168.218.40 is queried as 40.218.168.192.in-addr.arpa" },
            { "v-macro-ip6", "7.1/6", "CAFE:BABE::1", "test@e4.example.com", "msgbas2x.cos.example.com",
              s13_c10_results, 1, "cafe:babe::1 is queried as 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.E.B.A.B.E.F.A.C.ip6.arpa" },
            { "undef-macro", "7.1/6", "CAFE:BABE::192.168.218.40", "test@e5.example.com", "msgbas2x.cos.example.com",
              s13_c11_results, 1, 0 },
            { "p-macro-ip4-novalid", "7.1/22", "192.168.218.40", "test@e6.example.com", "msgbas2x.cos.example.com",
              s13_c12_results, 1, "connect from unknown" },
            { "p-macro-ip4-valid", "7.1/22", "192.168.218.41", "test@e6.example.com", "msgbas2x.cos.example.com",
              s13_c13_results, 1, "connect from mx.example.com" },
            { "p-macro-ip6-novalid", "7.1/22", "CAFE:BABE::1", "test@e6.example.com", "msgbas2x.cos.example.com",
              s13_c14_results, 1, "connect from unknown" },
            { "p-macro-ip6-valid", "7.1/22", "CAFE:BABE::3", "test@e6.example.com", "msgbas2x.cos.example.com",
              s13_c15_results, 1, "connect from mx.example.com" },
            { "p-macro-multiple", "7.1/22", "192.168.218.42", "test@e7.example.com", "msgbas2x.cos.example.com",
              s13_c16_results, 2, 0 },
            { "upper-macro", "7.1/26", "192.168.218.42", "~jack&jill=up-a_b3.c@e8.example.com", "msgbas2x.cos.example.com",
              s13_c17_results, 1, "http://example.com/why.html\?l=~jack%26jill%3Dup-a_b3.c" },
            { "hello-macro", "7.1/6", "192.168.218.40", "test@e9.example.com", "msgbas2x.cos.example.com",
              s13_c18_results, 1, 0 },
            { "invalid-hello-macro", "7.1/2", "192.168.218.40", "test@e9.example.com", "JUMPIN' JUPITER",
              s13_c19_results, 1, 0 },
            { "hello-domain-literal", "7.1/2", "192.168.218.40", "test@e9.example.com", "[192.168.218.40]",
              s13_c20_results, 1, 0 },
            { "require-valid-helo", "7.1/6", "1.2.3.4", "test@e10.example.com", "OEMCOMPUTER",
              s13_c21_results, 1, 0 },
            { "macro-reverse-split-on-dash", "7.1/15, 7.1/16, 7.1/17, 7.1/18", "1.2.3.4", "philip-gladstone-test@e11.example.com", "mail.example.com",
              s13_c22_results, 1, 0 },
            { "macro-multiple-delimiters", "7.1/15, 7.1/16", "1.2.3.4", "foo-bar+zip+quux@e12.example.com", "mail.example.com",
              s13_c23_results, 1, 0 },
         };

         // Processing limits
         const CharacterString s14_z0_r0[] = { { "1.2.3.4", 7 } };
         const Record s14_z0_records[] = {
            { RecordType::A, s14_z0_r0, 1 },
         };
         const CharacterString s14_z1_r0[] = { { "1.2.3.6", 7 } };
         const CharacterString s14_z1_r1[] = { { "v=spf1 ip4:1.1.1.1 redirect=e1.example.com", 42 } };
         const Record s14_z1_records[] = {
            { RecordType::A, s14_z1_r0, 1 },
            { RecordType::TXT, s14_z1_r1, 1 },
         };
         const CharacterString s14_z2_r0[] = { { "1.2.3.7", 7 } };
         const CharacterString s14_z2_r1[] = { { "v=spf1 include:e3.example.com", 29 } };
         const Record s14_z2_records[] = {
            { RecordType::A, s14_z2_r0, 1 },
            { RecordType::TXT, s14_z2_r1, 1 },
         };
         const CharacterString s14_z3_r0[] = { { "1.2.3.8", 7 } };
         const CharacterString s14_z3_r1[] = { { "v=spf1 include:e2.example.com", 29 } };
         const Record s14_z3_records[] = {
            { RecordType::A, s14_z3_r0, 1 },
            { RecordType::TXT, s14_z3_r1, 1 },
         };
         const CharacterString s14_z4_r0[] = { { "mail.example.com", 16 } };
         const CharacterString s14_z4_r1[] = { { "mail.example.com", 16 } };
         const CharacterString s14_z4_r2[] = { { "mail.example.com", 16 } };
         const CharacterString s14_z4_r3[] = { { "mail.example.com", 16 } };
         const CharacterString s14_z4_r4[] = { { "mail.example.com", 16 } };
         const CharacterString s14_z4_r5[] = { { "mail.example.com", 16 } };
         const CharacterString s14_z4_r6[] = { { "mail.example.com", 16 } };
         const CharacterString s14_z4_r7[] = { { "mail.example.com", 16 } };
         const CharacterString s14_z4_r8[] = { { "mail.example.com", 16 } };
         const CharacterString s14_z4_r9[] = { { "mail.example.com", 16 } };
         const CharacterString s14_z4_r10[] = { { "e4.example.com", 14 } };
         const CharacterString s14_z4_r11[] = { { "1.2.3.5", 7 } };
         const CharacterString s14_z4_r12[] = { { "v=spf1 mx", 9 } };
         const Record s14_z4_records[] = {
            { RecordType::MX, s14_z4_r0, 1 },
            { RecordType::MX, s14_z4_r1, 1 },
            { RecordType::MX, s14_z4_r2, 1 },
            { RecordType::MX, s14_z4_r3, 1 },
            { RecordType::MX, s14_z4_r4, 1 },
            { RecordType::MX, s14_z4_r5, 1 },
            { RecordType::MX, s14_z4_r6, 1 },
            { RecordType::MX, s14_z4_r7, 1 },
            { RecordType::MX, s14_z4_r8, 1 },
            { RecordType::MX, s14_z4_r9, 1 },
            { RecordType::MX, s14_z4_r10, 1 },
            { RecordType::A, s14_z4_r11, 1 },
            { RecordType::TXT, s14_z4_r12, 1 },
         };
         const CharacterString s14_z5_r0[] = { { "1.2.3.5", 7 } };
         const CharacterString s14_z5_r1[] = { { "v=spf1 ptr", 10 } };
         const Record s14_z5_records[] = {
            { RecordType::A, s14_z5_r0, 1 },
            { RecordType::TXT, s14_z5_r1, 1 },
         };
         const CharacterString s14_z6_r0[] = { { "e1.example.com.", 15 } };
         const CharacterString s14_z6_r1[] = { { "e2.example.com.", 15 } };
         const CharacterString s14_z6_r2[] = { { "e3.example.com.", 15 } };
         const CharacterString s14_z6_r3[] = { { "e4.example.com.", 15 } };
         const CharacterString s14_z6_r4[] = { { "example.com.", 12 } };
         const CharacterString s14_z6_r5[] = { { "e6.example.com.", 15 } };
         const CharacterString s14_z6_r6[] = { { "e7.example.com.", 15 } };
         const CharacterString s14_z6_r7[] = { { "e8.example.com.", 15 } };
         const CharacterString s14_z6_r8[] = { { "e9.example.com.", 15 } };
         const CharacterString s14_z6_r9[] = { { "e10.example.com.", 16 } };
         const CharacterString s14_z6_r10[] = { { "e5.example.com.", 15 } };
         const Record s14_z6_records[] = {
            { RecordType::PTR, s14_z6_r0, 1 },
            { RecordType::PTR, s14_z6_r1, 1 },
            { RecordType::PTR, s14_z6_r2, 1 },
            { RecordType::PTR, s14_z6_r3, 1 },
            { RecordType::PTR, s14_z6_r4, 1 },
            { RecordType::PTR, s14_z6_r5, 1 },
            { RecordType::PTR, s14_z6_r6, 1 },
            { RecordType::PTR, s14_z6_r7, 1 },
            { RecordType::PTR, s14_z6_r8, 1 },
            { RecordType::PTR, s14_z6_r9, 1 },
            { RecordType::PTR, s14_z6_r10, 1 },
         };
         const CharacterString s14_z7_r0[] = { { "1.2.3.8", 7 } };
         const CharacterString s14_z7_r1[] = { { "e6.example.com", 14 } };
         const CharacterString s14_z7_r2[] = { { "v=spf1 a mx a mx a mx a mx a ptr ip4:1.2.3.4 -all", 49 } };
         const Record s14_z7_records[] = {
            { RecordType::A, s14_z7_r0, 1 },
            { RecordType::MX, s14_z7_r1, 1 },
            { RecordType::TXT, s14_z7_r2, 1 },
         };
         const CharacterString s14_z8_r0[] = { { "1.2.3.20", 8 } };
         const CharacterString s14_z8_r1[] = { { "v=spf1 a mx a mx a mx a mx a ptr a ip4:1.2.3.4 -all", 51 } };
         const Record s14_z8_records[] = {
            { RecordType::A, s14_z8_r0, 1 },
            { RecordType::TXT, s14_z8_r1, 1 },
         };
         const CharacterString s14_z9_r0[] = { { "1.2.3.4", 7 } };
         const CharacterString s14_z9_r1[] = { { "v=spf1 a include:inc.example.com ip4:1.2.3.4 mx -all", 52 } };
         const Record s14_z9_records[] = {
            { RecordType::A, s14_z9_r0, 1 },
            { RecordType::TXT, s14_z9_r1, 1 },
         };
         const CharacterString s14_z10_r0[] = { { "1.2.3.10", 8 } };
         const CharacterString s14_z10_r1[] = { { "v=spf1 a a a a a a a a", 22 } };
         const Record s14_z10_records[] = {
            { RecordType::A, s14_z10_r0, 1 },
            { RecordType::TXT, s14_z10_r1, 1 },
         };
         const CharacterString s14_z11_r0[] = { { "1.2.3.21", 8 } };
         const CharacterString s14_z11_r1[] = { { "v=spf1 a include:inc.example.com a ip4:1.2.3.4 -all", 51 } };
         const Record s14_z11_records[] = {
            { RecordType::A, s14_z11_r0, 1 },
            { RecordType::TXT, s14_z11_r1, 1 },
         };
         const CharacterString s14_z12_r0[] = { { "1.2.3.1", 7 } };
         const CharacterString s14_z12_r1[] = { { "1.2.3.2", 7 } };
         const CharacterString s14_z12_r2[] = { { "1.2.3.3", 7 } };
         const CharacterString s14_z12_r3[] = { { "1.2.3.4", 7 } };
         const CharacterString s14_z12_r4[] = { { "1.2.3.5", 7 } };
         const CharacterString s14_z12_r5[] = { { "1.2.3.6", 7 } };
         const CharacterString s14_z12_r6[] = { { "1.2.3.7", 7 } };
         const CharacterString s14_z12_r7[] = { { "1.2.3.8", 7 } };
         const CharacterString s14_z12_r8[] = { { "1.2.3.9", 7 } };
         const CharacterString s14_z12_r9[] = { { "1.2.3.10", 8 } };
         const CharacterString s14_z12_r10[] = { { "1.2.3.11", 8 } };
         const CharacterString s14_z12_r11[] = { { "1.2.3.12", 8 } };
         const CharacterString s14_z12_r12[] = { { "v=spf1 a -all", 13 } };
         const Record s14_z12_records[] = {
            { RecordType::A, s14_z12_r0, 1 },
            { RecordType::A, s14_z12_r1, 1 },
            { RecordType::A, s14_z12_r2, 1 },
            { RecordType::A, s14_z12_r3, 1 },
            { RecordType::A, s14_z12_r4, 1 },
            { RecordType::A, s14_z12_r5, 1 },
            { RecordType::A, s14_z12_r6, 1 },
            { RecordType::A, s14_z12_r7, 1 },
            { RecordType::A, s14_z12_r8, 1 },
            { RecordType::A, s14_z12_r9, 1 },
            { RecordType::A, s14_z12_r10, 1 },
            { RecordType::A, s14_z12_r11, 1 },
            { RecordType::TXT, s14_z12_r12, 1 },
         };
         const CharacterString s14_z13_r0[] = { { "v=spf1 a:err.example.com a:err1.example.com a:err2.example.com \?all", 67 } };
         const Record s14_z13_records[] = {
            { RecordType::TXT, s14_z13_r0, 1 },
         };
         const CharacterString s14_z14_r0[] = { { "v=spf1 a:err.example.com a:err1.example.com \?all", 48 } };
         const Record s14_z14_records[] = {
            { RecordType::TXT, s14_z14_r0, 1 },
         };
         const Zone s14_zones[] = {
            { "mail.example.com", s14_z0_records, 1, false },
            { "e1.example.com", s14_z1_records, 2, false },
            { "e2.example.com", s14_z2_records, 2, false },
            { "e3.example.com", s14_z3_records, 2, false },
            { "e4.example.com", s14_z4_records, 13, false },
            { "e5.example.com", s14_z5_records, 2, false },
            { "5.3.2.1.in-addr.arpa", s14_z6_records, 11, false },
            { "e6.example.com", s14_z7_records, 3, false },
            { "e7.example.com", s14_z8_records, 2, false },
            { "e8.example.com", s14_z9_records, 2, false },
            { "inc.example.com", s14_z10_records, 2, false },
            { "e9.example.com", s14_z11_records, 2, false },
            { "e10.example.com", s14_z12_records, 13, false },
            { "e11.example.com", s14_z13_records, 1, false },
            { "e12.example.com", s14_z14_records, 1, false },
         };

         const Result s14_c0_results[] = { Result::PermError };
         const Result s14_c1_results[] = { Result::PermError };
         const Result s14_c2_results[] = { Result::PermError };
         const Result s14_c3_results[] = { Result::Neutral, Result::Pass };
         const Result s14_c4_results[] = { Result::Pass };
         const Result s14_c5_results[] = { Result::Pass };
         const Result s14_c6_results[] = { Result::PermError };
         const Result s14_c7_results[] = { Result::Pass };
         const Result s14_c8_results[] = { Result::PermError };
         const Result s14_c9_results[] = { Result::Neutral };
         const Result s14_c10_results[] = { Result::PermError };
         const Case s14_cases[] = {
            { "redirect-loop", "4.6.4/1", "1.2.3.4", "foo@e1.example.com", "mail.example.com",
              s14_c0_results, 1, 0 },
            { "include-loop", "4.6.4/1", "1.2.3.4", "foo@e2.example.com", "mail.example.com",
              s14_c1_results, 1, 0 },
            { "mx-limit", "4.6.4/2", "1.2.3.5", "foo@e4.example.com", "mail.example.com",
              s14_c2_results, 1, 0 },
            { "ptr-limit", "4.6.4/3", "1.2.3.5", "foo@e5.example.com", "mail.example.com",
              s14_c3_results, 2, 0 },
            { "false-a-limit", "4.6.4", "1.2.3.12", "foo@e10.example.com", "mail.example.com",
              s14_c4_results, 1, 0 },
            { "mech-at-limit", "4.6.4/1", "1.2.3.4", "foo@e6.example.com", "mail.example.com",
              s14_c5_results, 1, 0 },
            { "mech-over-limit", "4.6.4/1", "1.2.3.4", "foo@e7.example.com", "mail.example.com",
              s14_c6_results, 1, 0 },
            { "include-at-limit", "4.6.4/1", "1.2.3.4", "foo@e8.example.com", "mail.example.com",
              s14_c7_results, 1, 0 },
            { "include-over-limit", "4.6.4/1", "1.2.3.4", "foo@e9.example.com", "mail.example.com",
              s14_c8_results, 1, 0 },
            { "void-at-limit", "4.6.4/7", "1.2.3.4", "foo@e12.example.com", "mail.example.com",
              s14_c9_results, 1, 0 },
            { "void-over-limit", "4.6.4/7", "1.2.3.4", "foo@e11.example.com", "mail.example.com",
              s14_c10_results, 1, 0 },
         };

         // Test cases from implementation bugs
         const CharacterString s15_z0_r0[] = { { "smtp.example.org", 16 } };
         const CharacterString s15_z0_r1[] = { { "smtp1.example.com", 17 } };
         const CharacterString s15_z0_r2[] = { { "v=spf1 mx redirect=_spf.example.com", 35 } };
         const Record s15_z0_records[] = {
            { RecordType::MX, s15_z0_r0, 1 },
            { RecordType::MX, s15_z0_r1, 1 },
            { RecordType::TXT, s15_z0_r2, 1 },
         };
         const CharacterString s15_z1_r0[] = { { "198.51.100.2", 12 } };
         const CharacterString s15_z1_r1[] = { { "2001:db8:ff0:100::3", 19 } };
         const Record s15_z1_records[] = {
            { RecordType::A, s15_z1_r0, 1 },
            { RecordType::AAAA, s15_z1_r1, 1 },
         };
         const CharacterString s15_z2_r0[] = { { "192.0.2.26", 10 } };
         const CharacterString s15_z2_r1[] = { { "2001:db8:ff0:200::2", 19 } };
         const Record s15_z2_records[] = {
            { RecordType::A, s15_z2_r0, 1 },
            { RecordType::AAAA, s15_z2_r1, 1 },
         };
         const CharacterString s15_z3_r0[] = { { "smtp6-v.fe.example.org", 22 } };
         const Record s15_z3_records[] = {
            { RecordType::PTR, s15_z3_r0, 1 },
         };
         const CharacterString s15_z4_r0[] = { { "2001:db8:ff0:100::2", 19 } };
         const Record s15_z4_records[] = {
            { RecordType::AAAA, s15_z4_r0, 1 },
         };
         const CharacterString s15_z5_r0[] = { { "v=spf1 ptr:fe.example.org ptr:sgp.example.com exp=_expspf.example.org -all", 74 } };
         const Record s15_z5_records[] = {
            { RecordType::TXT, s15_z5_r0, 1 },
         };
         const CharacterString s15_z6_r0[] = { { "Sender domain not allowed from this host. Please see http://www.openspf.org/Why\?s=mfrom&id=%{S}&ip=%{C}&r=%{R}", 110 } };
         const Record s15_z6_records[] = {
            { RecordType::TXT, s15_z6_r0, 1 },
         };
         const CharacterString s15_z7_r0[] = { { "Another TXT record.", 19 } };
         const CharacterString s15_z7_r1[] = { { "v=spf1 ip4:192.0.2.225 \?include:webmail.pair.com \?include:relay.pair.com -all", 77 } };
         const CharacterString s15_z7_r2[] = { { "More TXT records.", 17 } };
         const CharacterString s15_z7_r3[] = { { "A third TXT record.", 19 } };
         const CharacterString s15_z7_r4[] = { { "2001:db8:ff0:300::4", 19 } };
         const Record s15_z7_records[] = {
            { RecordType::TXT, s15_z7_r0, 1 },
            { RecordType::TXT, s15_z7_r1, 1 },
            { RecordType::TXT, s15_z7_r2, 1 },
            { RecordType::TXT, s15_z7_r3, 1 },
            { RecordType::AAAA, s15_z7_r4, 1 },
         };
         const CharacterString s15_z8_r0[] = { { "a.example.org", 13 } };
         const Record s15_z8_records[] = {
            { RecordType::CNAME, s15_z8_r0, 1 },
         };
         const CharacterString s15_z9_r0[] = { { "192.0.2.28", 10 } };
         const CharacterString s15_z9_r1[] = { { "v=spf1 include:a.example.org include:b.example.org -all", 55 } };
         const Record s15_z9_records[] = {
            { RecordType::A, s15_z9_r0, 1 },
            { RecordType::TXT, s15_z9_r1, 1 },
         };
         const CharacterString s15_z10_r0[] = { { "v=spf1 ip4:66.39.3.0/24 ip4:209.68.6.94/32", 42 } };
         const Record s15_z10_records[] = {
            { RecordType::TXT, s15_z10_r0, 1 },
         };
         const CharacterString s15_z11_r0[] = { { "v=spf1 ip4:209.68.5.9/32 ip4:209.68.5.15/32 a -all", 50 } };
         const CharacterString s15_z11_r1[] = { { "192.0.2.131", 11 } };
         const Record s15_z11_records[] = {
            { RecordType::TXT, s15_z11_r0, 1 },
            { RecordType::A, s15_z11_r1, 1 },
         };
         const Zone s15_zones[] = {
            { "example.org", s15_z0_records, 3, false },
            { "smtp.example.org", s15_z1_records, 2, false },
            { "smtp1.example.com", s15_z2_records, 2, false },
            { "2.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.0.F.F.0.8.B.D.0.1.0.0.2.ip6.arpa", s15_z3_records, 1, false },
            { "smtp6-v.fe.example.org", s15_z4_records, 1, false },
            { "_spf.example.com", s15_z5_records, 1, false },
            { "_expspf.example.org", s15_z6_records, 1, false },
            { "a.example.org", s15_z7_records, 5, false },
            { "b.example.org", s15_z8_records, 1, false },
            { "mail.example.org", s15_z9_records, 2, false },
            { "webmail.pair.com", s15_z10_records, 1, false },
            { "relay.pair.com", s15_z11_records, 2, false },
         };

         const Result s15_c0_results[] = { Result::Pass };
         const Result s15_c1_results[] = { Result::Fail };
         const Case s15_cases[] = {
            { "bytes-bug", "5.4/4", "2001:db8:ff0:100::2", "test@example.org", "example.org",
              s15_c0_results, 1, 0 },
            { "cname-aliasing", "3.2/1", "192.0.2.27", "test@mail.example.org", "mail.example.org",
              s15_c1_results, 1, 0 },
         };

      }

      extern const Section Sections[] = {
         { "Initial processing",
           s0_zones, 14, s0_cases, 16 },
         { "Record lookup",
           s1_zones, 7, s1_cases, 7 },
         { "Selecting records",
           s2_zones, 10, s2_cases, 10 },
         { "Record evaluation",
           s3_zones, 13, s3_cases, 12 },
         { "ALL mechanism syntax",
           s4_zones, 6, s4_cases, 5 },
         { "PTR mechanism syntax",
           s5_zones, 14, s5_cases, 8 },
         { "A mechanism syntax",
           s6_zones, 27, s6_cases, 29 },
         { "Include mechanism semantics and syntax",
           s7_zones, 15, s7_cases, 9 },
         { "MX mechanism syntax",
           s8_zones, 19, s8_cases, 21 },
         { "EXISTS mechanism syntax",
           s9_zones, 9, s9_cases, 7 },
         { "IP4 mechanism syntax",
           s10_zones, 10, s10_cases, 9 },
         { "IP6 mechanism syntax",
           s11_zones, 7, s11_cases, 9 },
         { "Semantics of exp and other modifiers",
           s12_zones, 37, s12_cases, 24 },
         { "Macro expansion rules",
           s13_zones, 41, s13_cases, 24 },
         { "Processing limits",
           s14_zones, 15, s14_cases, 11 },
         { "Test cases from implementation bugs",
           s15_zones, 12, s15_cases, 2 },
      };

      extern const int SectionCount = 16;
      extern const int CaseCount = 203;
   }
}
