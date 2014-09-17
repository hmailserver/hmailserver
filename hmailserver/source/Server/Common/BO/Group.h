// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class GroupMembers;
   class Groups;

   class Group : public BusinessObject<Group>
   {
   public:
      Group(void);
      ~Group(void);

      String GetName() const;
      void SetName(const String &sName);

      std::shared_ptr<GroupMembers> GetMembers();
      
      bool UserIsMember(__int64 iAccountID);

      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pParentNode, int iRestoreOptions);
      bool XMLLoadSubItems(XNode *pParentNode, int iRestoreOptions);

   private:

      String name_;
   };
}