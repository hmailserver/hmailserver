// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPCommandSEARCH.h"
#include "IMAPConnection.h"
#include "IMAPSort.h"
#include "IMAPConfiguration.h"
#include "IMAPListLookup.h"

#include "../Common/BO/IMAPFolder.h"
#include "../Common/Persistence/PersistentMessage.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/Messages.h"
#include "../Common/BO/MessageData.h"
#include "../Common/Mime/Mime.h"
#include "../Common/Util/Time.h"
#include "../Common/Util/VariantDateTime.h"
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommandSEARCH::IMAPCommandSEARCH(bool bIsSort) :
      is_sort_(bIsSort),
      is_uid_(false)
   {

   }

   IMAPCommandSEARCH::~IMAPCommandSEARCH()
   {
      
   }

   IMAPResult
   IMAPCommandSEARCH::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (is_sort_ && !Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPSort())
         return IMAPResult(IMAPResult::ResultNo, "IMAP SORT is not enabled.");

      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      if (!pConnection->GetCurrentFolder())
         return IMAPResult(IMAPResult::ResultNo, "No folder selected.");

      if (!pArgument)
         return IMAPResult(IMAPResult::ResultNo, "Internal error IMAP-SEARCH-1.");

      {
         // The IMAP Search parser should not parse
         // the beginning of the command, UID SEARCH
         // or SEARCH
         String sCommand = pArgument->Command();

         int iCommandStartPos;
         
         if (is_uid_)
            iCommandStartPos = sCommand.Find(_T(" "), 4) + 1;
         else
            iCommandStartPos = sCommand.Find(_T(" ")) + 1;

         sCommand = sCommand.Mid(iCommandStartPos); // 3 as in UID

         pArgument->Command(sCommand);
      }

      std::shared_ptr<IMAPSearchParser> pParser = std::shared_ptr<IMAPSearchParser>(new IMAPSearchParser());
      IMAPResult result = pParser->ParseCommand(pArgument, is_sort_);
      if (result.GetResult() != IMAPResult::ResultOK)
         return result;

      if (is_sort_ && !pParser->GetSortParser())
         return IMAPResult(IMAPResult::ResultBad, "Incorrect search commands.");

      // Mails in current box
      std::shared_ptr<IMAPFolder> pCurFolder =  pConnection->GetCurrentFolder();

      if (!pCurFolder)
         return IMAPResult(IMAPResult::ResultBad, "No selected folder");

      std::vector<std::shared_ptr<Message>> messages = pCurFolder->GetMessages()->GetCopy();

      std::vector<String> sMatchingVec;
      if (messages.size() > 0)
      {
         // Iterate through the messages and see which ones match.
         std::vector<std::pair<int, std::shared_ptr<Message> > > vecMatchingMessages;

         int index = 0;
         for(std::shared_ptr<Message> pMessage : messages)
         {
            const String fileName = PersistentMessage::GetFileName(pConnection->GetAccount(), pMessage);

            index++;
            if (pMessage && DoesMessageMatch_(pConnection, pParser->GetCriteria(), fileName, pMessage, index))
            {
               // Yup we got a match.
               vecMatchingMessages.push_back(make_pair(index, pMessage));
            }
         }

         if (is_sort_)
         {
            IMAPSort oSorter;
            oSorter.Sort(pConnection, vecMatchingMessages, pParser->GetCharsetName(), pParser->GetSortParser());
            // Sort the message vector
         }

         typedef std::pair<int, std::shared_ptr<Message> > MessagePair;
         for(MessagePair messagePair : vecMatchingMessages)
         {
            int index = messagePair.first;
            std::shared_ptr<Message> pMessage = messagePair.second;

            String sID;
            if (is_uid_)
               sID.Format(_T("%u"), pMessage->GetUID());
            else
               sID.Format(_T("%d"), index);

            sMatchingVec.push_back(sID);
         }

      }

      // Send response
      String sMatching;
      if (sMatchingVec.size() > 0)
      {
         // If we don't find any matches, we shouldn't return a whitespace
         // after SEARCH/SORT below. That's why we add the white space here.
         sMatching = " " + StringParser::JoinVector(sMatchingVec, " ") ;
      }
      
      String sResponse;
      if (is_sort_)
         sResponse = "* SORT" + sMatching + "\r\n";
      else
         sResponse = "* SEARCH" + sMatching + "\r\n";

      if (!is_uid_) 
         // if this is a UID command, IMAPCommandUID takes care of the below line.
         sResponse += pArgument->Tag() + " OK Search completed\r\n";

      pConnection->SendAsciiData(sResponse);

      return IMAPResult();
   }

   bool
   IMAPCommandSEARCH::DoesMessageMatch_(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPSearchCriteria> pParentCriteria, const String &fileName, std::shared_ptr<Message> pMessage, int index)
   {
      message_data_.reset();
      mime_header_.reset();

      bool bIsOrCriteria = pParentCriteria->GetIsOR();

      // Loop over the criterias in the command.
      std::vector<std::shared_ptr<IMAPSearchCriteria> > &vecCriterias = pParentCriteria->GetSubCriterias();
      auto iterCriteria = vecCriterias.begin();

      bool bMessageIsMatchingCriteria = true;
      while (iterCriteria != vecCriterias.end())
      {
         bMessageIsMatchingCriteria = true;

         std::shared_ptr<IMAPSearchCriteria> pCriteria = (*iterCriteria);

         if (pCriteria->GetType() == IMAPSearchCriteria::CTSubCriteria)
         {
            if (!DoesMessageMatch_(pConnection, pCriteria, fileName, pMessage, index))
               bMessageIsMatchingCriteria = false;
         }

         switch (pCriteria->GetType())
         {
         case IMAPSearchCriteria::CTDeleted:
            {
               if (pCriteria->GetPositive() && !pMessage->GetFlagDeleted() ||
                   !pCriteria->GetPositive() && pMessage->GetFlagDeleted())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTAll:
            {
               if (!pCriteria->GetPositive())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }

         case IMAPSearchCriteria::CTUndeleted:
            {
               if (pCriteria->GetPositive() && pMessage->GetFlagDeleted() ||
                   !pCriteria->GetPositive() && !pMessage->GetFlagDeleted())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTSeen:
            {
               if (pCriteria->GetPositive() && !pMessage->GetFlagSeen() ||
                   !pCriteria->GetPositive() && pMessage->GetFlagSeen())
               {
                  bMessageIsMatchingCriteria = false;;
               }
               break;
            }
         case IMAPSearchCriteria::CTUnseen:
            {
               if (pCriteria->GetPositive() && pMessage->GetFlagSeen() ||
                   !pCriteria->GetPositive() && !pMessage->GetFlagSeen())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTRecent:
            {
               bool is_recent = IsMessageRecent_(pConnection, pMessage->GetID());

               if (pCriteria->GetPositive() && !is_recent ||
                  !pCriteria->GetPositive() && is_recent)
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTHeader:
            {
               if (!MatchesHeaderCriteria_(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;

               break;
            }
         case IMAPSearchCriteria::CTUID:
            {
               if (!MatchesUIDCriteria_(pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSequenceSet:
            {
               if (!MatchesSequenceSetCriteria_(pMessage, pCriteria, index))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTText:
            {
               if (!MatchesTEXTCriteria_(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTBody:
            {
               if (!MatchesBODYCriteria_(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSubject:
            {
               pCriteria->SetHeaderField("Subject");
               if (!MatchesHeaderCriteria_(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTFrom:
            {
               pCriteria->SetHeaderField("From");
               if (!MatchesHeaderCriteria_(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTTo:
            {
               pCriteria->SetHeaderField("To");
               if (!MatchesHeaderCriteria_(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTCC:
            {
               pCriteria->SetHeaderField("CC");
               if (!MatchesHeaderCriteria_(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTOn:
            {
               if (!MatchesONCriteria_(pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSentOn:
            {
               if (!MatchesSENTONCriteria_(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSentBefore:
            {
               if (!MatchesSENTBEFORECriteria_(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSentSince:
            {
               if (!MatchesSENTSINCECriteria_(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSince:
            {
               if (!MatchesSINCECriteria_(pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTAnswered:
            {
               if (pCriteria->GetPositive() && !pMessage->GetFlagAnswered() ||
                  !pCriteria->GetPositive() && pMessage->GetFlagAnswered())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTDraft:
            {
               if (pCriteria->GetPositive() && !pMessage->GetFlagDraft() ||
                  !pCriteria->GetPositive() && pMessage->GetFlagDraft())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTFlagged:
            {
               if (pCriteria->GetPositive() && !pMessage->GetFlagFlagged() ||
                  !pCriteria->GetPositive() && pMessage->GetFlagFlagged())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTNew:
            {
               bool is_recent = IsMessageRecent_(pConnection, pMessage->GetID());

               bool bSet = is_recent && !pMessage->GetFlagSeen();
               if (pCriteria->GetPositive() && !bSet ||
                  !pCriteria->GetPositive() && bSet)
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTOld:
            {
               bool is_recent = IsMessageRecent_(pConnection, pMessage->GetID());
               bool bSet = !is_recent;

               if (pCriteria->GetPositive() && !bSet ||
                  !pCriteria->GetPositive() && bSet)
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTUnanswered:
            {
               if (pCriteria->GetPositive() && pMessage->GetFlagAnswered() ||
                  !pCriteria->GetPositive() && !pMessage->GetFlagAnswered())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTUndraft:
            {
               if (pCriteria->GetPositive() && pMessage->GetFlagDraft() ||
                  !pCriteria->GetPositive() && !pMessage->GetFlagDraft())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTUnflagged:
            {
               if (pCriteria->GetPositive() && pMessage->GetFlagFlagged() ||
                  !pCriteria->GetPositive() && !pMessage->GetFlagFlagged())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTBefore:
            {
               if (!MatchesBEFORECriteria_(pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTLarger:
            {
               if (!MatchesLARGERCriteria_(pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSmaller:
            {
               if (!MatchesSMALLERCriteria_(pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }

         }

         if (bIsOrCriteria)
         {
            if (bMessageIsMatchingCriteria)
               return true;
         }
         else
         {
            // This isn't an OR criteria.
            if (!bMessageIsMatchingCriteria)
               return false;
         }

         iterCriteria++;
      }

      return bMessageIsMatchingCriteria;

    }

   bool
   IMAPCommandSEARCH::MatchesBODYCriteria_(const String &fileName, std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      if (!message_data_)
      {
         message_data_ = std::shared_ptr<MessageData>(new MessageData());
         message_data_->LoadFromMessage(fileName, pMessage);
      }

      String sBody = message_data_->GetBody();
      String sHTMLBody = message_data_->GetHTMLBody();

      String sTextToSearchIn = sBody + sHTMLBody;
      String sTextToFind = pCriteria->GetText();

      if (sTextToSearchIn.ContainsNoCase(sTextToFind))
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::MatchesONCriteria_(std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      String sCreationDate = pMessage->GetCreateTime();

      DateTime dt = Time::GetDateFromSystemDate(sCreationDate);
      DateTime criteriaDate = Time::GetDateFromIMAP(pCriteria->GetText()); 

      bool bMatch = false;
      if (dt.GetYear() == criteriaDate.GetYear() &&
          dt.GetMonth() == criteriaDate.GetMonth() &&
          dt.GetDay() == criteriaDate.GetDay())
      {
         bMatch = true;
      }

      if (bMatch)
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::MatchesSENTONCriteria_(const String &fileName, std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      String sDateHeader = GetHeaderValue_(fileName, pMessage, "Date");
      sDateHeader = Time::GetIMAPDateFromMimeHeader(sDateHeader);

      if (sDateHeader == pCriteria->GetText())
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::MatchesSENTBEFORECriteria_(const String &fileName, std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      String sDateHeader = GetHeaderValue_(fileName, pMessage, "Date");

      DateTime dtSentDate = Time::GetDateFromMimeHeader(sDateHeader);
      DateTime dtCriteria = Time::GetDateFromIMAP(pCriteria->GetText());

      if (dtSentDate <= dtCriteria)
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::MatchesSENTSINCECriteria_(const String &fileName, std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      String sDateHeader = GetHeaderValue_(fileName, pMessage, "Date");

      DateTime dtSentDate = Time::GetDateFromMimeHeader(sDateHeader);
      DateTime dtCriteria = Time::GetDateFromIMAP(pCriteria->GetText());

      if (dtSentDate >= dtCriteria)
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::MatchesSINCECriteria_(std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Messages whose internal date is within or later
   // than the specified date.
   //---------------------------------------------------------------------------()
   {
      DateTime dtSentDate = Time::GetDateFromSystemDate(pMessage->GetCreateTime());
      DateTime dtCriteria = Time::GetDateFromIMAP(pCriteria->GetText());

      if (dtSentDate >= dtCriteria)
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::MatchesBEFORECriteria_(std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Messages whose internal date is before the specified date
   //---------------------------------------------------------------------------()
   {
      DateTime dtMessageDate = Time::GetDateFromSystemDate(pMessage->GetCreateTime());
      DateTime dtCriteria = Time::GetDateFromIMAP(pCriteria->GetText());

      if (dtMessageDate < dtCriteria)
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::MatchesLARGERCriteria_(std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Messages whose size is larger than the size specified in critera.
   //---------------------------------------------------------------------------()
   {
      int iMessageSize = pMessage->GetSize();
      int iCriteriaSize = _ttoi(pCriteria->GetText());

      if (iMessageSize > iCriteriaSize)
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::MatchesSMALLERCriteria_(std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Messages whose size is smaller than the size specified in critera.
   //---------------------------------------------------------------------------()
{
      int iMessageSize = pMessage->GetSize();
      int iCriteriaSize = _ttoi(pCriteria->GetText());

      if (iMessageSize < iCriteriaSize)
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::MatchesTEXTCriteria_(const String &fileName, std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      if (!message_data_)
      {
         message_data_ = std::shared_ptr<MessageData>(new MessageData());
         message_data_->LoadFromMessage(fileName, pMessage);
      }

      String sHeader = message_data_->GetHeader();
      String sBody = message_data_->GetBody();
      String sHTMLBody = message_data_->GetHTMLBody();

      String sTextToFind = pCriteria->GetText();

      if (pCriteria->GetPositive())
      {
         if (!sHeader.ContainsNoCase(sTextToFind) && 
             !sBody.ContainsNoCase(sTextToFind) &&
             !sHTMLBody.ContainsNoCase(sTextToFind))
             return false;
      }
      else
      {
         if (sHeader.ContainsNoCase(sTextToFind) ||
             sBody.ContainsNoCase(sTextToFind) ||
             sHTMLBody.ContainsNoCase(sTextToFind))
             return false;
      }

      return true;


   }

   bool
   IMAPCommandSEARCH::MatchesUIDCriteria_(std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      std::vector<String> split = pCriteria->GetSequenceSet();

      bool found = IMAPListLookup::IsItemInList(split, (int) pMessage->GetUID());
      
      if (pCriteria->GetPositive())
         return found;
      else
         return !found;
   }

   bool
   IMAPCommandSEARCH::MatchesSequenceSetCriteria_(std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria, int index)
   {
      std::vector<String> split = pCriteria->GetSequenceSet();

      bool found = IMAPListLookup::IsItemInList(split, index);

      if (pCriteria->GetPositive())
         return found;
      else
         return !found;
   }


   bool
   IMAPCommandSEARCH::MatchesHeaderCriteria_(const String &fileName, std::shared_ptr<Message> pMessage, std::shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      String sHeaderField = pCriteria->GetHeaderField();
      String sTextToFind = pCriteria->GetText();
      
      String sHeaderFieldValue = GetHeaderValue_(fileName, pMessage, sHeaderField);

      if (sHeaderFieldValue.ContainsNoCase(sTextToFind))
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool 
   IMAPCommandSEARCH::IsMessageRecent_(std::shared_ptr<IMAPConnection> pConnection, __int64 message_uid)
   {
      auto& recent_messages = pConnection->GetRecentMessages();

      auto recent_messages_iter = recent_messages.find(message_uid);
      return recent_messages_iter != recent_messages.end();
   }

   String
   IMAPCommandSEARCH::GetHeaderValue_(const String &fileName, std::shared_ptr<Message> pMessage, const String &sHeaderField)
   {
      if (message_data_)
      {
         message_data_ = std::shared_ptr<MessageData>(new MessageData());
         message_data_->LoadFromMessage(fileName, pMessage);

         return message_data_->GetFieldValue(sHeaderField);
      }
      
      if (!mime_header_)
      {
         // Load header
         AnsiString sHeader = PersistentMessage::LoadHeader(fileName);

         mime_header_ = std::shared_ptr<MimeHeader>(new MimeHeader);
         mime_header_->Load(sHeader, sHeader.GetLength(), true);
      }

      AnsiString sHeaderFieldStr = sHeaderField;
      return mime_header_->GetUnicodeFieldValue(sHeaderFieldStr);

   }

}
