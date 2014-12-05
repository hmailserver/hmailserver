// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "TLD.h"

#include "Utilities.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   TLD::TLD(void)
   {
      
   }

   TLD::~TLD(void)
   {
   }

   void 
   TLD::Initialize()
   {
      tld_.clear();

      String sTLDSFile = Utilities::GetBinDirectory();
      if (sTLDSFile.Right(1) != _T("\\"))
         sTLDSFile += "\\";
      sTLDSFile += "tlds.txt";

#ifdef _DEBUG
      if (!FileUtilities::Exists(sTLDSFile))
      {
         // We're running in debug. Since the tlds.txt
         // is not copied to the Source\hMailServer\Debug directory, 
         // we try to find it on a hard coded place.

         sTLDSFile = "C:\\Program Files (x86)\\hMailServer\\Bin\\tlds.txt";
      }
#endif

      String sContents = FileUtilities::ReadCompleteTextFile(sTLDSFile);

      if (sContents.IsEmpty())
      {
         // Failed to load file containing TLD's. Report an error.
         String sErrorMessage;
         sErrorMessage.Format(_T("Failed to load file with TLD's %s."), sTLDSFile.c_str());
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 4335, "TLD::_LoadCCTLD", sErrorMessage);

         return;
      }

      std::vector<String> vecTLDs = StringParser::SplitString(sContents, "\r\n");
      for (auto iter = vecTLDs.begin(); iter != vecTLDs.end(); iter++)
         tld_.insert(*iter);

   }

   bool 
   TLD::IsTLD(const String &sName)
   {
      String sTmp = sName;

      sTmp.ToLower();

      if (tld_.find(sName) != tld_.end())
         return true;
      else
         return false;
   }

   bool 
   TLD::GetDomainNameFromHost(String &sHost, bool &bIsIPAddress)
   {
      bIsIPAddress = false;

      // Start at the end.
      std::vector<String> vecParts = StringParser::SplitString(sHost, ".");

      size_t iParts = vecParts.size();

      if (iParts < 2)
         return false;

      // Check if it's an IP address
      if (iParts == 4 && StringParser::IsValidIPAddress(sHost))
      {
         sHost = vecParts[3] + "." + vecParts[2] + "." + vecParts[1] + "." + vecParts[0];
         bIsIPAddress = true;
         return true;
      }

      // It's not an IP address.
      String sTwoLast = vecParts[iParts-2] + "." + vecParts[iParts-1];
      sTwoLast.ToLower();

      if (!IsTLD(sTwoLast))
         sHost = sTwoLast;
      else
      {
         if (iParts == 2)
            return false;

         sHost = vecParts[iParts-3] + "." + vecParts[iParts-2] + "." + vecParts[iParts-1];
      }

      return true;      
   }

}