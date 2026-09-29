// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The per-account UIDVALIDITY counters. Needs the database.
   class UIDValidityGeneratorTester
   {
   public:

      std::vector<AnsiString> Run();

   private:

      void Check_(bool condition, const AnsiString &description, __int64 actual);

      std::vector<AnsiString> failures_;
   };
}
