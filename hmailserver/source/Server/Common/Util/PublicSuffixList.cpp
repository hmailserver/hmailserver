// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "PublicSuffixList.h"

#include "Utilities.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      const String FileName = _T("public_suffix_list.dat");

      bool IsAscii(const String &value)
      {
         for (TCHAR character : value)
         {
            if (character > 127)
               return false;
         }

         return true;
      }
   }

   void
   PublicSuffixList::Initialize()
   {
      rules_.clear();
      wildcard_rules_.clear();
      exception_rules_.clear();

      String fileName = Utilities::GetBinDirectory();
      if (fileName.Right(1) != _T("\\"))
         fileName += _T("\\");
      fileName += FileName;

#ifdef _DEBUG
      if (!FileUtilities::Exists(fileName))
      {
         // We're running in debug. The file isn't copied to the build output
         // directory, so we try to find it where it's normally installed.
         fileName = _T("C:\\Program Files\\hMailServer\\Bin\\") + FileName;
      }
#endif

      String contents = FileUtilities::ReadCompleteTextFile(fileName);

      if (contents.IsEmpty())
      {
         String errorMessage;
         errorMessage.Format(_T("Failed to load the public suffix list %s."), fileName.c_str());
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4337, "PublicSuffixList::Initialize", errorMessage);

         return;
      }

      std::vector<String> lines = StringParser::SplitString(contents, _T("\n"));

      for (String line : lines)
      {
         line.Trim();
         line.ToLower();

         if (line.IsEmpty() || line.StartsWith(_T("//")))
            continue;

         // Rules containing non-ASCII characters are stored in unicode form,
         // while domain names are handled in punycode form. They never match.
         if (!IsAscii(line))
            continue;

         if (line.StartsWith(_T("!")))
            exception_rules_.insert(line.Mid(1));
         else if (line.StartsWith(_T("*.")))
            wildcard_rules_.insert(line.Mid(2));
         else
            rules_.insert(line);
      }
   }

   String
   PublicSuffixList::GetRegistrableDomain(const String &domain) const
   {
      String result = domain;
      result.ToLower();

      while (result.EndsWith(_T(".")))
         result = result.Mid(0, result.GetLength() - 1);

      std::vector<String> labels = StringParser::SplitString(result, _T("."));

      if (labels.size() < 2)
         return result;

      size_t suffixLabelCount = GetPublicSuffixLabelCount_(labels);

      // The domain is a public suffix in itself, so there's no domain to return.
      if (suffixLabelCount >= labels.size())
         return result;

      return JoinLabels_(labels, labels.size() - suffixLabelCount - 1);
   }

   size_t
   PublicSuffixList::GetPublicSuffixLabelCount_(const std::vector<String> &labels) const
   {
      size_t labelCount = labels.size();

      // An exception rule takes precedence over all other rules. It makes the
      // public suffix one label shorter than the rule itself.
      for (size_t i = 0; i < labelCount; i++)
      {
         if (exception_rules_.find(JoinLabels_(labels, i)) != exception_rules_.end())
            return labelCount - i - 1;
      }

      // Otherwise the rule matching the largest number of labels applies.
      for (size_t i = 0; i < labelCount; i++)
      {
         if (rules_.find(JoinLabels_(labels, i)) != rules_.end())
            return labelCount - i;

         // A wildcard rule matches exactly one label to the left of the rule.
         if (i + 1 < labelCount &&
             wildcard_rules_.find(JoinLabels_(labels, i + 1)) != wildcard_rules_.end())
            return labelCount - i;
      }

      // No rule matched, which means the default rule (*) applies.
      return 1;
   }

   String
   PublicSuffixList::JoinLabels_(const std::vector<String> &labels, size_t firstLabel)
   {
      String result;

      for (size_t i = firstLabel; i < labels.size(); i++)
      {
         if (i > firstLabel)
            result += _T(".");

         result += labels[i];
      }

      return result;
   }
}
