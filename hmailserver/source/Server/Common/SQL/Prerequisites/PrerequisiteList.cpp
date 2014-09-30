// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "PrerequisiteList.h"

#include "PreReqNoDuplicateFolders.h"
#include "IPrerequisite.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PrerequisiteList::PrerequisiteList(void)
   {
      Initialize_();
   }

   PrerequisiteList::~PrerequisiteList(void)
   {
   }

   void 
   PrerequisiteList::Initialize_()
   {
      std::shared_ptr<PreReqNoDuplicateFolders> duplicateFolderCheck = std::shared_ptr<PreReqNoDuplicateFolders>(new PreReqNoDuplicateFolders);
      prerequisites_.push_back(std::make_pair(duplicateFolderCheck->GetDatabaseVersion(), duplicateFolderCheck));
   }

   bool
   PrerequisiteList::Ensure(std::shared_ptr<DALConnection> connection, int scriptDatabaseVersion, String &sErrorMessage)
   {
      auto iter = prerequisites_.begin();
      auto iterEnd = prerequisites_.end();

      for (; iter != iterEnd; iter++)
      {
         int databaseVersion = (*iter).first;

         if (databaseVersion == scriptDatabaseVersion)
         {
            std::shared_ptr<IPrerequisite > prereq = (*iter).second;

            if (!prereq->Ensure(connection, sErrorMessage))
               return false;
         }
      }

      return true;
   }

   
}