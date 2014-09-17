// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPFolderUtilities.h"

#include "../IMAP/IMAPConfiguration.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/IMAPFolders.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPFolderUtilities::IMAPFolderUtilities()
   {
      
   }

   IMAPFolderUtilities::~IMAPFolderUtilities()
   {

   }

   bool 
   IMAPFolderUtilities::IsPublicFolder(const std::vector<String> &vecFolderPath)
   {
      if (vecFolderPath.size() == 0)
         return false;

      String sPublicFolderName = Configuration::Instance()->GetIMAPConfiguration()->GetIMAPPublicFolderName();
      if (sPublicFolderName.CompareNoCase(vecFolderPath[0]) == 0)
         return true;
      else
         return false;
   }
   
   std::shared_ptr<IMAPFolder> 
   IMAPFolderUtilities::GetTopMostExistingFolder(std::shared_ptr<IMAPFolders> pContainer, const std::vector<String> &vecFolderPath)
   {
      std::vector<String> tempFolderPath = vecFolderPath;
      std::shared_ptr<IMAPFolder> pTempFolder = pContainer->GetFolderByFullPath(tempFolderPath);

      while (!pTempFolder && tempFolderPath.size() > 0)
      {
         tempFolderPath.erase(tempFolderPath.end() - 1);
         
         pTempFolder = pContainer->GetFolderByFullPath(tempFolderPath);   
      }

      return pTempFolder;
   }


}
