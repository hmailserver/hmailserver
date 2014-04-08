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

      int GetAccountID() const {return _accountID;}
      void SetAccountID(int value) {_accountID = value;}

      int GetFolderID() const {return _folderID;}
      void SetFolderID(int value) {_folderID = value;}

      __int64 GetMessageID() const {return _messageID;}
      void SetMessageID(__int64 value) {_messageID = value;}

      String GetFrom() const  {return _from;}
      void SetFrom(String value) {_from = value;}

      String GetTo() const {return _to;}
      void SetTo(String value) {_to = value;}

      String GetCC() const {return _cc;}
      void SetCC(String value) {_cc = value;}

      String GetSubject() const {return _subject;}
      void SetSubject(String value) {_subject = value;}

      DateTime GetDate() const {return _date;}
      void SetDate(DateTime value) {_date = value;}

      static MetaDataField GetMetaDataField(const String &fieldName);

   private:
      int _accountID;
      int _folderID;
      __int64 _messageID;
      
      // contents
      String _from;
      String _to;
      String _cc;
      String _subject;
      DateTime _date;
   };
}
