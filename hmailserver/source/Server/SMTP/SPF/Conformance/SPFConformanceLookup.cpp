// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFConformanceLookup.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace SPFConformance
   {
      namespace
      {
         // DNS compares names without regard to the case of ASCII letters, and
         // only of ASCII letters - RFC 4343. The suite relies on it: it writes
         // some of its zone names in mixed case, and some of the names its
         // records point at differ from the zone they name in nothing but case.
         // A trailing dot says only that the name is already absolute, which
         // every name here is.
         AnsiString Normalize(const AnsiString &name)
         {
            int length = name.GetLength();

            if (length > 0 && name[length - 1] == '.')
               length--;

            AnsiString result;

            for (int i = 0; i < length; i++)
            {
               char character = name[i];

               if (character >= 'A' && character <= 'Z')
                  character = (char) (character - 'A' + 'a');

               result += character;
            }

            return result;
         }

         // The bytes of a record. A TXT record is a sequence of
         // character-strings which RFC 7208 section 3.3 joins with nothing
         // between them; every other type carries a single value.
         //
         // Copied a byte at a time because two of the suite's policies hold a
         // NUL octet, and appending them as C strings would drop it along with
         // the reason those two cases exist.
         AnsiString Join(const Record &record)
         {
            AnsiString result;

            for (int i = 0; i < record.stringCount; i++)
            {
               const CharacterString &string = record.strings[i];

               for (int j = 0; j < string.length; j++)
                  result += string.bytes[j];
            }

            return result;
         }
      }

      ConformanceLookup::ConformanceLookup(const Section &section) :
         section_(section),
         queryCount_(0)
      {

      }

      const Zone *
      ConformanceLookup::FindZone_(const AnsiString &name) const
      {
         AnsiString wanted = Normalize(name);

         for (int i = 0; i < section_.zoneCount; i++)
         {
            if (Normalize(section_.zones[i].name) == wanted)
               return &section_.zones[i];
         }

         return 0;
      }

      bool
      ConformanceLookup::Collect_(const Zone &zone, RecordType type, std::vector<AnsiString> &values) const
      {
         bool found = false;

         for (int i = 0; i < zone.recordCount; i++)
         {
            if (zone.records[i].type != type)
               continue;

            values.push_back(Join(zone.records[i]));
            found = true;
         }

         return found;
      }

      bool
      ConformanceLookup::Lookup_(const AnsiString &name, RecordType type, std::vector<AnsiString> &values)
      {
         queryCount_++;

         const Zone *zone = FindZone_(name);

         if (zone == 0)
         {
            // A name the section says nothing about does not exist. That is an
            // empty answer rather than a failure; RFC 7208 section 4.3 has the
            // caller decide what to make of it.
            return true;
         }

         bool found = Collect_(*zone, type, values);

         // A CNAME is followed one step and no further: what the alias points at
         // is read for records of the type asked for, and its own aliases are
         // not followed in turn. That is what the suite's own driver does, and
         // it is why cname.example.com, which is an alias for itself, and
         // loop4.example.com, which is an alias for that, do not run away.
         // Nothing in the suite needs a longer chain.
         for (int i = 0; i < zone->recordCount; i++)
         {
            if (zone->records[i].type != RecordType::CNAME)
               continue;

            const Zone *target = FindZone_(Join(zone->records[i]));

            // An alias for itself would otherwise have its records counted
            // twice.
            if (target != 0 && target != zone)
               Collect_(*target, type, values);
         }

         // Every timing-out zone in the suite carries the marker as its last
         // record, so what decides whether the query is answered is only
         // whether the zone holds a record of the type asked for. The
         // generator asserts that, and says more about why.
         if (!found && zone->timesOut)
            return false;

         return true;
      }

      bool
      ConformanceLookup::GetTXTRecords(const AnsiString &domain, std::vector<AnsiString> &records)
      {
         return Lookup_(domain, RecordType::TXT, records);
      }

      bool
      ConformanceLookup::GetARecords(const AnsiString &host, std::vector<AnsiString> &addresses)
      {
         return Lookup_(host, RecordType::A, addresses);
      }

      bool
      ConformanceLookup::GetAAAARecords(const AnsiString &host, std::vector<AnsiString> &addresses)
      {
         return Lookup_(host, RecordType::AAAA, addresses);
      }

      bool
      ConformanceLookup::GetMXRecords(const AnsiString &domain, std::vector<AnsiString> &hostNames)
      {
         return Lookup_(domain, RecordType::MX, hostNames);
      }

      bool
      ConformanceLookup::GetPTRRecords(const AnsiString &reverseName, std::vector<AnsiString> &hostNames)
      {
         return Lookup_(reverseName, RecordType::PTR, hostNames);
      }
   }
}
