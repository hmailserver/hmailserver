// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "distributionlistrecipient.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DistributionListRecipient::DistributionListRecipient(void) :
      list_id_(0)
   {

   }

   DistributionListRecipient::~DistributionListRecipient(void)
   {
   }

   void
   DistributionListRecipient::SetAddress(const String & sAddress)
   {
      address_ = sAddress;
   }

   bool 
   DistributionListRecipient::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Recipient"));

      pNode->AppendAttr(_T("Name"), address_);

      return true;
   }

   bool 
   DistributionListRecipient::XMLLoad(XNode *pNode, int iOptions)
   {
      address_ = pNode->GetAttrValue(_T("Name"));

      return true;
   }
}