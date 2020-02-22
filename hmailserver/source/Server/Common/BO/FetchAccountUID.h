// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DateTime;

   class FetchAccountUID : public BusinessObject<FetchAccountUID>
   {
   public:
      FetchAccountUID (__int64 iID, __int64 iAccountID, const String &sUID, const String &sDate);
      FetchAccountUID();

	  ~FetchAccountUID();

     String GetName() const { return "FetchAccountUID"; }

      void SetAccountID(__int64 accountID);

      const String &GetUID() const {return uid_; }
      DateTime GetCreationDate() const;
      __int64 GetAccountID() const {return account_id_;}

      bool XMLStore(XNode *pUIDsNode, int iBackupOptions);
      bool XMLLoad(XNode *pUIDNode, int iRestoreOptions);
      bool XMLLoadSubItems(XNode *pUIDNode, int iRestoreOptions) {return true; } 

   private:
      __int64 account_id_;
      String uid_;
      String date_;
   };
}