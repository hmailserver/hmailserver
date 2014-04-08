// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MessageRecipient
   {
   public:
      MessageRecipient();
      ~MessageRecipient(void);

      enum DeliveryResult
      {
         ResultUndefined = 0,
         ResultOK = 1,
         ResultNonFatalError = 2,
         ResultFatalError =3
      };

      void CopyFrom(shared_ptr<MessageRecipient> pRecip);

      void SetAddress(const String & sNewVal) {m_sAddress = sNewVal; }
      String GetAddress() const {return m_sAddress;}

      void SetOriginalAddress(const String & sNewVal) {m_sOriginalAddress = sNewVal; }
      String GetOriginalAddress() const {return m_sOriginalAddress;}

      void SetLocalAccountID(__int64 lNewVal) {m_iLocalAccountID = lNewVal;}
      __int64 GetLocalAccountID() const{return m_iLocalAccountID;}

      void SetMessageID(__int64 lNewVal) {m_iMessageID = lNewVal;}
      __int64 GetMessageID() const {return m_iMessageID;}

      bool GetRequireAuth() const {return m_bRequiresAuthentication;}
      void SetRequiresAuth(bool bNewVal) {m_bRequiresAuthentication = bNewVal; }

      String GetRequiredSender() const {return m_sRequiredSender; }
      void SetRequiredSender(const String &sNewVal) {m_sRequiredSender = sNewVal; }

      bool GetIsLocalName() const {return _isLocalName; }
      void SetIsLocalName(bool isLocalName) {_isLocalName = isLocalName; }

      bool IsEmpty() {return m_sAddress.IsEmpty(); }

      // -- BEGIN REMOTE DELIVERY
      DeliveryResult GetDeliveryResult() const {return  m_drResult; }
      void SetDeliveryResult(DeliveryResult newVal) {m_drResult = newVal; }

      String GetErrorMessage() const {return m_sErrorMessage;}
      void SetErrorMessage(const String &sNewVal) {m_sErrorMessage = sNewVal; }
      // -- END REMOTE DELIVERY

   protected:

      String m_sAddress;
      __int64 m_iLocalAccountID;
      __int64 m_iMessageID;

      bool _isLocalName;

      bool m_bRequiresAuthentication;
      String m_sRequiredSender;
      String m_sOriginalAddress;
      
      // -- BEGIN REMOTE DELIVERY
      DeliveryResult m_drResult;
      String m_sErrorMessage;
      // -- END REMOTE DELIVERY
   };
}
