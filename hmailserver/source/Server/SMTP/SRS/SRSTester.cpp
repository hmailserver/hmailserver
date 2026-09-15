// Copyright (c) 2026 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SRSTester.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   // A fixed secret and a fixed point in time, so that every address these tests create
   // is the same one on every run. 2026-01-01 00:00:00 UTC is day 20454 since the epoch,
   // which is 998 into the 1024-day cycle the timestamp counts in, and 998 encodes as
   // "7G" in the base32 alphabet SRS uses.
   namespace
   {
      const wchar_t * const TestSecret = _T("hMailServer SRS unit test secret");
      const time_t TestTime = 1767225600;
      const wchar_t * const TestTimestamp = _T("7G");
      const int TestMaxAgeDays = 21;
      const int TestHashLength = 8;
      const int SecondsPerTestDay = 60 * 60 * 24;
   }

   void
   SRSTester::Test()
   {
      TestForwardFormat_();
      TestRoundTrip_();
      TestCaseHandling_();
      TestSeparators_();
      TestTampering_();
      TestHashLengthChanges_();
      TestMalformedAddresses_();
      TestTimestamps_();
      TestChaining_();
      TestVersionConfusion_();
      TestLimits_();
      TestSecretGeneration_();
      TestQuotedLocalParts_();
      TestNonAsciiSecret_();
   }

   SRS
   SRSTester::CreateSRS_()
   {
      return SRS(TestSecret, TestMaxAgeDays, TestHashLength);
   }

   void
   SRSTester::AssertTrue_(bool value)
   {
      if (!value)
      {
         assert(0);
         throw;
      }
   }

   void
   SRSTester::AssertEqual_(const String &expected, const String &actual)
   {
      if (expected.Compare(actual) != 0)
      {
         assert(0);
         throw;
      }
   }

   void
   SRSTester::AssertReverse_(const SRS &srs, const String &address, SRS::ReverseResult expectedResult)
   {
      String originalAddress;

      if (srs.Reverse(address, TestTime, originalAddress) != expectedResult)
      {
         assert(0);
         throw;
      }
   }

   void
   SRSTester::TestForwardFormat_()
   {
      SRS srs = CreateSRS_();

      String address = srs.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      // SRS0=<hash>=<timestamp>=<domain>=<local part>@<forwarding domain>
      std::vector<String> parts = StringParser::SplitString(address, _T("="));

      AssertTrue_(parts.size() == 5);
      AssertEqual_(_T("SRS0"), parts[0]);
      AssertTrue_(parts[1].GetLength() == TestHashLength);
      AssertEqual_(TestTimestamp, parts[2]);
      AssertEqual_(_T("example.com"), parts[3]);
      AssertEqual_(_T("user@forwarder.test"), parts[4]);

      AssertTrue_(SRS::IsSrsAddress(address));

      // The address the sender is rewritten into is one of ours, so that a bounce comes
      // back here rather than to the domain the message came from.
      AssertEqual_(_T("forwarder.test"), StringParser::ExtractDomain(address));
   }

   void
   SRSTester::TestRoundTrip_()
   {
      SRS srs = CreateSRS_();

      String address = srs.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      String originalAddress;
      AssertTrue_(srs.Reverse(address, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("user@example.com"), originalAddress);

      // A local part may contain the character SRS separates its fields with. It is the
      // last field of the address, so it is not split any further.
      address = srs.Forward(_T("od==d@example.com"), _T("forwarder.test"), TestTime);
      AssertTrue_(srs.Reverse(address, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("od==d@example.com"), originalAddress);

      // As may a sub-domain, and a plus-addressed local part.
      address = srs.Forward(_T("user+tag@mail.example.co.uk"), _T("forwarder.test"), TestTime);
      AssertTrue_(srs.Reverse(address, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("user+tag@mail.example.co.uk"), originalAddress);
   }

   void
   SRSTester::TestCaseHandling_()
   {
      SRS srs = CreateSRS_();

      String address = srs.Forward(_T("User.Name@Example.COM"), _T("forwarder.test"), TestTime);

      String originalAddress;
      AssertTrue_(srs.Reverse(address, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("User.Name@Example.COM"), originalAddress);

      // Not every mail server preserves the case of a local part it bounces to. An
      // address which comes back in a different case must still validate.
      String lowerCased = address;
      lowerCased.ToLower();

      AssertTrue_(srs.Reverse(lowerCased, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("user.name@example.com"), originalAddress);

      String upperCased = address;
      upperCased.ToUpper();

      AssertTrue_(srs.Reverse(upperCased, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("USER.NAME@EXAMPLE.COM"), originalAddress);
   }

   void
   SRSTester::TestSeparators_()
   {
      SRS srs = CreateSRS_();

      String address = srs.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      // Only the character which follows the tag varies between implementations, and all
      // three of them are accepted. It is not part of what is hashed.
      String withPlus = address;
      withPlus.SetAt(4, '+');

      String withMinus = address;
      withMinus.SetAt(4, '-');

      String originalAddress;

      AssertTrue_(srs.Reverse(withPlus, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("user@example.com"), originalAddress);

      AssertTrue_(srs.Reverse(withMinus, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("user@example.com"), originalAddress);

      AssertTrue_(SRS::IsSrsAddress(withPlus));
      AssertTrue_(SRS::IsSrsAddress(withMinus));

      // Anything else is not an SRS address.
      String withColon = address;
      withColon.SetAt(4, ':');

      AssertTrue_(!SRS::IsSrsAddress(withColon));
      AssertReverse_(srs, withColon, SRS::ResultNotAnSrsAddress);
   }

   void
   SRSTester::TestTampering_()
   {
      SRS srs = CreateSRS_();

      String address = srs.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      // Without the secret, an address which decodes to a recipient of the sender's
      // choosing cannot be created - which is what keeps this from being an open relay.
      AssertReverse_(srs, _T("SRS0=AAAAAAAA=7G=attacker.example=victim@forwarder.test"), SRS::ResultInvalidHash);

      // Changing any of the fields the hash covers invalidates it.
      String differentDomain = address;
      differentDomain.Replace(_T("=example.com="), _T("=example.org="));
      AssertReverse_(srs, differentDomain, SRS::ResultInvalidHash);

      String differentLocalPart = address;
      differentLocalPart.Replace(_T("=user@"), _T("=root@"));
      AssertReverse_(srs, differentLocalPart, SRS::ResultInvalidHash);

      String differentTimestamp = address;
      differentTimestamp.Replace(String(_T("=")) + TestTimestamp + _T("="), _T("=AA="));
      AssertReverse_(srs, differentTimestamp, SRS::ResultInvalidHash);

      // An address created by another server, with another secret, is not ours to
      // reverse either.
      SRS otherServer("another secret", TestMaxAgeDays, TestHashLength);
      String otherAddress = otherServer.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      AssertTrue_(otherAddress.Compare(address) != 0);
      AssertReverse_(srs, otherAddress, SRS::ResultInvalidHash);

      // A hash can be made longer than the default but not shorter: a hash is compared
      // without regard to case, so the minimum is already only 42 bits, and taking
      // characters off it is what would make an address worth guessing at.
      AssertTrue_(SRS::MinHashLength == SRS::DefaultHashLength);

      SRS shortHash(TestSecret, TestMaxAgeDays, 1);
      String shortAddress = shortHash.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      std::vector<String> parts = StringParser::SplitString(shortAddress, _T("="));
      AssertTrue_(parts[1].GetLength() == SRS::MinHashLength);

      String reversed;
      AssertTrue_(shortHash.Reverse(shortAddress, TestTime, reversed) == SRS::ResultSuccess);
      AssertEqual_(_T("user@example.com"), reversed);
   }

   void
   SRSTester::TestHashLengthChanges_()
   {
      SRS srs = CreateSRS_();

      String address = srs.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      String originalAddress;

      // The hash in an address is as long as the setting was when the address was handed
      // out. Bounces for those addresses keep arriving for as long as they are valid, so
      // changing the setting must not strand them - in either direction.
      SRS longer(TestSecret, TestMaxAgeDays, TestHashLength + 4);

      AssertTrue_(longer.Reverse(address, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("user@example.com"), originalAddress);

      String longAddress = longer.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);
      std::vector<String> longParts = StringParser::SplitString(longAddress, _T("="));
      AssertTrue_(longParts[1].GetLength() == TestHashLength + 4);

      AssertTrue_(srs.Reverse(longAddress, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("user@example.com"), originalAddress);

      // How many characters of hash an address carries is the sender's choice, so a hash
      // shorter than the minimum is rejected rather than compared as far as it goes.
      std::vector<String> parts = StringParser::SplitString(address, _T("="));

      String truncated = parts[0] + _T("=") + parts[1].Mid(0, SRS::MinHashLength - 1) +
                         _T("=") + parts[2] + _T("=") + parts[3] + _T("=") + parts[4];

      AssertReverse_(srs, truncated, SRS::ResultInvalidHash);

      // As is one longer than any this server would ever create.
      String padding;
      for (int i = 0; i < SRS::MaxHashLength; i++)
         padding += _T("a");

      String overlong = parts[0] + _T("=") + parts[1] + padding +
                        _T("=") + parts[2] + _T("=") + parts[3] + _T("=") + parts[4];

      AssertReverse_(srs, overlong, SRS::ResultInvalidHash);
   }

   void
   SRSTester::TestMalformedAddresses_()
   {
      SRS srs = CreateSRS_();

      // Ordinary addresses are not SRS addresses, and are left for the rest of the
      // server to make sense of.
      AssertTrue_(!SRS::IsSrsAddress(_T("user@example.com")));
      AssertTrue_(!SRS::IsSrsAddress(_T("")));
      AssertTrue_(!SRS::IsSrsAddress(_T("user")));
      AssertTrue_(!SRS::IsSrsAddress(_T("@example.com")));
      AssertTrue_(!SRS::IsSrsAddress(_T("SRS0=a=b=c=d")));
      AssertTrue_(!SRS::IsSrsAddress(_T("SRS2=a=b=c=d@example.com")));
      AssertTrue_(!SRS::IsSrsAddress(_T("SRS0=@example.com")));
      AssertTrue_(!SRS::IsSrsAddress(_T("SRS0@example.com")));

      AssertReverse_(srs, _T("user@example.com"), SRS::ResultNotAnSrsAddress);
      AssertReverse_(srs, _T("SRS0=a=b=c=d"), SRS::ResultNotAnSrsAddress);
      AssertReverse_(srs, _T("SRS2=a=b=c=d@example.com"), SRS::ResultNotAnSrsAddress);

      // The tag is there, but the fields behind it are not.
      AssertReverse_(srs, _T("SRS0=a@example.com"), SRS::ResultMalformed);
      AssertReverse_(srs, _T("SRS0=hash=7G@example.com"), SRS::ResultMalformed);
      AssertReverse_(srs, _T("SRS0=hash=7G=example.com@forwarder.test"), SRS::ResultMalformed);
      AssertReverse_(srs, _T("SRS0=hash=7G=example.com=@forwarder.test"), SRS::ResultMalformed);
      AssertReverse_(srs, _T("SRS1=hash@example.com"), SRS::ResultMalformed);
      AssertReverse_(srs, _T("SRS1=hash=hop.test@forwarder.test"), SRS::ResultMalformed);

      // An SRS1 address whose embedded SRS0 part does not start with a separator of its
      // own is malformed rather than merely unsigned.
      AssertReverse_(srs, _T("SRS1=hash=hop.test=a=b=c@forwarder.test"), SRS::ResultMalformed);

      // Without a secret, nothing can be validated, and nothing is rewritten either.
      SRS notConfigured("", TestMaxAgeDays, TestHashLength);

      String address = srs.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);
      AssertReverse_(notConfigured, address, SRS::ResultNotConfigured);
      AssertTrue_(notConfigured.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime).IsEmpty());

      // Neither is an address we cannot make sense of in the first place.
      AssertTrue_(srs.Forward(_T(""), _T("forwarder.test"), TestTime).IsEmpty());
      AssertTrue_(srs.Forward(_T("user"), _T("forwarder.test"), TestTime).IsEmpty());
      AssertTrue_(srs.Forward(_T("@example.com"), _T("forwarder.test"), TestTime).IsEmpty());
      AssertTrue_(srs.Forward(_T("user@"), _T("forwarder.test"), TestTime).IsEmpty());
      AssertTrue_(srs.Forward(_T("user@example.com"), _T(""), TestTime).IsEmpty());
   }

   void
   SRSTester::TestTimestamps_()
   {
      SRS srs = CreateSRS_();

      String address = srs.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      String originalAddress;

      // Valid for as many days as it is configured to be, and no longer.
      AssertTrue_(srs.Reverse(address, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertTrue_(srs.Reverse(address, TestTime + TestMaxAgeDays * SecondsPerTestDay, originalAddress) == SRS::ResultSuccess);
      AssertTrue_(srs.Reverse(address, TestTime + (TestMaxAgeDays + 1) * SecondsPerTestDay, originalAddress) == SRS::ResultExpired);
      AssertTrue_(srs.Reverse(address, TestTime + 400 * SecondsPerTestDay, originalAddress) == SRS::ResultExpired);

      // A clock which runs a little behind the one of the server which created the address
      // - far enough behind for the two of them to be on different days - still validates
      // it. TestTime is midnight, so half an hour earlier is the day before.
      AssertTrue_(srs.Reverse(address, TestTime - 1800, originalAddress) == SRS::ResultSuccess);

      // More than that is not a clock difference. Modulo the day counter, a timestamp from
      // tomorrow and one from TimestampCycle - 1 days ago are written the same way, so an
      // address which is a whole cycle old must not be let through as tomorrow's.
      AssertTrue_(srs.Reverse(address, TestTime - SecondsPerTestDay, originalAddress) == SRS::ResultExpired);
      AssertTrue_(srs.Reverse(address, TestTime + 1023 * SecondsPerTestDay, originalAddress) == SRS::ResultExpired);

      // The hash is checked before the age is, so a forged address which has also
      // expired is reported as forged.
      String expiredAndForged = address;
      expiredAndForged.Replace(_T("=example.com="), _T("=example.org="));
      AssertTrue_(srs.Reverse(expiredAndForged, TestTime + 400 * SecondsPerTestDay, originalAddress) == SRS::ResultInvalidHash);

      // The counter wraps rather than growing, so an address created just before a wrap
      // is still valid just after one.
      time_t justBeforeWrap = (time_t) ((TestTime / SecondsPerTestDay + 1023 - 998) * (time_t) SecondsPerTestDay);
      String wrappingAddress = srs.Forward(_T("user@example.com"), _T("forwarder.test"), justBeforeWrap);

      AssertTrue_(srs.Reverse(wrappingAddress, justBeforeWrap, originalAddress) == SRS::ResultSuccess);
      AssertTrue_(srs.Reverse(wrappingAddress, justBeforeWrap + 5 * SecondsPerTestDay, originalAddress) == SRS::ResultSuccess);
      AssertTrue_(srs.Reverse(wrappingAddress, justBeforeWrap + (TestMaxAgeDays + 1) * SecondsPerTestDay, originalAddress) == SRS::ResultExpired);

      // A timestamp which is not in the alphabet the counter is written in, or is the
      // wrong length, belongs to no day at all. The hash covers the timestamp, so such
      // an address is rejected before its age is ever considered.
      AssertReverse_(srs, _T("SRS0=hash=!!=example.com=user@forwarder.test"), SRS::ResultInvalidHash);
      AssertReverse_(srs, _T("SRS0=hash=7GG=example.com=user@forwarder.test"), SRS::ResultInvalidHash);
   }

   void
   SRSTester::TestChaining_()
   {
      SRS srs = CreateSRS_();

      // A message which has been forwarded once already arrives with a sender another
      // server has rewritten. Forwarding it again keeps that server as the first hop
      // rather than wrapping one rewritten address inside another.
      String firstHop = srs.Forward(_T("user@example.com"), _T("hop1.test"), TestTime);
      String secondHop = srs.Forward(firstHop, _T("hop2.test"), TestTime);

      AssertTrue_(secondHop.StartsWith(_T("SRS1=")));
      AssertTrue_(secondHop.Contains(_T("=hop1.test==")));
      AssertEqual_(_T("hop2.test"), StringParser::ExtractDomain(secondHop));

      // Reversing it yields the address the first hop handed out, which that server can
      // reverse in turn.
      String originalAddress;
      AssertTrue_(srs.Reverse(secondHop, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(firstHop, originalAddress);

      // A third hop keeps the first one, and the address stops growing.
      String thirdHop = srs.Forward(secondHop, _T("hop3.test"), TestTime);

      AssertTrue_(thirdHop.StartsWith(_T("SRS1=")));
      AssertTrue_(thirdHop.Contains(_T("=hop1.test==")));
      AssertTrue_(thirdHop.GetLength() == secondHop.GetLength() - String(_T("hop2.test")).GetLength() + String(_T("hop3.test")).GetLength());

      AssertTrue_(srs.Reverse(thirdHop, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(firstHop, originalAddress);

      // The SRS0 payload an SRS1 address carries is covered by our own hash, so its
      // timestamp is as much ours to go by as the one in an SRS0 address. Without that an
      // SRS1 address would never expire, and a single forward would leave an address
      // relaying mail to a domain of somebody else's choosing for good.
      AssertTrue_(srs.Reverse(secondHop, TestTime + (TestMaxAgeDays + 1) * SecondsPerTestDay, originalAddress) == SRS::ResultExpired);
      AssertTrue_(srs.Reverse(secondHop, TestTime + 400 * SecondsPerTestDay, originalAddress) == SRS::ResultExpired);

      String expiredFirstHop = srs.Forward(_T("user@example.com"), _T("hop1.test"), TestTime - 400 * SecondsPerTestDay);
      String expiredSecondHop = srs.Forward(expiredFirstHop, _T("hop2.test"), TestTime);

      AssertReverse_(srs, expiredSecondHop, SRS::ResultExpired);

      // Forging one is no easier than forging an SRS0 address.
      String forged = secondHop;
      forged.Replace(_T("=hop1.test=="), _T("=hop9.test=="));
      AssertReverse_(srs, forged, SRS::ResultInvalidHash);

      // A sender whose local part starts with the tag but holds none of the fields a
      // rewritten sender holds was never rewritten by anybody - a mailing list posting as
      // srs0-bounces@ is the everyday case. Chaining it would produce an address which
      // reverses to one that never existed, so it is rewritten as any other sender is.
      String notReallyRewritten = srs.Forward(_T("srs0-bounces@lists.example.org"), _T("hop2.test"), TestTime);

      AssertTrue_(notReallyRewritten.StartsWith(_T("SRS0=")));
      AssertTrue_(srs.Reverse(notReallyRewritten, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("srs0-bounces@lists.example.org"), originalAddress);

      // The same where the fields are there but the timestamp belongs to no day, and
      // where an SRS1 address does not hold the fields one of ours holds.
      String unparsableTimestamp = srs.Forward(_T("SRS0=abcdefgh=!!=example.com=user@lists.example.org"), _T("hop2.test"), TestTime);

      AssertTrue_(unparsableTimestamp.StartsWith(_T("SRS0=")));
      AssertTrue_(srs.Reverse(unparsableTimestamp, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("SRS0=abcdefgh=!!=example.com=user@lists.example.org"), originalAddress);

      String notReallyChained = srs.Forward(_T("SRS1=hash=hop.test@example.com"), _T("hop2.test"), TestTime);

      AssertTrue_(notReallyChained.StartsWith(_T("SRS0=")));
      AssertTrue_(srs.Reverse(notReallyChained, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("SRS1=hash=hop.test@example.com"), originalAddress);

      // A tag is only a tag where one of the separators follows it. Nothing separates this
      // one from the rest of the local part, so the sender was never rewritten by anybody,
      // whichever of the two tags it happens to begin with.
      String unseparatedTag = srs.Forward(_T("SRS1_hash=hop.test@example.com"), _T("hop2.test"), TestTime);

      AssertTrue_(unseparatedTag.StartsWith(_T("SRS0=")));
      AssertTrue_(srs.Reverse(unseparatedTag, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("SRS1_hash=hop.test@example.com"), originalAddress);

      String unseparatedSrs0Tag = srs.Forward(_T("SRS0_hash=7G=example.com=user@example.org"), _T("hop2.test"), TestTime);

      AssertTrue_(unseparatedSrs0Tag.StartsWith(_T("SRS0=")));
      AssertTrue_(srs.Reverse(unseparatedSrs0Tag, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("SRS0_hash=7G=example.com=user@example.org"), originalAddress);

      // A timestamp written in characters which are not the ones the counter uses, but
      // whose low bytes are. Narrowing the comparison to a char would read U+0141 as 'A'
      // and U+0147 as 'G', and an address which was never really rewritten would be
      // chained rather than rewritten as the ordinary sender it is.
      String wideTimestamp = _T("SRS0=abcdefgh=\x0141\x0147=example.com=user@lists.example.org");

      String notReallyRewrittenWideTimestamp = srs.Forward(wideTimestamp, _T("hop2.test"), TestTime);

      AssertTrue_(notReallyRewrittenWideTimestamp.StartsWith(_T("SRS0=")));
      AssertTrue_(srs.Reverse(notReallyRewrittenWideTimestamp, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(wideTimestamp, originalAddress);

      // A hash shorter than we would ever write is still chained: other implementations
      // default to four characters, and their addresses have to keep working.
      String shortHashFirstHop = String(_T("SRS0=abcd=")) + TestTimestamp + _T("=example.com=user@hop1.test");
      String shortHashSecondHop = srs.Forward(shortHashFirstHop, _T("hop2.test"), TestTime);

      AssertTrue_(shortHashSecondHop.StartsWith(_T("SRS1=")));
      AssertTrue_(srs.Reverse(shortHashSecondHop, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(shortHashFirstHop, originalAddress);
   }

   void
   SRSTester::TestVersionConfusion_()
   {
      SRS srs = CreateSRS_();

      // What the two forms hash is easy to make into the same string. An SRS0 address
      // covers "<timestamp>=<domain>=<local part>"; an SRS1 address covers "<first hop>"
      // followed by the embedded SRS0 payload, which opens with a separator of its own.
      // A sender whose domain reads like a hash and whose local part reads like the tail
      // of an SRS0 payload therefore gets an address whose signature would carry straight
      // over to an SRS1 address this server never handed out - unless the version is part
      // of what is hashed.
      String sender = String(_T("7g=example.com=user@evil.test"));

      String address = srs.Forward(sender, _T("forwarder.test"), TestTime);

      String originalAddress;
      AssertTrue_(srs.Reverse(address, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(sender, originalAddress);

      std::vector<String> parts = StringParser::SplitString(address, _T("="));
      AssertEqual_(_T("SRS0"), parts[0]);

      // The very same hash, presented as one over the other form: the first hop is the
      // timestamp of the address above, and the hash the crafted address embeds is that
      // address's sender domain. Reversing it would yield an address at the two-character
      // domain "7G", which this server never created and must not sign for.
      String crafted = String(_T("SRS1=")) + parts[1] + _T("=") + TestTimestamp +
                       _T("==evil.test=7g=example.com=user@forwarder.test");

      AssertReverse_(srs, crafted, SRS::ResultInvalidHash);

      // The same the other way round: the hash of an SRS1 address we did create, offered
      // back as the hash of an SRS0 address. The one separator which tells the two forms
      // apart is dropped, and what is left reads as an SRS0 address whose timestamp is the
      // first hop - so the hash would be accepted, and the address turned away only for
      // having a timestamp belonging to no day, which says that the hash was ours.
      String firstHop = srs.Forward(_T("user@example.com"), _T("hop1.test"), TestTime);
      String secondHop = srs.Forward(firstHop, _T("hop2.test"), TestTime);

      std::vector<String> firstHopParts = StringParser::SplitString(firstHop, _T("="));
      std::vector<String> chainedParts = StringParser::SplitString(secondHop, _T("="));

      AssertEqual_(_T("SRS1"), chainedParts[0]);

      String craftedSrs0 = String(_T("SRS0=")) + chainedParts[1] + _T("=hop1.test=") + firstHopParts[1] +
                           _T("=") + TestTimestamp + _T("=example.com=user@hop2.test");

      AssertReverse_(srs, craftedSrs0, SRS::ResultInvalidHash);
   }

   void
   SRSTester::TestQuotedLocalParts_()
   {
      SRS srs = CreateSRS_();

      // A local part which only holds together inside quotes cannot be embedded in an
      // address as it is: the result is neither one the next server would accept, nor one
      // we would accept the bounce for. The message is forwarded unrewritten instead.
      AssertTrue_(srs.Forward(_T("\"john doe\"@example.com"), _T("forwarder.test"), TestTime).IsEmpty());
      AssertTrue_(srs.Forward(_T("\"a@b\"@example.com"), _T("forwarder.test"), TestTime).IsEmpty());

      // Nor is an address built around a domain which is not one.
      AssertTrue_(srs.Forward(_T("user@example.com"), _T("not a domain"), TestTime).IsEmpty());

      // What is rewritten is what the rest of the server accepts as an address, so a
      // bounce to it comes back through RCPT TO rather than being refused there.
      String address = srs.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      AssertTrue_(StringParser::IsValidEmailAddress(address));
   }

   void
   SRSTester::TestNonAsciiSecret_()
   {
      // The secret is hashed as the bytes it is written in. Narrowing it through the code
      // page of the machine instead loses whatever that code page has no room for, so two
      // servers sharing a database could end up signing with different keys - and two
      // different secrets could end up signing with the same one.
      SRS first(_T("secret-\x3042"), TestMaxAgeDays, TestHashLength);
      SRS second(_T("secret-\x3044"), TestMaxAgeDays, TestHashLength);

      String firstAddress = first.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);
      String secondAddress = second.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      AssertTrue_(firstAddress.Compare(secondAddress) != 0);

      String originalAddress;
      AssertTrue_(first.Reverse(firstAddress, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("user@example.com"), originalAddress);

      AssertReverse_(second, firstAddress, SRS::ResultInvalidHash);

      // The same secret gives the same address, whichever object it is written into.
      SRS sameAsFirst(_T("secret-\x3042"), TestMaxAgeDays, TestHashLength);

      AssertEqual_(firstAddress, sameAsFirst.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime));
   }

   void
   SRSTester::TestLimits_()
   {
      SRS srs = CreateSRS_();

      // An address which would end up too long to send, or to store, is left alone.
      String longLocalPart;
      for (int i = 0; i < 240; i++)
         longLocalPart += _T("a");

      AssertTrue_(srs.Forward(longLocalPart + _T("@example.com"), _T("forwarder.test"), TestTime).IsEmpty());

      String address = srs.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);
      AssertTrue_(address.GetLength() <= SRS::MaxAddressLength);

      // The limit is the one the rest of the server enforces on an address, so an address
      // which is rewritten is one the bounce for it will also be accepted at. The rewrite
      // is "SRS0=" + hash + "=" + timestamp + "=" + domain + "=" + local part + "@" + the
      // forwarding domain, which is 19 characters plus the three of variable length.
      const int fixedLength = 19 + String(_T("example.com")).GetLength() + String(_T("forwarder.test")).GetLength();

      String longestLocalPart;
      for (int i = 0; i < SRS::MaxAddressLength - fixedLength; i++)
         longestLocalPart += _T("a");

      String longestAddress = srs.Forward(longestLocalPart + _T("@example.com"), _T("forwarder.test"), TestTime);
      AssertTrue_(longestAddress.GetLength() == SRS::MaxAddressLength);

      // One character more, and the sender is left as it is rather than rewritten into an
      // address which would be refused.
      AssertTrue_(srs.Forward(longestLocalPart + _T("a@example.com"), _T("forwarder.test"), TestTime).IsEmpty());

      // The hash length and the maximum age are clamped to what makes sense, so that a
      // misconfiguration cannot produce an address nobody can validate.
      SRS tooSmall(TestSecret, 0, 1);
      String tooSmallAddress = tooSmall.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);
      std::vector<String> parts = StringParser::SplitString(tooSmallAddress, _T("="));
      AssertTrue_(parts[1].GetLength() == SRS::MinHashLength);

      String originalAddress;
      AssertTrue_(tooSmall.Reverse(tooSmallAddress, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertTrue_(tooSmall.Reverse(tooSmallAddress, TestTime + 2 * SecondsPerTestDay, originalAddress) == SRS::ResultExpired);

      SRS tooLarge(TestSecret, 100000, 100);
      String tooLargeAddress = tooLarge.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);
      parts = StringParser::SplitString(tooLargeAddress, _T("="));
      AssertTrue_(parts[1].GetLength() == SRS::MaxHashLength);

      AssertTrue_(tooLarge.Reverse(tooLargeAddress, TestTime + SRS::MaxMaxAgeDays * SecondsPerTestDay, originalAddress) == SRS::ResultSuccess);
   }

   void
   SRSTester::TestSecretGeneration_()
   {
      AnsiString secret = SRS::GenerateSecret();
      AnsiString otherSecret = SRS::GenerateSecret();

      AssertTrue_(!secret.IsEmpty());
      AssertTrue_(secret.GetLength() >= SRS::SecretLength);
      AssertTrue_(secret.Compare(otherSecret) != 0);

      // A generated secret is a usable one.
      SRS srs(secret, TestMaxAgeDays, TestHashLength);

      String address = srs.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      String originalAddress;
      AssertTrue_(srs.Reverse(address, TestTime, originalAddress) == SRS::ResultSuccess);
      AssertEqual_(_T("user@example.com"), originalAddress);
   }
}
