// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "PlusAddressing.h"
#include "../Common/BO/Domain.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PlusAddressing::PlusAddressing(void)
   {
   }

   PlusAddressing::~PlusAddressing(void)
   {
   }

   String 
   PlusAddressing::ExtractAccountAddress(const String &sAddress, std::shared_ptr<const Domain> pDomain) 
   {  
      if (!pDomain || !pDomain->GetUsePlusAddressing())
      {
         // Plus addressing is not enabled for the recipient domain.
         return sAddress;
      }

      // Plus addressing is enabled so we should check
      // if it has been utilized.
      String sPlusAddrChar = pDomain->GetPlusAddressingChar();

      // Extract the account address. Easier this way.
      String sAddrPart = StringParser::ExtractAddress(sAddress);

      // Locate the plus address char (if it exists)
      int iPlusCharPos = sAddrPart.Find(sPlusAddrChar);

      if (iPlusCharPos == -1)
      {  
         // Plus character wasn't used in the address.
         return sAddress;
      }

      // Copy the address from the start up until (but excluding)
      // the plus addressing character.
      String sAddressPart = sAddrPart.Mid(0, iPlusCharPos);

      String sDomainName = StringParser::ExtractDomain(sAddress);
      return sAddressPart + "@" + sDomainName;      
   }

}