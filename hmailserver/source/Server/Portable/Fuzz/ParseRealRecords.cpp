// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

// Parses every record in a file written by Get-SpfRecords.ps1 and reports the ones
// the parser rejects, grouped by why. Optionally writes each distinct record to a
// directory, as seed input for fuzz-spf-record. README.md says how to run it.

#include "stdafx.h"

#include "../../SMTP/SPF/SPFRecord.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace
{
   // Get-SpfRecords.ps1 escapes \, tab, CR and LF so that a record fits on one line.
   std::string Unescape(const std::string &text)
   {
      std::string result;

      for (size_t i = 0; i < text.size(); i++)
      {
         if (text[i] != '\\' || i + 1 == text.size())
         {
            result += text[i];
            continue;
         }

         char next = text[++i];

         switch (next)
         {
         case 't': result += '\t'; break;
         case 'r': result += '\r'; break;
         case 'n': result += '\n'; break;
         default: result += next; break;
         }
      }

      return result;
   }

   // "Not a valid domain: foo" and "Not a valid domain: bar" are one group.
   std::string GroupOf(const std::string &error)
   {
      size_t colon = error.find(": ");
      return colon == std::string::npos ? error : error.substr(0, colon);
   }

   // FNV-1a, so that a record always gets the same corpus file name.
   std::string HashName(const std::string &text)
   {
      unsigned long long hash = 14695981039346656037ULL;

      for (unsigned char c : text)
      {
         hash ^= c;
         hash *= 1099511628211ULL;
      }

      char name[17];
      snprintf(name, sizeof(name), "%016llx", hash);
      return name;
   }

   struct Rejection
   {
      std::string domain;
      std::string record;
      std::string error;
   };
}

int main(int argc, char *argv[])
{
   if (argc < 2 || argc > 3)
   {
      fprintf(stderr, "Usage: %s <records.tsv> [corpus directory]\n", argv[0]);
      return 2;
   }

   std::ifstream input(argv[1], std::ios::binary);

   if (!input)
   {
      fprintf(stderr, "Cannot open %s\n", argv[1]);
      return 2;
   }

   const char *corpusDirectory = argc == 3 ? argv[2] : nullptr;

   int parsed = 0;
   int accepted = 0;
   int notSpf = 0;
   std::set<std::string> distinct;
   std::map<std::string, std::vector<Rejection>> rejections;

   std::string line;

   while (std::getline(input, line))
   {
      if (!line.empty() && line.back() == '\r')
         line.pop_back();

      size_t firstTab = line.find('\t');
      size_t secondTab = firstTab == std::string::npos ? std::string::npos : line.find('\t', firstTab + 1);

      if (secondTab == std::string::npos)
         continue;

      std::string status = line.substr(firstTab + 1, secondTab - firstTab - 1);

      if (status != "ok" && status != "multiple")
         continue;

      std::string domain = line.substr(0, firstTab);
      std::string record = Unescape(line.substr(secondTab + 1));

      parsed++;

      if (distinct.insert(record).second && corpusDirectory)
      {
         std::string path = std::string(corpusDirectory) + "/" + HashName(record);
         std::ofstream file(path, std::ios::binary);
         file.write(record.data(), record.size());
      }

      HM::AnsiString text(record.data(), (int) record.size());

      // Section 4.5 discards these, so they are no record rather than a rejected one.
      if (!HM::SPFRecord::HasVersionTag(text))
      {
         notSpf++;
         continue;
      }

      HM::SPFRecord result;
      HM::AnsiString error;

      if (HM::SPFRecord::Parse(text, result, error))
         accepted++;
      else
         rejections[GroupOf(error.c_str())].push_back({domain, record, error.c_str()});
   }

   std::vector<std::pair<std::string, std::vector<Rejection>>> groups(rejections.begin(), rejections.end());
   std::sort(groups.begin(), groups.end(),
      [](const auto &left, const auto &right) { return left.second.size() > right.second.size(); });

   printf("%d records, %zu distinct. %d accepted, %d rejected, %d not starting with the version.\n\n",
          parsed, distinct.size(), accepted, parsed - accepted - notSpf, notSpf);

   for (const auto &group : groups)
   {
      printf("%6zu  %s\n", group.second.size(), group.first.c_str());

      std::set<std::string> shown;

      for (const Rejection &rejection : group.second)
      {
         if (!shown.insert(rejection.record).second)
            continue;

         printf("        %s\n          %s\n          %s\n",
                rejection.domain.c_str(), rejection.record.c_str(), rejection.error.c_str());
      }

      printf("\n");
   }

   return 0;
}
