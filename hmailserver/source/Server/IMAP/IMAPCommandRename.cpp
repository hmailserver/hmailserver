// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandRename.h"
#include "IMAPConnection.h"
#include "IMAPSimpleCommandParser.h"
#include "IMAPConfiguration.h"
#include "IMAPFolderUtilities.h"

#include "../Common/BO/Account.h"
#include "../Common/BO/ACLPermission.h"
#include "../Common/BO/IMAPFolders.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/Persistence/PersistentIMAPFolder.h"
#include "../Common/Util/FolderManipulationLock.h"
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPResult
   IMAPCommandRENAME::ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {

      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      
      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());

      pParser->Parse(pArgument);

      if (pParser->ParamCount() != 2)
         return IMAPResult(IMAPResult::ResultBad, "RENAME command requires 2 parameters.");

      // Fetch parameters
      String sOldFolderName = pParser->GetParamValue(pArgument, 0);
      String sNewFolderName = pParser->GetParamValue(pArgument, 1);
            
      std::shared_ptr<IMAPFolder> pFolderToRename = pConnection->GetFolderByFullPath(sOldFolderName);
      if (!pFolderToRename)
         return IMAPResult(IMAPResult::ResultBad, "Folder could not be found.");

      String hierarchyDelimiter = Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter();

      std::vector<String> vecOldPath = StringParser::SplitString(sOldFolderName, hierarchyDelimiter);
      std::vector<String> vecNewPath = StringParser::SplitString(sNewFolderName, hierarchyDelimiter);
      
      bool bSourceIsPublic = IMAPFolderUtilities::IsPublicFolder(vecOldPath);
      bool bDestinationIsPublic = IMAPFolderUtilities::IsPublicFolder(vecNewPath);

      IMAPResult result = ConfirmPossibleToRename(pConnection, pFolderToRename, vecOldPath, vecNewPath);
      if (result.GetResult() != IMAPResult::ResultOK)
         return result;

      // Get the old and new parent folders.
      std::shared_ptr<IMAPFolder> pOldParentFolder = GetParentFolder(pConnection, vecOldPath);
      std::shared_ptr<IMAPFolder> pNewParentFolder = GetParentFolder(pConnection, vecNewPath);

      if (vecNewPath.size() > 1 && !pNewParentFolder)
      {
         // we're renaming to a sub folder, but the new parent folder does not exist. We need
         // to create the new parent folder before we try to add the new folder to it.
         std::vector<String> vecNewFolderParent = vecNewPath;
         vecNewFolderParent.erase(vecNewFolderParent.end()-1);

         pConnection->GetAccountFolders()->CreatePath(pConnection->GetAccountFolders(), vecNewFolderParent, false);

         // fetch the newly created folder
         pNewParentFolder  = GetParentFolder(pConnection, vecNewPath);
      }

      // Create global for manipulating folders for this account.
      FolderManipulationLock oFolderLock ((int) pConnection->GetAccount()->GetID(), -1);
      oFolderLock.Lock();

      // Get the new folder name
      String sFolderName = vecNewPath[vecNewPath.size()-1];

      // We are at the last position. Get folder name here

      // 1) Remove from old parent.
      if (pOldParentFolder)
         pOldParentFolder->GetSubFolders()->RemoveFolder(pFolderToRename);
      else
      {
         if (bSourceIsPublic)
            pConnection->GetPublicFolders()->RemoveFolder(pFolderToRename);  
         else
            pConnection->GetAccountFolders()->RemoveFolder(pFolderToRename);
      }

      // 2a) Set the new namepNewParentFolder of the folder
      pFolderToRename->SetFolderName(sFolderName);

      // 2a) Add to new parent, and update all paths recursivly.
      if (pNewParentFolder)
      {
         pFolderToRename->SetParentFolderID(pNewParentFolder->GetID());
         pNewParentFolder->GetSubFolders()->AddItem(pFolderToRename);
      }
      else
      {
         // Make sure that the parent folder is set to nothing here.
         // it could be that it's set to something else at this stage,
         // since the folder have been moved from one location to
         // another...

         pFolderToRename->SetParentFolderID(-1);
         
         if (bDestinationIsPublic)
         {
            // We are not allowed to add new root folders.
            assert(0);
         }
         else
         {
            pConnection->GetAccountFolders()->AddItem(pFolderToRename);
         }
      }

      PersistentIMAPFolder::SaveObject(pFolderToRename);

      String sResponse = pArgument->Tag() + " OK Rename completed\r\n";

      pConnection->SendAsciiData(sResponse);   
 
      return IMAPResult();
   }
   
   std::shared_ptr<IMAPFolder> 
   IMAPCommandRENAME::GetParentFolder(std::shared_ptr<HM::IMAPConnection> pConnection, const std::vector<String> &vecFolderPath)
   {
      // Get the old parent folder
      if (vecFolderPath.size() < 1)
      {
         std::shared_ptr<IMAPFolder> pEmpty;
         return pEmpty;
      }
      
      // Remove everythin but the last...
      std::vector<String> vecParentFolder = vecFolderPath;
      vecParentFolder.resize(vecParentFolder.size() -1);

      std::shared_ptr<IMAPFolder> pParentFolder = pConnection->GetFolderByFullPath(vecParentFolder);

      return pParentFolder;
   }

   IMAPResult
   IMAPCommandRENAME::ConfirmPossibleToRename(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPFolder> pFolderToRename, const std::vector<String> &vecOldPath, const std::vector<String> &vecNewPath)
   {


      if (!pFolderToRename)
         return IMAPResult(IMAPResult::ResultNo, "Folder to rename not found.");

      if ((vecOldPath.size() == 1 && vecOldPath[0].CompareNoCase(_T("INBOX")) == 0) ||
         (vecNewPath.size() == 1 && vecNewPath[0].CompareNoCase(_T("INBOX")) == 0))
      {
         return IMAPResult(IMAPResult::ResultNo, "Cannot rename INBOX.");
      }

      bool bSourceIsPublic = IMAPFolderUtilities::IsPublicFolder(vecOldPath);
      bool bDestinationIsPublic = IMAPFolderUtilities::IsPublicFolder(vecNewPath);

      if ((bSourceIsPublic && !bDestinationIsPublic) ||
         (!bSourceIsPublic && bDestinationIsPublic))
      {
         return IMAPResult(IMAPResult::ResultNo, "RENAME: Cannot rename a public folder to local or vice versa.");
      }
      
      // The user is trying to rename to a root public folder, such as Public folders.Something
      // This isn't allowed at the moment, since permissions for this is undefined.
      if (bDestinationIsPublic && vecNewPath.size() == 2)
         return IMAPResult(IMAPResult::ResultNo, "Root public folders can only be created using administration tools.");

      // Check if the user has access to rename this folder
      if (!pConnection->CheckPermission(pFolderToRename, ACLPermission::PermissionDeleteMailbox))
         return IMAPResult(IMAPResult::ResultNo, "ACL DeleteMailbox permission denied (required for RENAME).");
         
      String hierarchyDelimiter = Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter();
      String sNewFolderName = StringParser::JoinVector(vecNewPath, hierarchyDelimiter);
      std::shared_ptr<IMAPFolder> pTargetFolder = pConnection->GetFolderByFullPath(sNewFolderName);
      if (pTargetFolder)
         return IMAPResult(IMAPResult::ResultNo, "Target folder already exist.");


      int iRecursion = 0;
      int iOldFolderDepth = pFolderToRename->GetFolderDepth(iRecursion);
      int iNewMaxFolderDepth = (int) (iOldFolderDepth + (vecNewPath.size()-1));
      
      if (iNewMaxFolderDepth > 25)
         return IMAPResult(IMAPResult::ResultNo, "To many sub-folders in structure.");
         
       if (!IMAPFolder::IsValidFolderName(vecNewPath, bDestinationIsPublic))
          return IMAPResult(IMAPResult::ResultNo, "The new folder name is invalid.");

       // Prevent user from moving parent folder into sub folder.
       // For instance, the user should not be able to move A.B into A.B.A.
       // or Folder.Sub1 into Folder.Sub1.Sub2. Because if the user did this, what
       // would happen to Folder.Sub1?
       String sOldFolderName = StringParser::JoinVector(vecOldPath, hierarchyDelimiter);

       if (sNewFolderName.FindNoCase(sOldFolderName + ".") == 0)
       {
          // The new path starts with the entire old path. The user is trying to move an existing
          // folder into its own sub folder. This is not allowed, for reason stated above.
          return IMAPResult(IMAPResult::ResultNo, "A folder cannot be moved into one of its subfolders.");
       }

       return IMAPResult();
   }
}