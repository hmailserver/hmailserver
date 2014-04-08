// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "AntiVirusConfiguration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   AntiVirusConfiguration::AntiVirusConfiguration()
   { 
   
   }

   AntiVirusConfiguration::~AntiVirusConfiguration()
   {
   
   }

   shared_ptr<PropertySet> 
   AntiVirusConfiguration::_GetSettings() const
   {
      return Configuration::Instance()->GetSettings();
   }

   void
	AntiVirusConfiguration::ClamWinEnabled(bool newVal) 
   {
	   _GetSettings()->SetBool(PROPERTY_AV_CLAMWIN_ENABLE, newVal);
   }

   bool 
	AntiVirusConfiguration::ClamWinEnabled() const
   {
	   return _GetSettings()->GetBool(PROPERTY_AV_CLAMWIN_ENABLE);
   }  

   void 
	AntiVirusConfiguration::ClamWinExecutable(const String & sValue)
   {
	   _GetSettings()->SetString(PROPERTY_AV_CLAMWIN_EXEC, sValue);
   }

   String
	AntiVirusConfiguration::ClamWinExecutable() const
   {
	   return _GetSettings()->GetString(PROPERTY_AV_CLAMWIN_EXEC);
   }

   void 
	AntiVirusConfiguration::ClamWinDatabase(const String & sValue)
   {
	   _GetSettings()->SetString(PROPERTY_AV_CLAMWIN_DB, sValue);
   }

   String
	AntiVirusConfiguration::ClamWinDatabase() const
   {
	   return _GetSettings()->GetString(PROPERTY_AV_CLAMWIN_DB);
   }

   void
	AntiVirusConfiguration::SetCustomScannerEnabled(bool newVal) 
   {
	   _GetSettings()->SetBool(PROPERTY_USECUSTOMSCANNER, newVal);
   }

   bool 
	AntiVirusConfiguration::GetCustomScannerEnabled() const
   {
	   return _GetSettings()->GetBool(PROPERTY_USECUSTOMSCANNER);
   }  

   void 
	AntiVirusConfiguration::SetCustomScannerExecutable(const String & sValue)
   {
	   _GetSettings()->SetString(PROPERTY_CUSTOMSCANNEREXECUTABLE, sValue);
   }

   String
	AntiVirusConfiguration::GetCustomScannerExecutable() const
   {
	   return _GetSettings()->GetString(PROPERTY_CUSTOMSCANNEREXECUTABLE);
   }

   void 
	AntiVirusConfiguration::SetCustomScannerReturnValue(long lValue)
   {
	   _GetSettings()->SetLong(PROPERTY_CUSTOMSCANNERRETURNVALUE, lValue);
   }

   long
	AntiVirusConfiguration::GetCustomScannerReturnValue() const
   {
	   return _GetSettings()->GetLong(PROPERTY_CUSTOMSCANNERRETURNVALUE);
   }

   void
   AntiVirusConfiguration::SetVirusScanMaxSize(int iNewVal)
   {
	   _GetSettings()->SetLong(PROPERTY_AV_MAXMSGSIZE, iNewVal);
   }

   int
   AntiVirusConfiguration::GetVirusScanMaxSize() const
   {
	   return _GetSettings()->GetLong(PROPERTY_AV_MAXMSGSIZE);
   }

   void
   AntiVirusConfiguration::SetEnableAttachmentBlocking(bool bValue)
   {
      _GetSettings()->SetBool(PROPERTY_ENABLEATTACHMENTBLOCKING, bValue);
   }

   bool
   AntiVirusConfiguration::GetEnableAttachmentBlocking() const
   {
      return _GetSettings()->GetBool(PROPERTY_ENABLEATTACHMENTBLOCKING);
   }

   void
   AntiVirusConfiguration::AVAction(eAVAction newVal)
   {
      _GetSettings()->SetLong(PROPERTY_AV_ACTION, newVal);
   }

   AntiVirusConfiguration::eAVAction 
   AntiVirusConfiguration::AVAction() const
   {
      switch (_GetSettings()->GetLong(PROPERTY_AV_ACTION))
      {
      case 0:
         return ActionDelete;
      case 1:
         return ActionStripAttachments;
      }

      return ActionDelete;
   }  

   void
   AntiVirusConfiguration::AVNotifySender(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_AV_NOTIFY_SENDER, newVal);
   }

   bool 
   AntiVirusConfiguration::AVNotifySender()
   {
      return _GetSettings()->GetBool(PROPERTY_AV_NOTIFY_SENDER);
   }  

   void
   AntiVirusConfiguration::AVNotifyReceiver(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_AV_NOTIFY_RECEIVER, newVal);
   }

   bool 
   AntiVirusConfiguration::AVNotifyReceiver()
   {
      return _GetSettings()->GetBool(PROPERTY_AV_NOTIFY_RECEIVER);
   } 

   void
   AntiVirusConfiguration::SetClamAVEnabled(bool newVal)
   {
      _GetSettings()->SetBool(PROPERTY_CLAMAV_ENABLED, newVal);
   }

   bool 
   AntiVirusConfiguration::GetClamAVEnabled() 
   {
      return _GetSettings()->GetBool(PROPERTY_CLAMAV_ENABLED);
   }

   void
   AntiVirusConfiguration::SetClamAVHost(const String &newVal)
   {
      _GetSettings()->SetString(PROPERTY_CLAMAV_HOST, newVal);
   }

   String 
   AntiVirusConfiguration::GetClamAVHost() 
   {
      return _GetSettings()->GetString(PROPERTY_CLAMAV_HOST);
   }

   void
   AntiVirusConfiguration::SetClamAVPort(int newVal)
   {
      _GetSettings()->SetLong(PROPERTY_CLAMAV_PORT, newVal);
   }

   int 
   AntiVirusConfiguration::GetClamAVPort() 
   {
      return _GetSettings()->GetLong(PROPERTY_CLAMAV_PORT);
   }
}
