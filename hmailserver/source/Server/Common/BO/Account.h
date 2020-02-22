// Copyright (c) 2005 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class Rules;
   class IMAPFolders;
   class Messages;

   class Account  : public BusinessObject<Account>
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

      String GetName() const {return address_; }

      void SetDomainID(__int64 newVal) { domain_id_ = newVal; };
      __int64 GetDomainID() const { return domain_id_; }

      void SetAddress(const String & newVal) { address_ = newVal; };
      String GetAddress() const { return address_; }

      void SetPassword(const String & newVal);
      String GetPassword() const { return password_; }

      void SetADDomain(const String & newVal) { addomain_ = newVal; };
      String GetADDomain() const { return addomain_; }

      void SetADUsername(const String & newVal) { adusername_ = newVal; };
      String GetADUsername() const { return adusername_; }

      void SetActive(bool newVal) { active_ = newVal; };
      bool GetActive() const { return active_; }

      void SetIsAD(bool newVal) { is_ad_ = newVal; };
      bool GetIsAD() const { return is_ad_; }

      void SetAccountMaxSize(long newVal) {account_max_size_ = newVal; }
      long GetAccountMaxSize() const {return account_max_size_;}

      bool GetVacationMessageIsOn() const; 
      void SetVacationMessageIsOn(bool bNewVal);

      String GetVacationMessage() const {return vacation_message_; }
      void SetVacationMessage(const String &sNewVal) {vacation_message_ = sNewVal;}

      String GetVacationSubject() const{return vacation_subject_; }
      void SetVacationSubject(const String &sNewVal) {vacation_subject_ = sNewVal;}

      bool GetVacationExpires() const {return vacation_expires_; }
      void SetVacationExpires(bool bNewVal) {vacation_expires_ = bNewVal ; }

      String GetVacationExpiresDate() const{return vacation_expires_date_; }
      void SetVacationExpiresDate(const String &sNewVal) {vacation_expires_date_ = sNewVal;}


      AdminLevel GetAdminLevel() const{return admin_level_;}
      void SetAdminLevel(AdminLevel iNewVal) {admin_level_ = iNewVal; }

      void SetPasswordEncryption(int iNewVal) {password_encryption_ = iNewVal; }
      long GetPasswordEncryption() const {return password_encryption_; }

      std::shared_ptr<Messages> GetMessages();
      std::shared_ptr<Rules> GetRules(); 
      std::shared_ptr<IMAPFolders> GetFolders(); 

      bool SpaceAvailable(__int64 iBytes) const;
      // Returns true if a message with iBytes bytes can fit inside the account

      bool XMLStore(XNode *pParentNode, int iBackupOptions);
      bool XMLLoad(XNode *pAccountNode, int iRestoreOptions);
      bool XMLLoadSubItems(XNode *pAccountNode, int iRestoreOptions);

      String GetForwardAddress() const {return forward_address_;}
      void SetForwardAddress(const String &sNewVal) { forward_address_ = sNewVal; }

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

      size_t GetEstimatedCachingSize();

   protected:
      __int64 domain_id_;
      
      unsigned int account_max_size_;
      // Maximum account size. MB

      long password_encryption_;

      AnsiString address_;
      AnsiString password_;
      String addomain_;
      String adusername_;

      String person_first_name_;
      String person_last_name_;

      String vacation_message_;
      String vacation_subject_;
      bool vacation_expires_;
      String vacation_expires_date_;
      
      String signature_plain_text_;
      String signature_html_;

      AnsiString forward_address_;
      bool forward_enabled_;
      bool forward_keep_original_;
      bool active_;
      bool is_ad_;
      bool vacation_message_is_on_;
      bool enable_signature_;

      std::shared_ptr<Messages> messages_;
      std::shared_ptr<Rules> rules_;
      std::shared_ptr<IMAPFolders> folders_;
      
      AdminLevel admin_level_;

      AnsiString last_logon_time_;
   };

}
