// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPConfiguration.h"

#include "../Common/Application/Configuration.h"
#include "../common/Application/ObjectCache.h"
#include "../Common/BO/IMAPFolders.h"
#include "../Common/BO/Groups.h"

#include "../Common/Persistence/PersistentIMAPFolder.h"
#include "../Common/Persistence/PersistentRuleAction.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   IMAPConfiguration::IMAPConfiguration()
   {

   }

   IMAPConfiguration::~IMAPConfiguration()
   {

   }

   bool
   IMAPConfiguration::Load()
   {
      // Shared public have their AccountID set to zero.
      public_folders_ = std::shared_ptr<IMAPFolders>(new IMAPFolders(0, -1));
      public_folders_->Refresh();

      // Shared public have their AccountID set to zero.
      groups_ = std::shared_ptr<Groups>(new Groups());
      groups_->Refresh();

      return true;
   }

   std::shared_ptr<PropertySet>
   IMAPConfiguration::GetSettings_() const
   {
      return Configuration::Instance()->GetSettings();
   }

   String 
   IMAPConfiguration::GetWelcomeMessage() const
   {
      return GetSettings_()->GetString(PROPERTY_WELCOMEIMAP);
   }
   
   void 
   IMAPConfiguration::SetWelcomeMessage(const String &sMessage)
   {
      GetSettings_()->SetString(PROPERTY_WELCOMEIMAP, sMessage);
   }

   bool
   IMAPConfiguration::GetUseIMAPQuota() const
   {
      return GetSettings_()->GetBool(PROPERTY_ENABLEIMAPQUOTA);
   }

   void 
   IMAPConfiguration::SetUseIMAPQuota(bool bValue)
   {
      GetSettings_()->SetBool(PROPERTY_ENABLEIMAPQUOTA, bValue);
   }

   bool 
   IMAPConfiguration::GetUseIMAPIdle() const
   {
      return GetSettings_()->GetBool(PROPERTY_ENABLEIMAPIDLE);
   }

   void 
   IMAPConfiguration::SetUseIMAPIdle(bool bValue)
   {
      GetSettings_()->SetBool(PROPERTY_ENABLEIMAPIDLE, bValue);
   }

   bool 
   IMAPConfiguration::GetUseIMAPACL() const
   {
      return GetSettings_()->GetBool(PROPERTY_ENABLEIMAPACL);
   }

   void 
   IMAPConfiguration::SetUseIMAPACL(bool bValue)
   {
      GetSettings_()->SetBool(PROPERTY_ENABLEIMAPACL, bValue);
   }

   bool 
   IMAPConfiguration::GetUseIMAPSort() const
   {
      return GetSettings_()->GetBool(PROPERTY_ENABLEIMAPSORT);
   }

   void 
   IMAPConfiguration::SetUseIMAPSort(bool bValue)
   {
      GetSettings_()->SetBool(PROPERTY_ENABLEIMAPSORT, bValue);
   }

   void
   IMAPConfiguration::SetMaxIMAPConnections(int newVal)
   {
      GetSettings_()->SetLong(PROPERTY_MAXIMAPCONNECTIONS, newVal);
   }

   long
   IMAPConfiguration::GetMaxIMAPConnections() const
   {
      return GetSettings_()->GetLong(PROPERTY_MAXIMAPCONNECTIONS);
   }

   void
   IMAPConfiguration::SetIMAPPublicFolderName(const String& newVal)
   {
      GetSettings_()->SetString(PROPERTY_IMAPPUBLICFOLDERNAME, newVal);
   }

   String
   IMAPConfiguration::GetIMAPPublicFolderName() const
   {
      return GetSettings_()->GetString(PROPERTY_IMAPPUBLICFOLDERNAME);
   }

   String
   IMAPConfiguration::GetIMAPMasterUser() const
   {
      return GetSettings_()->GetString(PROPERTY_IMAPMASTERUSER);
   }

   void
   IMAPConfiguration::SetIMAPMasterUser(const String &newVal)
   {
      GetSettings_()->SetString(PROPERTY_IMAPMASTERUSER, newVal);
   }

   bool
   IMAPConfiguration::GetUseIMAPSASLPlain() const
   {
      return GetSettings_()->GetBool(PROPERTY_ENABLEIMAPSASLPLAIN);
   }

   void
   IMAPConfiguration::SetUseIMAPSASLPlain(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_ENABLEIMAPSASLPLAIN, newVal);
   }

   bool
   IMAPConfiguration::GetUseIMAPSASLInitialResponse() const
   {
      return GetSettings_()->GetBool(PROPERTY_ENABLEIMAPSASLINTIALRESPONSE);
   }

   void
   IMAPConfiguration::SetUseIMAPSASLInitialResponse(bool newVal)
   {
      GetSettings_()->SetBool(PROPERTY_ENABLEIMAPSASLINTIALRESPONSE, newVal);
   }

   bool
   IMAPConfiguration::SetHierarchyDelimiter(const String& newVal)
   {
      if (GetHierarchyDelimiter() == newVal)
         return true;

      // Check that no IMAP folder contains this character.
      if (PersistentIMAPFolder::GetExistsFolderContainingCharacter(newVal))
         return false;

      // Check that no rule action exists containing this character.
      if (PersistentRuleAction::GetExistsFolderReferenceContainingCharacter(newVal))
         return false;

      // We need to replace the hierarchy delimiter in all rule actions.
      if (!PersistentRuleAction::UpdateHierarchyDelimiter(GetHierarchyDelimiter(), newVal))
         return false;

      GetSettings_()->SetString(PROPERTY_IMAP_HIERARCHY_DELIMITER, newVal);

      ObjectCache::Instance()->ClearRuleCaches();

      return true;
   }

   String
   IMAPConfiguration::GetHierarchyDelimiter()
   {
      return GetSettings_()->GetString(PROPERTY_IMAP_HIERARCHY_DELIMITER);
   }

   String 
   IMAPConfiguration::GetPublicFolderDiskName()
   {
      return "#Public";
   }

   std::shared_ptr<IMAPFolders> 
   IMAPConfiguration::GetPublicFolders()
   {
      return public_folders_;
   }


   std::shared_ptr<Groups> 
   IMAPConfiguration::GetGroups()
   {
      return groups_;
   }


   bool 
   IMAPConfiguration::XMLStore(XNode *pBackupNode, int iOptions)
   {
      // Public folders
      std::shared_ptr<IMAPFolders> pIMAPFolders = GetPublicFolders();
      pIMAPFolders->XMLStore(pBackupNode, iOptions);

      if (!GetGroups()->XMLStore(pBackupNode, iOptions))
         return false;

      return true;
   }

   bool
   IMAPConfiguration::XMLLoad(XNode *pBackupNode, int iRestoreOptions)
   {
      GetGroups()->XMLLoad(pBackupNode, iRestoreOptions);

      // Should we restore messages as well?
      if (iRestoreOptions & Backup::BOMessages)
      {
         GetPublicFolders()->XMLLoad(pBackupNode, iRestoreOptions);
      }

      return true;
   }


}
