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
      m_bIsSort(bIsSort),
      m_bIsUID(false)
   {

   }

   IMAPCommandSEARCH::~IMAPCommandSEARCH()
   {
      
   }

   IMAPResult
   IMAPCommandSEARCH::ExecuteCommand(shared_ptr<IMAPConnection> pConnection, shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (m_bIsSort && !Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPSort())
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
         
         if (m_bIsUID)
            iCommandStartPos = sCommand.Find(_T(" "), 4) + 1;
         else
            iCommandStartPos = sCommand.Find(_T(" ")) + 1;

         sCommand = sCommand.Mid(iCommandStartPos); // 3 as in UID

         pArgument->Command(sCommand);
      }

      shared_ptr<IMAPSearchParser> pParser = shared_ptr<IMAPSearchParser>(new IMAPSearchParser());
      IMAPResult result = pParser->ParseCommand(pArgument, m_bIsSort);
      if (result.GetResult() != IMAPResult::ResultOK)
         return result;

      if (m_bIsSort && !pParser->GetSortParser())
         return IMAPResult(IMAPResult::ResultBad, "Incorrect search commands.");

      // Mails in current box
      shared_ptr<IMAPFolder> pCurFolder =  pConnection->GetCurrentFolder();

      if (!pCurFolder)
         return IMAPResult(IMAPResult::ResultBad, "No selected folder");

      vector<shared_ptr<Message>> messages = pCurFolder->GetMessages()->GetCopy();

      std::vector<String> sMatchingVec;
      if (messages.size() > 0)
      {
         // Iterate through the messages and see which ones match.
         vector<pair<int, shared_ptr<Message> > > vecMatchingMessages;

         int index = 0;
         boost_foreach(shared_ptr<Message> pMessage, messages)
         {
            const String fileName = PersistentMessage::GetFileName(pConnection->GetAccount(), pMessage);

            index++;
            if (pMessage && _DoesMessageMatch(pParser->GetCriteria(), fileName, pMessage, index))
            {
               // Yup we got a match.
               vecMatchingMessages.push_back(make_pair(index, pMessage));
            }
         }

         if (m_bIsSort)
         {
            IMAPSort oSorter;
            oSorter.Sort(pConnection, vecMatchingMessages, pParser->GetSortParser());
            // Sort the message vector
         }

         typedef pair<int, shared_ptr<Message> > MessagePair;
         boost_foreach(MessagePair messagePair, vecMatchingMessages)
         {
            int index = messagePair.first;
            shared_ptr<Message> pMessage = messagePair.second;

            String sID;
            if (m_bIsUID)
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
      if (m_bIsSort)
         sResponse = "* SORT" + sMatching + "\r\n";
      else
         sResponse = "* SEARCH" + sMatching + "\r\n";

      if (!m_bIsUID) 
         // if this is a UID command, IMAPCommandUID takes care of the below line.
         sResponse += pArgument->Tag() + " OK Search completed\r\n";

      pConnection->SendAsciiData(sResponse);

      return IMAPResult();
   }

   bool
   IMAPCommandSEARCH::_DoesMessageMatch(shared_ptr<IMAPSearchCriteria> pParentCriteria, const String &fileName, shared_ptr<Message> pMessage, int index)
   {
      m_pMessageData.reset();
      m_pMimeHeader.reset();

      bool bIsOrCriteria = pParentCriteria->GetIsOR();

      // Loop over the criterias in the command.
      vector<shared_ptr<IMAPSearchCriteria> > &vecCriterias = pParentCriteria->GetSubCriterias();
      vector<shared_ptr<IMAPSearchCriteria> >::iterator iterCriteria = vecCriterias.begin();

      bool bMessageIsMatchingCriteria = true;
      while (iterCriteria != vecCriterias.end())
      {
         bMessageIsMatchingCriteria = true;

         shared_ptr<IMAPSearchCriteria> pCriteria = (*iterCriteria);

         if (pCriteria->GetType() == IMAPSearchCriteria::CTSubCriteria)
         {
            if (!_DoesMessageMatch(pCriteria, fileName, pMessage, index))
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
               if (pCriteria->GetPositive() && !pMessage->GetFlagRecent() ||
                   !pCriteria->GetPositive() && pMessage->GetFlagRecent())
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTHeader:
            {
               if (!_MatchesHeaderCriteria(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;

               break;
            }
         case IMAPSearchCriteria::CTUID:
            {
               if (!_MatchesUIDCriteria(pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSequenceSet:
            {
               if (!_MatchesSequenceSetCriteria(pMessage, pCriteria, index))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTText:
            {
               if (!_MatchesTEXTCriteria(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTBody:
            {
               if (!_MatchesBODYCriteria(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSubject:
            {
               pCriteria->SetHeaderField("Subject");
               if (!_MatchesHeaderCriteria(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTFrom:
            {
               pCriteria->SetHeaderField("From");
               if (!_MatchesHeaderCriteria(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTTo:
            {
               pCriteria->SetHeaderField("To");
               if (!_MatchesHeaderCriteria(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTCC:
            {
               pCriteria->SetHeaderField("CC");
               if (!_MatchesHeaderCriteria(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTOn:
            {
               if (!_MatchesONCriteria(pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSentOn:
            {
               if (!_MatchesSENTONCriteria(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSentBefore:
            {
               if (!_MatchesSENTBEFORECriteria(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSentSince:
            {
               if (!_MatchesSENTSINCECriteria(fileName, pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSince:
            {
               if (!_MatchesSINCECriteria(pMessage, pCriteria))
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
               bool bSet = pMessage->GetFlagRecent() && !pMessage->GetFlagSeen();
               if (pCriteria->GetPositive() && !bSet ||
                  !pCriteria->GetPositive() && bSet)
               {
                  bMessageIsMatchingCriteria = false;
               }
               break;
            }
         case IMAPSearchCriteria::CTOld:
            {
               bool bSet = !pMessage->GetFlagRecent();
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
               if (!_MatchesBEFORECriteria(pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTLarger:
            {
               if (!_MatchesLARGERCriteria(pMessage, pCriteria))
                  bMessageIsMatchingCriteria = false;
               break;
            }
         case IMAPSearchCriteria::CTSmaller:
            {
               if (!_MatchesSMALLERCriteria(pMessage, pCriteria))
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
   IMAPCommandSEARCH::_MatchesBODYCriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      if (!m_pMessageData)
      {
         m_pMessageData = shared_ptr<MessageData>(new MessageData());
         m_pMessageData->LoadFromMessage(fileName, pMessage);
      }

      String sBody = m_pMessageData->GetBody();
      String sHTMLBody = m_pMessageData->GetHTMLBody();

      String sTextToSearchIn = sBody + sHTMLBody;
      String sTextToFind = pCriteria->GetText();

      if (sTextToSearchIn.ContainsNoCase(sTextToFind))
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::_MatchesONCriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
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
   IMAPCommandSEARCH::_MatchesSENTONCriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      String sDateHeader = _GetHeaderValue(fileName, pMessage, "Date");
      sDateHeader = Time::GetIMAPDateFromMimeHeader(sDateHeader);

      if (sDateHeader == pCriteria->GetText())
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::_MatchesSENTBEFORECriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      String sDateHeader = _GetHeaderValue(fileName, pMessage, "Date");

      DateTime dtSentDate = Time::GetDateFromMimeHeader(sDateHeader);
      DateTime dtCriteria = Time::GetDateFromIMAP(pCriteria->GetText());

      if (dtSentDate <= dtCriteria)
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::_MatchesSENTSINCECriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      String sDateHeader = _GetHeaderValue(fileName, pMessage, "Date");

      DateTime dtSentDate = Time::GetDateFromMimeHeader(sDateHeader);
      DateTime dtCriteria = Time::GetDateFromIMAP(pCriteria->GetText());

      if (dtSentDate >= dtCriteria)
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   bool
   IMAPCommandSEARCH::_MatchesSINCECriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
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
   IMAPCommandSEARCH::_MatchesBEFORECriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
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
   IMAPCommandSEARCH::_MatchesLARGERCriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
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
   IMAPCommandSEARCH::_MatchesSMALLERCriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
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
   IMAPCommandSEARCH::_MatchesTEXTCriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      if (!m_pMessageData)
      {
         m_pMessageData = shared_ptr<MessageData>(new MessageData());
         m_pMessageData->LoadFromMessage(fileName, pMessage);
      }

      String sHeader = m_pMessageData->GetHeader();
      String sBody = m_pMessageData->GetBody();

      String sTextToFind = pCriteria->GetText();

      if (pCriteria->GetPositive())
      {
         if (!sHeader.ContainsNoCase(sTextToFind) && 
             !sBody.ContainsNoCase(sTextToFind))
             return false;
      }
      else
      {
         if (sHeader.ContainsNoCase(sTextToFind) ||
             sBody.ContainsNoCase(sTextToFind))
             return false;
      }

      return true;


   }

   bool
   IMAPCommandSEARCH::_MatchesUIDCriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      vector<String> split = pCriteria->GetSequenceSet();

      bool found = IMAPListLookup::IsItemInList(split, (int) pMessage->GetUID());
      
      if (pCriteria->GetPositive())
         return found;
      else
         return !found;
   }

   bool
   IMAPCommandSEARCH::_MatchesSequenceSetCriteria(shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria, int index)
   {
      vector<String> split = pCriteria->GetSequenceSet();

      bool found = IMAPListLookup::IsItemInList(split, index);

      if (pCriteria->GetPositive())
         return found;
      else
         return !found;
   }


   bool
   IMAPCommandSEARCH::_MatchesHeaderCriteria(const String &fileName, shared_ptr<Message> pMessage, shared_ptr<IMAPSearchCriteria> pCriteria)
   {
      String sHeaderField = pCriteria->GetHeaderField();
      String sTextToFind = pCriteria->GetText();
      
      String sHeaderFieldValue = _GetHeaderValue(fileName, pMessage, sHeaderField);

      if (sHeaderFieldValue.ContainsNoCase(sTextToFind))
         return pCriteria->GetPositive();
      else
         return !pCriteria->GetPositive();
   }

   String
   IMAPCommandSEARCH::_GetHeaderValue(const String &fileName, shared_ptr<Message> pMessage, const String &sHeaderField)
   {
      if (m_pMessageData)
      {
         m_pMessageData = shared_ptr<MessageData>(new MessageData());
         m_pMessageData->LoadFromMessage(fileName, pMessage);

         return m_pMessageData->GetFieldValue(sHeaderField);
      }
      
      if (!m_pMimeHeader)
      {
         // Load header
         AnsiString sHeader = PersistentMessage::LoadHeader(fileName);

         m_pMimeHeader = shared_ptr<MimeHeader>(new MimeHeader);
         m_pMimeHeader->Load(sHeader, sHeader.GetLength(), true);
      }

      AnsiString sHeaderFieldStr = sHeaderField;
      return m_pMimeHeader->GetUnicodeFieldValue(sHeaderFieldStr);

   }

}
