// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class GroupMember : public BusinessObject<GroupMember>
   {
   public:
      GroupMember(void);
      ~GroupMember(void);

      String GetName();

      __int64 GetGroupID();
      void SetGroupID(__int64 iNewVal);

      __int64 GetAccountID();
      void SetAccountID(__int64 iNewVal);

      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pParentNode, int iOptions);
      bool XMLLoadSubItems(XNode *pParentNode, int iOptions) {return true;};

   private:

      __int64 group_id_;
      __int64 account_id_;
   };
}