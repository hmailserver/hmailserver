// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "UIDValidityGeneratorTester.h"
#include "UIDValidityGenerator.h"
#include "../Util/VariantDateTime.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   void
   UIDValidityGeneratorTester::Check_(bool condition, const AnsiString &description, __int64 actual)
   {
      if (!condition)
         failures_.push_back(Formatter::FormatAsAnsi("{0} (got {1})", description, actual));
   }

   std::vector<AnsiString>
   UIDValidityGeneratorTester::Run()
   {
      failures_.clear();

      // No real account has a negative ID.
      const __int64 account = -1001;
      const __int64 otherAccount = -1002;

      UIDValidityGenerator::DeleteAccount(account);
      UIDValidityGenerator::DeleteAccount(otherAccount);

      __int64 now = DateTime::GetCurrentTime().ToInt();

      // A new counter starts after the current time, above any creation-time UIDVALIDITY.
      __int64 first = UIDValidityGenerator::GetNext(account);
      Check_(first > now, "First value is after the current time", first);

      __int64 second = UIDValidityGenerator::GetNext(account);
      Check_(second == first + 1, "Values increase by one", second);

      // Another account's counter is separate.
      UIDValidityGenerator::GetNext(otherAccount);
      UIDValidityGenerator::GetNext(otherAccount);

      __int64 third = UIDValidityGenerator::GetNext(account);
      Check_(third == second + 1, "Other accounts don't move the counter", third);

      // Without a counter, it starts over from the current time.
      UIDValidityGenerator::DeleteAccount(account);
      __int64 afterDelete = UIDValidityGenerator::GetNext(account);
      Check_(afterDelete > now, "A deleted counter starts after the current time", afterDelete);

      UIDValidityGenerator::DeleteAccount(account);
      UIDValidityGenerator::DeleteAccount(otherAccount);

      return failures_;
   }
}
