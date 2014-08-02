// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DomainAlias : public BusinessObject<DomainAlias>
   {
   public:
      DomainAlias(void);
      ~DomainAlias(void);

      String GetName() const {return alias_; }

      __int64 GetDomainID() const {return domain_id_; }
      void SetDomainID(__int64 newval) {domain_id_ = newval; }

      String GetAlias() const {return alias_; }
      void SetAlias(const String &newval) {alias_ = newval; }

      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pAliasNode, int iOptions);
      bool XMLLoadSubItems(XNode *pAliasNode, int iOptions) {return true;};

   private:
      
      __int64 domain_id_;
      String alias_;
   };
}