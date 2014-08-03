// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once


#include "../Util/VariantDateTime.h"

namespace HM
{

   class MessageMetaData : public BusinessObject<MessageMetaData>
   {
   public:

      enum MetaDataField
      {
         Unknown = 0,
         From = 1,
         To = 2,
         CC = 3,
         Subject = 4,
         Date = 5
      };

      MessageMetaData();

      int GetAccountID() const {return account_id_;}
      void SetAccountID(int value) {account_id_ = value;}

      int GetFolderID() const {return folder_id_;}
      void SetFolderID(int value) {folder_id_ = value;}

      __int64 GetMessageID() const {return message_id_;}
      void SetMessageID(__int64 value) {message_id_ = value;}

      String GetFrom() const  {return from_;}
      void SetFrom(String value) {from_ = value;}

      String GetTo() const {return to_;}
      void SetTo(String value) {to_ = value;}

      String GetCC() const {return cc_;}
      void SetCC(String value) {cc_ = value;}

      String GetSubject() const {return subject_;}
      void SetSubject(String value) {subject_ = value;}

      DateTime GetDate() const {return date_;}
      void SetDate(DateTime value) {date_ = value;}

      static MetaDataField GetMetaDataField(const String &fieldName);

   private:
      int account_id_;
      int folder_id_;
      __int64 message_id_;
      
      // contents
      String from_;
      String to_;
      String cc_;
      String subject_;
      DateTime date_;
   };
}
