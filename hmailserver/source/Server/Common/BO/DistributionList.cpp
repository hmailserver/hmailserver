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
      domain_id_(false),
      require_auth_(false),
      list_mode_(LMPublic),
      enabled_(false)
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
      sListMode.Format(_T("%d"), list_mode_);

      pNode->AppendAttr(_T("Name"), address_);
      pNode->AppendAttr(_T("Active"), enabled_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("RequiresAuth"), require_auth_ ? _T("1") : _T("0"));
      pNode->AppendAttr(_T("RequiresAuthAddress"), require_address_);
      pNode->AppendAttr(_T("ListMode"), sListMode);

      return GetMembers()->XMLStore(pNode, iOptions);
   }

   bool
   DistributionList::XMLLoad(XNode *pNode, int iRestoreOptions)
   {
      address_ = pNode->GetAttrValue(_T("Name"));
      enabled_ = pNode->GetAttrValue(_T("Active")) == _T("1");
      require_auth_ = pNode->GetAttrValue(_T("RequiresAuth")) == _T("1");
      require_address_ = pNode->GetAttrValue(_T("RequiresAuthAddress"));
      list_mode_ = (ListMode) _ttoi(pNode->GetAttrValue(_T("ListMode")));

      return true;
   }

   bool
   DistributionList::XMLLoadSubItems(XNode *pNode, int iRestoreOptions)
   {
      std::shared_ptr<DistributionListRecipients> pDistListRecipients = GetMembers();
      return pDistListRecipients->XMLLoad(pNode, iRestoreOptions);
   }

   std::shared_ptr<DistributionListRecipients>
   DistributionList::GetMembers() const
   {
      std::shared_ptr<DistributionListRecipients> members = std::shared_ptr<DistributionListRecipients> (new DistributionListRecipients(id_)) ;
      members->Refresh();
      return members;
   }

   size_t
   DistributionList::GetEstimatedCachingSize()
   {
      return 1024;
   }

}
