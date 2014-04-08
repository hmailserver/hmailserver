// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Rule;

   class PersistentRule
   {
   public:
      PersistentRule(void);
      ~PersistentRule(void);

      static bool ReadObject(shared_ptr<Rule> pRule, const SQLCommand& sSQL);
      static bool ReadObject(shared_ptr<Rule> pRule, shared_ptr<DALRecordset> pRS);

      static bool SaveObject(shared_ptr<Rule> pRule, String &errorMessage);
      static bool SaveObject(shared_ptr<Rule> pRule);
      static bool DeleteObject(shared_ptr<Rule> pRule);

      static void DeleteByAccountID(__int64 ID);

   private:

      static void _NotifyReload(shared_ptr<Rule> pRule);
   };
}