// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "distributionlist.h"

#include "DistributionListRecipients.h"

#include "../Persistence/PersistentDistributionListRecipient.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DistributionList::DistributionList(void) :
      m_iDomainID(false),
      m_bRequireAuth(false),
      m_eListMode(LMPublic)

   {
      
   }

   DistributionList::~DistributionList(void)
   {
   }

   bool 
   DistributionList::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("DistributionList"));

      String sListMode;
      sListMode.Format(_T("%d"), m_eListMode);

      pNode->AppendAttr(_T("Name"), m_sAddress);
      pNode->AppendAttr(_T("Active"), m_bEnabled ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("RequiresAuth"), m_bRequireAuth ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("RequiresAuthAddress"), m_sRequireAddress);
      pNode->AppendAttr(_T("ListMode"), sListMode);

      return GetMembers()->XMLStore(pNode, iOptions);
   }

   bool
   DistributionList::XMLLoad(XNode *pNode, int iRestoreOptions)
   {
      m_sAddress = pNode->GetAttrValue(_T("Name"));
      m_bEnabled = pNode->GetAttrValue(_T("Active")) == _T("1");
      m_bRequireAuth = pNode->GetAttrValue(_T("RequiresAuth")) == _T("1");
      m_sRequireAddress = pNode->GetAttrValue(_T("RequiresAuthAddress"));
      m_eListMode = (ListMode) _ttoi(pNode->GetAttrValue(_T("ListMode")));

      return true;
   }

   bool
   DistributionList::XMLLoadSubItems(XNode *pNode, int iRestoreOptions)
   {
      shared_ptr<DistributionListRecipients> pDistListRecipients = GetMembers();
      return pDistListRecipients->XMLLoad(pNode, iRestoreOptions);
   }

   shared_ptr<DistributionListRecipients>
   DistributionList::GetMembers() const
   {
      shared_ptr<DistributionListRecipients> members = shared_ptr<DistributionListRecipients> (new DistributionListRecipients(m_iID)) ;
      members->Refresh();
      return members;
   }
}
