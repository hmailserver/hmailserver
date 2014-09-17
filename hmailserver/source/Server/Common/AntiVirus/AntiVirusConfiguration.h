// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SURBLServers;
   class GreyListingWhiteAddresses;
   class WhiteListAddresses;
   class DNSBlackLists;

   class AntiVirusConfiguration
   {
   public:
      enum eAVAction
      {
         ActionDelete = 0,
         ActionStripAttachments = 1
      };

      AntiVirusConfiguration();
	   virtual ~AntiVirusConfiguration();

      bool ClamWinEnabled() const;
      void ClamWinEnabled(bool Value);
      String ClamWinExecutable() const;
      void ClamWinExecutable(const String & Value);
      String ClamWinDatabase() const;
      void ClamWinDatabase(const String & Value);
      
      bool GetCustomScannerEnabled() const;
      void SetCustomScannerEnabled(bool Value);
      String GetCustomScannerExecutable() const;
      void SetCustomScannerExecutable(const String & Value);
      long GetCustomScannerReturnValue() const;
      void SetCustomScannerReturnValue(long Value);

      int GetVirusScanMaxSize() const;
      void SetVirusScanMaxSize(int iNewVal);

      bool GetEnableAttachmentBlocking() const;
      void SetEnableAttachmentBlocking(bool iNewVal);

      eAVAction AVAction() const;
      void AVAction(eAVAction Value);
      bool AVNotifySender();
      void AVNotifySender(bool Value);
      bool AVNotifyReceiver();
      void AVNotifyReceiver(bool Value);

      bool GetClamAVEnabled();
      void SetClamAVEnabled(bool bNewVal);

      String GetClamAVHost();
      void SetClamAVHost(const String &sNewVal);

      int GetClamAVPort();
      void SetClamAVPort(int bNewVal);


   private:
      
      std::shared_ptr<PropertySet> GetSettings_() const;

   };
}
