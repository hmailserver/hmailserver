// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "BackupExecuter.h"
#include "Backup.h"

#include "..\Util\Utilities.h"
#include "..\Util\Time.h"
#include "..\BO\Domains.h"
#include "..\BO\Domain.h"
#include "..\BO\IMAPFolders.h"
#include "..\BO\Accounts.h"
#include "..\BO\Aliases.h"
#include "..\BO\DomainAliases.h"
#include "..\BO\DistributionLists.h"

#include "..\Persistence\PersistentMessage.h"
#include "..\Util\Compression.h"
#include "..\Util\ServiceManager.h"

#include "BackupManager.h"
#include "ACLManager.h"
#include "Reinitializator.h"

#include "../../IMAP/IMAPConfiguration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   BackupExecuter::BackupExecuter()
   {
      backup_mode_ = 0;
   }

   BackupExecuter::~BackupExecuter(void)
   {

   }

   void 
   BackupExecuter::LoadSettings_()
   {
      destination_ = Configuration::Instance()->GetBackupDestination();
      if (destination_.Right(1) == _T("\\"))
         destination_ = destination_.Left(destination_.GetLength() - 1);

      backup_mode_ = Configuration::Instance()->GetBackupOptions();
   }

   bool 
   BackupExecuter::StartBackup()
   {
      Logger::Instance()->LogBackup("Loading backup settings....");         

      LoadSettings_();

      // Special temp setting to skip files during backup/restore while still storing/restoring db file/message info.
      bool bMessagesDBOnly = IniFileSettings::Instance()->GetBackupMessagesDBOnly();


      if (backup_mode_ & Backup::BOMessages)
      {
         if (!PersistentMessage::GetAllMessageFilesAreInDataFolder())
         {
            Application::Instance()->GetBackupManager()->OnBackupFailed("All messages are not located in the data folder.");
            return false;
         }
      }

      if (!FileUtilities::Exists(destination_))
      {
         Application::Instance()->GetBackupManager()->OnBackupFailed("The specified backup directory is not accessible: " + destination_);
         return false;
      }


      String sTime = Time::GetCurrentDateTime();
      sTime.Replace(_T(":"), _T(""));

      // Generate name for zip file. We always create zip
      // file
      String sZipFile;
      sZipFile.Format(_T("%s\\HMBackup %s.7z"), destination_.c_str(), sTime.c_str());

      String sXMLFile;
      sXMLFile.Format(_T("%s\\hMailServerBackup.xml"), destination_.c_str());

      // The name of the backup directory that
      // contains all the data files.
      String sDataBackupDir = destination_ + "\\DataBackup";

      // Backup all properties.
      XDoc oDoc; 

      XNode *pBackupNode = oDoc.AppendChild(_T("Backup"));
      XNode *pBackupInfoNode = pBackupNode->AppendChild(_T("BackupInformation"));

      // Store backup mode
      pBackupInfoNode->AppendAttr(_T("Mode"), StringParser::IntToString(backup_mode_));
      pBackupInfoNode->AppendAttr(_T("Version"), Application::Instance()->GetVersionNumber());

      // Backup business objects
      if (backup_mode_ & Backup::BODomains)
      {
         Logger::Instance()->LogBackup("Backing up domains...");

         if (!BackupDomains_(pBackupNode))
         {
            Application::Instance()->GetBackupManager()->OnBackupFailed("Could not backup domains.");
            return false;
         }
         
         // Backup message files
         if (backup_mode_ & Backup::BOMessages && !bMessagesDBOnly)
         {
            Logger::Instance()->LogBackup("Backing up data directory...");
            if (!BackupDataDirectory_(sDataBackupDir))
            {
               Application::Instance()->GetBackupManager()->OnBackupFailed("Could not backup data directory.");
               return false;
            }


         }
      }

      // Save information in the XML file where messages can be found.
      if (backup_mode_ & Backup::BOMessages)
      {
         XNode *pMessageFile = pBackupInfoNode->AppendChild(_T("DataFiles"));

         if (backup_mode_ & Backup::BOCompression)
         {
            pMessageFile->AppendAttr(_T("Format"), _T("7z"));
            pMessageFile->AppendAttr(_T("Size"), StringParser::IntToString(FileUtilities::FileSize(sZipFile)));
         }
         else
         {
            pMessageFile->AppendAttr(_T("Format"), _T("Raw"));
            pMessageFile->AppendAttr(_T("FolderName"), _T("DataBackup"));
         }
      }

      if (backup_mode_ & Backup::BOSettings)
      {
         Logger::Instance()->LogBackup("Backing up settings...");
         Configuration::Instance()->XMLStore(pBackupNode);
      }


      Logger::Instance()->LogBackup(_T("Writing XML file..."));
      String sXMLData = oDoc.GetXML();
      if (!FileUtilities::WriteToFile(sXMLFile, sXMLData, true))
      {
         Application::Instance()->GetBackupManager()->OnBackupFailed("Could not write to the XML file.");
         return false;
      }

      // Compress the XML file
      Compression oComp;
      oComp.AddFile(sZipFile, sXMLFile);

      // Delete the XML file
      FileUtilities::DeleteFile(sXMLFile);

      // Should we compress the message files?
      if (backup_mode_ & Backup::BOMessages && 
          backup_mode_ & Backup::BOCompression && !bMessagesDBOnly)
      {
         Logger::Instance()->LogBackup("Compressing message files...");
         
         if (backup_mode_ & Backup::BOMessages)
            oComp.AddDirectory(sZipFile, sDataBackupDir + "\\");

         // Since the files are now compressed, we can deleted
         // the data backup directory
         if (!FileUtilities::DeleteDirectory(sDataBackupDir, true))
         {
            Application::Instance()->GetBackupManager()->OnBackupFailed("Could not delete files from the destination directory.");
            return false;
         }
       }

      Application::Instance()->GetBackupManager()->OnBackupCompleted();

      return true;
   }

   bool 
   BackupExecuter::BackupDataDirectory_(const String &sDataBackupDir)
   {
      String sDataDir = IniFileSettings::Instance()->GetDataDirectory();

      String errorMessage;

      bool bResult = FileUtilities::CopyDirectory(sDataDir, sDataBackupDir, errorMessage);
      if (!bResult)
      {
         Logger::Instance()->LogBackup("Failed to copy data directory. Details: " + errorMessage);
         return bResult;
      }

      bResult = FileUtilities::DeleteFilesInDirectory(sDataBackupDir);

      if (!bResult)
      {
         Logger::Instance()->LogBackup("Failed to delete files in backup root directory. Please see hMailServer error log.");
      }
      
      return bResult;
   }

   bool 
   BackupExecuter::BackupDomains_(XNode *pBackupNode)
   {
      std::shared_ptr<Domains> pDomains = std::shared_ptr<Domains>(new Domains);
      pDomains->Refresh();
      pDomains->XMLStore(pBackupNode, backup_mode_);

      return true;
   }

   bool
   BackupExecuter::StartRestore(std::shared_ptr<Backup> pBackup)
   {
      bool bMessagesDBOnly = IniFileSettings::Instance()->GetBackupMessagesDBOnly();

      Logger::Instance()->LogBackup("Reading XML file...");
      String sZipFile = pBackup->GetBackupFile();

      String sTempDir = IniFileSettings::Instance()->GetTempDirectory();
      String sXMLFile = sTempDir + "\\hMailServerBackup.xml";
      FileUtilities::DeleteFile(sXMLFile);

      Compression oComp;
      if (!oComp.Uncompress(sZipFile, sTempDir, "hMailServerBackup.xml"))
      {
         String sErrorMessage = Formatter::Format("Unable to uncompress hMailServerBackup.xml from {0} to {1}. Please confirm that hMailServer has permissions to {0} and {1}.", sZipFile, sTempDir);
         Application::Instance()->GetBackupManager()->OnBackupFailed(sErrorMessage);
         return false;
      }

      String sXMLData = FileUtilities::ReadCompleteTextFile(sXMLFile);
      if (sXMLData.IsEmpty())
      {
         String sErrorMessage = Formatter::Format("The file {0} could not be read.", sXMLFile);
         Application::Instance()->GetBackupManager()->OnBackupFailed(sErrorMessage);
         return false;
      }

      XDoc oDoc;
      oDoc.Load(sXMLData);

      FileUtilities::DeleteFile(sXMLFile);

      String sBackup = "Backup";
      XNode *pBackupNode = oDoc.GetChild(sBackup);
      if (!pBackupNode)
      {
         String sErrorMessage = "The supplied XML file is not a valid hMailServer backup file";
         Application::Instance()->GetBackupManager()->OnBackupFailed(sErrorMessage);
         return false;
      }

      int iRestoreOptions = pBackup->GetRestoreOptions();

    
      if (iRestoreOptions & Backup::BODomains)
      {
         // First drop all domains. We need to do this prior to restoring
         // the data directory. When we drop the domains, we will also
         // drop the domain folders from the data directory. If we do this
         // in the wrong order, we'll hence first restore the data directory
         // and then drop it.
         std::shared_ptr<Domains> pDomains = std::shared_ptr<Domains>(new Domains);

         pDomains->Refresh();
         if (!bMessagesDBOnly) 
            pDomains->DeleteAll();

         // We need to do the same with public folders.
         if (iRestoreOptions & Backup::BOSettings && !bMessagesDBOnly)
            Configuration::Instance()->GetIMAPConfiguration()->GetPublicFolders()->DeleteAll();
         
         // Should we restore messages as well?
         if (iRestoreOptions & Backup::BOMessages && !bMessagesDBOnly)
         {
            Logger::Instance()->LogBackup("Restoring data directory...");
            RestoreDataDirectory_(pBackup, pBackupNode);
         }

         Logger::Instance()->LogBackup("Restoring domains...");

         if (!pDomains->XMLLoad(pBackupNode, iRestoreOptions))
         {
            String sErrorMessage = "Restore of domains failed. Please check hMailServer log.";
            Logger::Instance()->LogBackup(sErrorMessage);
            Application::Instance()->GetBackupManager()->OnBackupFailed(sErrorMessage);
            
            return false;
         }
      }

      // Backup settings last since they may be referring to objects in the domains.
      if (iRestoreOptions & Backup::BOSettings)
      {
         Logger::Instance()->LogBackup("Restoring settings...");
         if (!Configuration::Instance()->XMLLoad(pBackupNode, iRestoreOptions))
         {
            String sErrorMessage = "Restore of settings failed. Please check hMailServer log.";
            Logger::Instance()->LogBackup(sErrorMessage);
            Application::Instance()->GetBackupManager()->OnBackupFailed(sErrorMessage);
            return false;
         }
      }


      // Reinitialize server since everything may have changed.
      // We can't run Application::ReInitialize here, since this
      // thread is owned by the backup manager, which is owned
      // by the application. And the backup manager is recreated
      // upon reinitialization.
      Logger::Instance()->LogBackup("Reinitializing server (async)...");

      Reinitializator::Instance()->ReInitialize();

      Logger::Instance()->LogBackup("Restore completed successfully.");         

      return true;
   }

   void
   BackupExecuter::RestoreDataDirectory_(std::shared_ptr<Backup> pBackup, XNode *pBackupNode)
   {
      XNode *pBackupInfoNode = pBackupNode->GetChild(_T("BackupInformation"));
      
      // Create the path to the zip file.
      String sBackupFile = pBackup->GetBackupFile();
      String sPath = sBackupFile.Mid(0, sBackupFile.ReverseFind(_T("\\")));

      String sDirContainingDataFiles;
      String sDataFileFormat = pBackupInfoNode->GetChildAttr(_T("DataFiles"), _T("Format"))->value;
      
      String sExtractedFilesDirectory;
      if (sDataFileFormat.CompareNoCase(_T("7Z")) == 0)
      {
         // Create the path to the directory that will contain the extracted files. 
         //  This directory is temporary and will be removed when we're done.
         sExtractedFilesDirectory = Utilities::GetUniqueTempDirectory();

         // Extract the files to this directory.
         Compression oComp;
         oComp.Uncompress(sBackupFile, sExtractedFilesDirectory);

         // The data files in the zip file are stored in
         // a directory called DataBackup.
         sDirContainingDataFiles = sExtractedFilesDirectory + "\\DataBackup";
      }
      else
      {
         // Fetch the path to the data files.
         String sFolderName = pBackupInfoNode->GetChildAttr(_T("DataFiles"), _T("FolderName"))->value;
         sDirContainingDataFiles = sPath + "\\" + sFolderName;
      }

      // Delete all directories from the data directory
      // so that we're sure that we're doing a clean restore
      String sDataDirectory = IniFileSettings::Instance()->GetDataDirectory();
      
      FileUtilities::DeleteFilesInDirectory(sDataDirectory);
      FileUtilities::DeleteDirectoriesInDirectory(sDataDirectory);

      String errorMessage;
      FileUtilities::CopyDirectory(sDirContainingDataFiles, sDataDirectory, errorMessage);

      if (sDataFileFormat.CompareNoCase(_T("7z")) == 0)
      {
         // The temporary directory we created while
         // unzipping should be deleted now.
         FileUtilities::DeleteDirectory(sExtractedFilesDirectory, true);
      }
   }
}
