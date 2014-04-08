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

      String GetName() const {return m_sAlias; }

      __int64 GetDomainID() const {return m_iDomainID; }
      void SetDomainID(__int64 newval) {m_iDomainID = newval; }

      String GetAlias() const {return m_sAlias; }
      void SetAlias(const String &newval) {m_sAlias = newval; }

      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pAliasNode, int iOptions);
      bool XMLLoadSubItems(XNode *pAliasNode, int iOptions) {return true;};

   private:
      
      __int64 m_iDomainID;
      String m_sAlias;
   };
}