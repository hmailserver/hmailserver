// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandCreate.h"
#include "IMAPConnection.h"
#include "IMAPSimpleCommandParser.h"
#include "IMAPConfiguration.h"
#include "IMAPFolderUtilities.h"

#include "../Common/BO/IMAPFolders.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/ACLPermission.h"
#include "../Common/Persistence/PersistentIMAPFolder.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPResult
   IMAPCommandCREATE::ExecuteCommand(std::shared_ptr<HM::IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

     
      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());

      pParser->Parse(pArgument);

      if (pParser->ParamCount() != 1 && pParser->ParamCount() != 2)
         return IMAPResult(IMAPResult::ResultBad, "CREATE Command requires 1 or 2 parameters.");

      // Fetch the name of the mailbox to create.
      String sFolderName = pParser->GetParamValue(pArgument, 0);

      if (sFolderName.IsEmpty())
         return IMAPResult(IMAPResult::ResultNo, "Folder name not specified.");

      // RFC 6154 CREATE ... (USE (\Attr ...)) - assign special-use attribute(s) on creation.
      String sSpecialUse;
      if (pParser->ParamCount() == 2)
      {
         String sUseParam = pParser->GetParamValue(pArgument, 1);

         if (sUseParam.Left(3).CompareNoCase(_T("USE")) != 0)
            return IMAPResult(IMAPResult::ResultBad, "CREATE Invalid create-param. Only USE is supported.");

         int iOpenParen = sUseParam.Find(_T("("));
         int iCloseParen = sUseParam.ReverseFind(_T(")"));

         if (iOpenParen < 0 || iCloseParen < 0 || iCloseParen < iOpenParen)
            return IMAPResult(IMAPResult::ResultBad, "CREATE Invalid USE create-param.");

         String sAttributes = sUseParam.Mid(iOpenParen + 1, iCloseParen - iOpenParen - 1);

         std::vector<String> vecAttributes = StringParser::SplitString(sAttributes, _T(" "));

         if (vecAttributes.empty())
            return IMAPResult(IMAPResult::ResultBad, "CREATE Invalid USE create-param.");

         for (const String &sAttribute : vecAttributes)
         {
            if (!IMAPFolder::IsValidSpecialUseAttribute(sAttribute))
               return IMAPResult(IMAPResult::ResultNo, "[BADSPECIALUSE] CREATE Unsupported special-use attribute.");
         }

         sSpecialUse = StringParser::JoinVector(vecAttributes, _T(" "));

         // RFC 6154: only one mailbox may have a given special-use attribute at a time.
         unsigned int newSpecialUseFlags = IMAPFolder::SpecialUseStringToFlags(sSpecialUse);
         if (pConnection->GetAccountFolders()->GetFolderWithSpecialUse(newSpecialUseFlags))
            return IMAPResult(IMAPResult::ResultNo, "[USEATTR] CREATE A mailbox with this special-use attribute already exists.");
      }

      // Check so that it does not already exist.
      std::shared_ptr<IMAPFolder> pExistsCheck = pConnection->GetFolderByFullPath(sFolderName);
      if (pExistsCheck)
         return IMAPResult(IMAPResult::ResultNo, "Folder already exists.");

      String hierarchyDelimiter = Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter();

      std::vector<String> vecFolderPath = StringParser::SplitString(sFolderName, hierarchyDelimiter);
   
      bool bIsPublicFolder = IMAPFolderUtilities::IsPublicFolder(vecFolderPath);
      if (bIsPublicFolder)
         vecFolderPath.erase(vecFolderPath.begin());

      if (!IMAPFolder::IsValidFolderName(vecFolderPath, bIsPublicFolder))
         return IMAPResult(IMAPResult::ResultNo, "CREATE The folder name is invalid.");

      if (bIsPublicFolder && !sSpecialUse.IsEmpty())
         return IMAPResult(IMAPResult::ResultNo, "CREATE USE is not supported for public folders.");

      IMAPResult result = ConfirmPossibleToCreate(pConnection, vecFolderPath, bIsPublicFolder);
      if (result.GetResult() != IMAPResult::ResultOK)
         return result;

      std::shared_ptr<IMAPFolders> pParentFolderContainer;
      if (!bIsPublicFolder)
         pParentFolderContainer = pConnection->GetAccountFolders();
      else
         pParentFolderContainer = pConnection->GetPublicFolders();

      bool bSubscribeToFolder = bIsPublicFolder;

      pParentFolderContainer->CreatePath(pParentFolderContainer, vecFolderPath, bSubscribeToFolder);

      if (!sSpecialUse.IsEmpty())
      {
         std::shared_ptr<IMAPFolder> pCreatedFolder = pParentFolderContainer->GetFolderByFullPath(vecFolderPath);
         if (pCreatedFolder)
         {
            pCreatedFolder->SetSpecialUse(sSpecialUse);
            PersistentIMAPFolder::SaveObject(pCreatedFolder);
         }
      }

      String sResponse = pArgument->Tag() + " OK CREATE Completed\r\n";

      pConnection->SendAsciiData(sResponse);   

      assert(pParentFolderContainer->GetCount() > 0);

      // Send a notification to everyone subscribing to this event.
      std::shared_ptr<IMAPFolder> firstFolder = pParentFolderContainer->GetItem(0);
   
      return IMAPResult();
   }

   IMAPResult
   IMAPCommandCREATE::ConfirmPossibleToCreate(std::shared_ptr<HM::IMAPConnection> pConnection, const std::vector<String> &vecNewPath, bool bIsPublicFolder)
   {
      if (bIsPublicFolder)
      {
         std::shared_ptr<IMAPFolders> pFolders = pConnection->GetPublicFolders();

         std::vector<String> vecTempPath = vecNewPath;
         vecTempPath.erase(vecTempPath.end()-1);
         std::shared_ptr<IMAPFolder> pParentFolder = IMAPFolderUtilities::GetTopMostExistingFolder(pFolders, vecTempPath);

         // Check if the user has permission to create a folder in the parent folder
         if (pParentFolder)
         {
            if (!pConnection->CheckPermission(pParentFolder, ACLPermission::PermissionCreate))
               return IMAPResult(IMAPResult::ResultNo, "ACL: Create permission denied (Required for CREATE command).");
         }     

         // Check if the user is trying to create a new root public folder, such as Public folders/Test
         if (bIsPublicFolder && !pParentFolder)
            return IMAPResult(IMAPResult::ResultNo, "ACL: Root public folders can only be created using administration tools.");
      }

      return IMAPResult();
   }
}