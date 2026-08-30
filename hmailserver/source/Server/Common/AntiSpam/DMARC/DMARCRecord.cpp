// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DMARCRecord.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DMARCRecord::DMARCRecord() :
      policy_(Policy::None),
      subdomain_policy_(Policy::None),
      has_subdomain_policy_(false),
      dkim_alignment_(Alignment::Relaxed),
      spf_alignment_(Alignment::Relaxed),
      percent_(100)
   {

   }

   bool
   DMARCRecord::HasVersionTag(const AnsiString &textRecord)
   {
      std::vector<AnsiString> tags = StringParser::SplitString(textRecord, ";");

      if (tags.size() == 0)
         return false;

      AnsiString versionTag = tags[0];

      // Whitespace is permitted around the equals sign. The value is case-sensitive.
      versionTag.Trim();
      versionTag.Replace(" ", "");
      versionTag.Replace("\t", "");

      return versionTag == "v=DMARC1";
   }

   bool
   DMARCRecord::Parse(const AnsiString &textRecord, DMARCRecord &record)
   {
      if (!HasVersionTag(textRecord))
         return false;

      record = DMARCRecord();

      std::map<AnsiString, AnsiString> tags;

      std::vector<AnsiString> tagList = StringParser::SplitString(textRecord, ";");

      for (size_t i = 1; i < tagList.size(); i++)
      {
         AnsiString tag = tagList[i];

         int equalsPosition = tag.Find("=");
         if (equalsPosition < 0)
            continue;

         AnsiString name = tag.Mid(0, equalsPosition);
         AnsiString value = tag.Mid(equalsPosition + 1);

         name.Trim();
         value.Trim();

         // Tag names are case-insensitive.
         name.ToLower();

         tags[name] = value;
      }

      // The policy is required. A record without one isn't a policy record.
      auto policyTag = tags.find("p");
      if (policyTag == tags.end())
         return false;

      if (!ParsePolicy_(policyTag->second, record.policy_))
         return false;

      auto subdomainPolicyTag = tags.find("sp");
      if (subdomainPolicyTag != tags.end())
         record.has_subdomain_policy_ = ParsePolicy_(subdomainPolicyTag->second, record.subdomain_policy_);

      auto dkimAlignmentTag = tags.find("adkim");
      if (dkimAlignmentTag != tags.end())
         record.dkim_alignment_ = ParseAlignment_(dkimAlignmentTag->second);

      auto spfAlignmentTag = tags.find("aspf");
      if (spfAlignmentTag != tags.end())
         record.spf_alignment_ = ParseAlignment_(spfAlignmentTag->second);

      auto percentTag = tags.find("pct");
      if (percentTag != tags.end())
      {
         int percent = 0;

         if (StringParser::TryParseInt(percentTag->second, percent) &&
             percent >= 0 && percent <= 100)
         {
            record.percent_ = percent;
         }
      }

      return true;
   }

   bool
   DMARCRecord::ParsePolicy_(const AnsiString &value, Policy &policy)
   {
      AnsiString policyValue = value;
      policyValue.ToLower();

      if (policyValue == "none")
         policy = Policy::None;
      else if (policyValue == "quarantine")
         policy = Policy::Quarantine;
      else if (policyValue == "reject")
         policy = Policy::Reject;
      else
         return false;

      return true;
   }

   DMARCRecord::Alignment
   DMARCRecord::ParseAlignment_(const AnsiString &value)
   {
      AnsiString alignmentValue = value;
      alignmentValue.ToLower();

      // Anything other than strict means relaxed, which is the default.
      return alignmentValue == "s" ? Alignment::Strict : Alignment::Relaxed;
   }
}
