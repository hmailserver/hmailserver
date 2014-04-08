// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Cache/Cacheable.h"

namespace HM
{

   class DistributionListRecipients;
   
   class DistributionList : public Cacheable, public BusinessObject<DistributionList>
   {
   public:
      DistributionList(void);
      ~DistributionList(void);

      enum ListMode
      {
         LMPublic = 0,
         LMMembership = 1,
         LMAnnouncement = 2
      };

      String GetName() {return m_sAddress; }

      __int64 GetDomainID() const {return m_iDomainID;}
      void SetDomainID(__int64 newVal) {m_iDomainID = newVal;}

      String GetAddress() const {return m_sAddress;}
      void SetAddress(const String & sNewVal) {m_sAddress = sNewVal;}

      bool GetActive() const {return m_bEnabled;}
      void SetActive(bool bNewVal) {m_bEnabled = bNewVal;}

      bool GetRequireAuth() const {return m_bRequireAuth;}
      void SetRequireAuth(bool bNewVal) {m_bRequireAuth = bNewVal;}

      String GetRequireAddress() const {return m_sRequireAddress;}
      void SetRequireAddress(const String & sNewVal) {m_sRequireAddress = sNewVal;}

      ListMode GetListMode() const {return m_eListMode; }
      void SetListMode(ListMode m) {m_eListMode = m; }

      bool XMLStore(XNode *pParentNode, int iOptions);
      bool XMLLoad(XNode *pParentNode, int iRestoreOptions);
      bool XMLLoadSubItems(XNode *pParentNode, int iRestoreOptions);

      shared_ptr<DistributionListRecipients> GetMembers() const;

   protected:

      String m_sAddress;
      __int64 m_iDomainID;
      bool m_bEnabled;

      bool m_bRequireAuth;
      String m_sRequireAddress;

      ListMode m_eListMode;
   };
}
