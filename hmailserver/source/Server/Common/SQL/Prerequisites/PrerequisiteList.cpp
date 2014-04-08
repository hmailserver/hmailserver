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
      _Initialize();
   }

   PrerequisiteList::~PrerequisiteList(void)
   {
   }

   void 
   PrerequisiteList::_Initialize()
   {
      shared_ptr<PreReqNoDuplicateFolders> duplicateFolderCheck = shared_ptr<PreReqNoDuplicateFolders>(new PreReqNoDuplicateFolders);
      _prerequisites.push_back(std::make_pair(duplicateFolderCheck->GetDatabaseVersion(), duplicateFolderCheck));
   }

   bool
   PrerequisiteList::Ensure(shared_ptr<DALConnection> connection, int scriptDatabaseVersion, String &sErrorMessage)
   {
      vector<pair<int, shared_ptr<IPrerequisite > > >::iterator iter = _prerequisites.begin();
      vector<pair<int, shared_ptr<IPrerequisite > > >::iterator iterEnd = _prerequisites.end();

      for (; iter != iterEnd; iter++)
      {
         int databaseVersion = (*iter).first;

         if (databaseVersion == scriptDatabaseVersion)
         {
            shared_ptr<IPrerequisite > prereq = (*iter).second;

            if (!prereq->Ensure(connection, sErrorMessage))
               return false;
         }
      }

      return true;
   }

   
}