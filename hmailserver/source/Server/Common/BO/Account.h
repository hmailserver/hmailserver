// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Cache/Cacheable.h"

namespace HM
{

   class Rules;
   class IMAPFolders;
   class Messages;

   class Account  : public Cacheable, public BusinessObject<Account>
   {
   public:
      enum AdminLevel
      {
         NormalUser = 0,
         DomainAdmin = 1,
         ServerAdmin = 2
      };

	   Account();
      Account(const String &address, AdminLevel adminLevel);
	   Account(const Account &oldAccount);

      virtual ~Account();
	   


      void Initialize();

      String GetName() const {return m_sAddress; }

      void SetDomainID(__int64 newVal) { m_iDomainID = newVal; };
      __int64 GetDomainID() const { return m_iDomainID; }

      void SetAddress(const String & newVal) { m_sAddress = newVal; };
      String GetAddress() const { return m_sAddress; }

      void SetPassword(const String & newVal);
      String GetPassword() const { return m_sPassword; }

      void SetADDomain(const String & newVal) { m_sADDomain = newVal; };
      String GetADDomain() const { return m_sADDomain; }

      void SetADUsername(const String & newVal) { m_sADUsername = newVal; };
      String GetADUsername() const { return m_sADUsername; }

      void SetActive(bool newVal) { m_bActive = newVal; };
      bool GetActive() const { return m_bActive; }

      void SetIsAD(bool newVal) { m_bIsAD = newVal; };
      bool GetIsAD() const { return m_bIsAD; }

      void SetAccountMaxSize(long newVal) {m_iAccountMaxSize = newVal; }
      long GetAccountMaxSize() const {return m_iAccountMaxSize;}

      bool GetVacationMessageIsOn() const; 
      void SetVacationMessageIsOn(bool bNewVal);

      String GetVacationMessage() const {return m_sVacationMessage; }
      void SetVacationMessage(const String &sNewVal) {m_sVacationMessage = sNewVal;}

      String GetVacationSubject() const{return m_sVacationSubject; }
      void SetVacationSubject(const String &sNewVal) {m_sVacationSubject = sNewVal;}

      bool GetVacationExpires() const {return m_bVacationExpires; }
      void SetVacationExpires(bool bNewVal) {m_bVacationExpires = bNewVal ; }

      String GetVacationExpiresDate() const{return m_sVacationExpiresDate; }
      void SetVacationExpiresDate(const String &sNewVal) {m_sVacationExpiresDate = sNewVal;}


      AdminLevel GetAdminLevel() const{return m_iAdminLevel;}
      void SetAdminLevel(AdminLevel iNewVal) {m_iAdminLevel = iNewVal; }

      void SetPasswordEncryption(int iNewVal) {m_iPasswordEncryption = iNewVal; }
      long GetPasswordEncryption() const {return m_iPasswordEncryption; }

      shared_ptr<Messages> GetMessages();
      shared_ptr<Rules> GetRules(); 
      shared_ptr<IMAPFolders> GetFolders(); 

      bool SpaceAvailable(__int64 iBytes) const;
      // Returns true if a message with iBytes bytes can fit inside the account

      bool XMLStore(XNode *pParentNode, int iBackupOptions);
      bool XMLLoad(XNode *pAccountNode, int iRestoreOptions);
      bool XMLLoadSubItems(XNode *pAccountNode, int iRestoreOptions);

      String GetForwardAddress() const {return m_sForwardAddress;}
      void SetForwardAddress(const String &sNewVal) { m_sForwardAddress = sNewVal; }

      bool GetForwardEnabled() const;
      void SetForwardEnabled(bool bEnabled);
      
      bool GetForwardKeepOriginal() const;
      void SetForwardKeepOriginal(bool bEnabled);

      bool GetEnableSignature() const;
      void SetEnableSignature(bool bEnabled);

      String GetSignaturePlainText() const;
      void SetSignaturePlainText(const String &sNewVal);

      String GetSignatureHTML() const;
      void SetSignatureHTML(const String &sNewVal);

      String GetLastLogonTime() const;
      void SetLastLogonTime(const String &sNewVal);

      String GetPersonFirstName() const;
      void SetPersonFirstName(const String &sNewVal);

      String GetPersonLastName() const;
      void SetPersonLastName(const String &sNewVal);

   protected:
      __int64 m_iDomainID;
      
      unsigned int m_iAccountMaxSize;
      // Maximum account size. MB

      long m_iPasswordEncryption;

      AnsiString m_sAddress;
      AnsiString m_sPassword;
      String m_sADDomain;
      String m_sADUsername;

      String m_sPersonFirstName;
      String m_sPersonLastName;

      String m_sVacationMessage;
      String m_sVacationSubject;
      bool m_bVacationExpires;
      String m_sVacationExpiresDate;
      
      String m_sSignaturePlainText;
      String m_sSignatureHTML;

      AnsiString m_sForwardAddress;
      bool m_bForwardEnabled;
      bool m_bForwardKeepOriginal;
      bool m_bActive;
      bool m_bIsAD;
      bool m_bVacationMessageIsOn;
      bool m_bEnableSignature;

      shared_ptr<Messages> m_oMessages;
      shared_ptr<Rules> m_pRules;
      shared_ptr<IMAPFolders> m_pFolders;
      
      AdminLevel m_iAdminLevel;

      AnsiString m_sLastLogonTime;
   };

}
