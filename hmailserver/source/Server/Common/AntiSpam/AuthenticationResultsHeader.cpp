// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "AuthenticationResultsHeader.h"

#include "SenderAuthentication.h"

#include "../BO/MessageData.h"
#include "../Mime/Mime.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   namespace
   {
      const AnsiString FieldName = "Authentication-Results";

      String GetSPFResultText(SPF::Result result)
      {
         switch (result)
         {
         case SPF::Pass:
            return "pass";
         case SPF::Fail:
            return "fail";
         }

         return "neutral";
      }

      String GetDKIMResultText(DKIM::Result result)
      {
         switch (result)
         {
         case DKIM::Pass:
            return "pass";
         case DKIM::PermFail:
            return "fail";
         case DKIM::TempFail:
            return "temperror";
         }

         return "none";
      }
   }

   void
   AuthenticationResultsHeader::Apply(std::shared_ptr<MessageData> messageData, std::shared_ptr<SenderAuthentication> senderAuthentication)
   {
      if (!messageData || !senderAuthentication)
         return;

      std::shared_ptr<MimeBody> mimeMessage = messageData->GetMimeMessage();

      if (!mimeMessage)
         return;

      String authservId = Configuration::Instance()->GetHostName();

      // A sender may have added a header claiming to come from us, to make the
      // message look authenticated. Drop those before adding our own.
      RemoveOwnFields_(messageData, authservId);

      // Added rather than set, since results from upstream relays must be kept.
      mimeMessage->AddRawFieldValue(FieldName, BuildValue(senderAuthentication, authservId));
   }

   String
   AuthenticationResultsHeader::BuildValue(std::shared_ptr<SenderAuthentication> senderAuthentication, const String &authservId)
   {
      std::vector<String> methods;

      switch (senderAuthentication->GetDMARCResult())
      {
      case SenderAuthentication::DMARCResult::Pass:
         methods.push_back("dmarc=pass header.from=" + senderAuthentication->GetDMARCDomain());
         break;
      case SenderAuthentication::DMARCResult::Fail:
         methods.push_back("dmarc=fail header.from=" + senderAuthentication->GetDMARCDomain());
         break;
      }

      if (senderAuthentication->GetSPFChecked())
      {
         methods.push_back("spf=" + GetSPFResultText(senderAuthentication->GetSPFResult()) +
                           " smtp.mailfrom=" + senderAuthentication->GetSPFDomain());
      }

      for (auto signature : senderAuthentication->GetDKIMSignatures())
      {
         methods.push_back("dkim=" + GetDKIMResultText(signature.second) +
                           " header.d=" + String(signature.first));
      }

      if (methods.empty())
         return authservId + "; none";

      return authservId + "; " + StringParser::JoinVector(methods, "; ");
   }

   AnsiString
   AuthenticationResultsHeader::GetAuthservId(const AnsiString &fieldValue)
   {
      AnsiString value = fieldValue;
      value.Trim();

      int index = 0;
      while (index < value.GetLength())
      {
         char character = value.GetAt(index);

         if (character == ';' || character == ' ' || character == '\t')
            break;

         index++;
      }

      return value.Mid(0, index);
   }

   void
   AuthenticationResultsHeader::RemoveOwnFields_(std::shared_ptr<MessageData> messageData, const String &authservId)
   {
      std::shared_ptr<MimeBody> mimeMessage = messageData->GetMimeMessage();

      if (!mimeMessage)
         return;

      bool fieldDeleted = true;

      while (fieldDeleted)
      {
         fieldDeleted = false;

         for (int i = 0; i < mimeMessage->GetFieldCount(); i++)
         {
            MimeField *field = mimeMessage->GetField(static_cast<unsigned int>(i));

            if (FieldName.CompareNoCase(field->GetName()) != 0)
               continue;

            if (String(GetAuthservId(field->GetValue())).CompareNoCase(authservId) != 0)
               continue;

            mimeMessage->DeleteField(field);

            fieldDeleted = true;
            break;
         }
      }
   }
}
