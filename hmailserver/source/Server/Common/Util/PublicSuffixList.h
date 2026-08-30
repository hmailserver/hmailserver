// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The public suffix list contains the domains below which anyone can register
   // a name, such as com, co.uk and github.io.
   class PublicSuffixList : public Singleton<PublicSuffixList>
   {
   public:

      void Initialize();

      String GetRegistrableDomain(const String &domain) const;

   private:

      size_t GetPublicSuffixLabelCount_(const std::vector<String> &labels) const;

      static String JoinLabels_(const std::vector<String> &labels, size_t firstLabel);

      std::set<String> rules_;
      std::set<String> wildcard_rules_;
      std::set<String> exception_rules_;
   };
}
