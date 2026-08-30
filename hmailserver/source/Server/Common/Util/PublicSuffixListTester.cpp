// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "PublicSuffixListTester.h"

#include "PublicSuffixList.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   void
   PublicSuffixListTester::Test()
   {
      TestRegistrableDomain_();
      TestDomainsWithoutRegistrableDomain_();
   }

   void
   PublicSuffixListTester::TestRegistrableDomain_()
   {
      PublicSuffixList *list = PublicSuffixList::Instance();

      if (list->GetRegistrableDomain(_T("example.com")) != _T("example.com") ||
          list->GetRegistrableDomain(_T("EXAMPLE.COM")) != _T("example.com") ||
          list->GetRegistrableDomain(_T("www.mail.example.com")) != _T("example.com") ||
          list->GetRegistrableDomain(_T("example.com.")) != _T("example.com"))
      {
         assert(0);
         throw;
      }

      // Suffixes consisting of several labels.
      if (list->GetRegistrableDomain(_T("example.co.uk")) != _T("example.co.uk") ||
          list->GetRegistrableDomain(_T("a.b.example.co.uk")) != _T("example.co.uk") ||
          list->GetRegistrableDomain(_T("evil.github.io")) != _T("evil.github.io") ||
          list->GetRegistrableDomain(_T("a.b.evil.github.io")) != _T("evil.github.io"))
      {
         assert(0);
         throw;
      }

      // Wildcard rule (*.kawasaki.jp) and the exception to it (!city.kawasaki.jp).
      if (list->GetRegistrableDomain(_T("a.b.kawasaki.jp")) != _T("a.b.kawasaki.jp") ||
          list->GetRegistrableDomain(_T("x.a.b.kawasaki.jp")) != _T("a.b.kawasaki.jp") ||
          list->GetRegistrableDomain(_T("www.city.kawasaki.jp")) != _T("city.kawasaki.jp"))
      {
         assert(0);
         throw;
      }

      // Unknown top level domains fall back to the default rule.
      if (list->GetRegistrableDomain(_T("example.nonexistingtld")) != _T("example.nonexistingtld") ||
          list->GetRegistrableDomain(_T("a.b.example.nonexistingtld")) != _T("example.nonexistingtld"))
      {
         assert(0);
         throw;
      }
   }

   void
   PublicSuffixListTester::TestDomainsWithoutRegistrableDomain_()
   {
      PublicSuffixList *list = PublicSuffixList::Instance();

      String registrableDomain;

      // A public suffix in itself, or a single label, isn't a registrable domain.
      if (list->GetRegistrableDomain(_T("com"), registrableDomain) ||
          list->GetRegistrableDomain(_T("co.uk"), registrableDomain) ||
          list->GetRegistrableDomain(_T("github.io"), registrableDomain) ||
          list->GetRegistrableDomain(_T("localhost"), registrableDomain) ||
          list->GetRegistrableDomain(_T(""), registrableDomain))
      {
         assert(0);
         throw;
      }

      // These domains are returned as-is by the other overload.
      if (list->GetRegistrableDomain(_T("co.uk")) != _T("co.uk") ||
          list->GetRegistrableDomain(_T("localhost")) != _T("localhost"))
      {
         assert(0);
         throw;
      }

      if (!list->GetRegistrableDomain(_T("www.example.co.uk"), registrableDomain) ||
          registrableDomain != _T("example.co.uk"))
      {
         assert(0);
         throw;
      }
   }
}
