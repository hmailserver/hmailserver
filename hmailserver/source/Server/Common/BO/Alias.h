// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Cache/Cacheable.h"

namespace HM
{
   class Alias : public Cacheable, public BusinessObject<Alias>
   {
   public:
	   Alias();
	   virtual ~Alias();

      __int64 GetDomainID() const { return m_iDomainID; }
      String GetName() const { return m_sName; }
      String GetValue() const { return m_sValue; }
      bool GetIsActive() const { return m_bActive; }

      void SetDomainID(__int64 newVal) { m_iDomainID = newVal; }
      void SetName(const String & newVal) {  m_sName = newVal; }
      void SetValue(const String & newVal) { m_sValue = newVal; }
      void SetIsActive(bool newVal) { m_bActive = newVal; }

      bool XMLStore(XNode *pParentNode, int iOptions);     
      bool XMLLoad(XNode *pNode, int iOptions);
      bool XMLLoadSubItems(XNode *pNode, int iOptions) {return true; }

   protected:

      __int64 m_iDomainID;
      String m_sName;
      String m_sValue;
      bool m_bActive;


   };
}
