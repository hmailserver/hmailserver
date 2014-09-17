// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Rule;
   enum PersistenceMode;

   class PersistentRule
   {
   public:
      PersistentRule(void);
      ~PersistentRule(void);

      static bool ReadObject(std::shared_ptr<Rule> pRule, const SQLCommand& sSQL);
      static bool ReadObject(std::shared_ptr<Rule> pRule, std::shared_ptr<DALRecordset> pRS);

      static bool SaveObject(std::shared_ptr<Rule> pRule, String &errorMessage, PersistenceMode mode);
      static bool SaveObject(std::shared_ptr<Rule> pRule);
      static bool DeleteObject(std::shared_ptr<Rule> pRule);

      static void DeleteByAccountID(__int64 ID);

   private:

      static void NotifyReload_(std::shared_ptr<Rule> pRule);
   };
}