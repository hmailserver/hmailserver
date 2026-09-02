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
      const AnsiString TestSecret = "hMailServer SRS unit test secret";
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
      TestMalformedAddresses_();
      TestTimestamps_();
      TestChaining_();
      TestLimits_();
      TestSecretGeneration_();
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

      // A shorter hash is a weaker one, but it is still checked.
      SRS shortHash(TestSecret, TestMaxAgeDays, SRS::MinHashLength);
      String shortAddress = shortHash.Forward(_T("user@example.com"), _T("forwarder.test"), TestTime);

      std::vector<String> parts = StringParser::SplitString(shortAddress, _T("="));
      AssertTrue_(parts[1].GetLength() == SRS::MinHashLength);

      String reversed;
      AssertTrue_(shortHash.Reverse(shortAddress, TestTime, reversed) == SRS::ResultSuccess);
      AssertEqual_(_T("user@example.com"), reversed);
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

      // A single day of clock difference between the server which created the address
      // and the one reversing it is tolerated; more than that is an address from before
      // the day counter last wrapped.
      AssertTrue_(srs.Reverse(address, TestTime - SecondsPerTestDay, originalAddress) == SRS::ResultSuccess);
      AssertTrue_(srs.Reverse(address, TestTime - 2 * SecondsPerTestDay, originalAddress) == SRS::ResultExpired);

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

      // The timestamp in an SRS1 address belongs to the first hop, which is the server
      // that can tell whether it is still valid. Ours is the hash, and only that is
      // checked here.
      AssertTrue_(srs.Reverse(secondHop, TestTime + 400 * SecondsPerTestDay, originalAddress) == SRS::ResultSuccess);

      // Forging one is no easier than forging an SRS0 address.
      String forged = secondHop;
      forged.Replace(_T("=hop1.test=="), _T("=hop9.test=="));
      AssertReverse_(srs, forged, SRS::ResultInvalidHash);

      // Chaining an address we cannot take apart yields nothing, and the sender is left
      // as it is.
      AssertTrue_(srs.Forward(_T("SRS1=hash=hop.test@example.com"), _T("hop2.test"), TestTime).IsEmpty());
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
