// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "IMAPSort.h"
#include "IMAPSortParser.h"
#include "IMAPConnection.h"

#include "..\Common\Persistence\PersistentMessageMetaData.h"
#include "..\Common\BO\Message.h"
#include "..\Common\MIME\Mime.h"
#include "..\Common\Util\Time.h"
#include "..\Common\Util\VariantDateTime.h"
#include "../Common/Persistence/PersistentMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPSort::IMAPSort(void) 
   {
   }

   IMAPSort::~IMAPSort(void)
   {
   }
 
   class IMAPSortSize {
   public:
      //Return true if s1 < s2; otherwise, return false.
      bool operator()(const std::pair<int, std::shared_ptr<Message> > p1, const std::pair<int, std::shared_ptr<Message> >  p2)
      {
         return p1.second->GetSize() < p2.second->GetSize();
      }
   };   

   class IMAPSortCachedDateTime {
   public:
      IMAPSortCachedDateTime(std::map<__int64, DateTime> &map) :
         date_time_info_(map)
      {
      }

      //Return true if s1 < s2; otherwise, return false.
      bool operator()(const std::pair<int, std::shared_ptr<Message> > p1, const std::pair<int, std::shared_ptr<Message> >  p2)
      {
         DateTime dt1 = date_time_info_[p1.second->GetID()];
         DateTime dt2 = date_time_info_[p2.second->GetID()];
         return (dt1 < dt2) == TRUE;
      }

   private:
      std::map<__int64, DateTime> &date_time_info_;
   };   

   class IMAPSortHeaderField 
   {
   public:
      IMAPSortHeaderField(std::map<__int64, String> &mapHeaderFields, String character_set) :
         _mapHeaderFields(mapHeaderFields),
         character_set_(character_set)
      {
         
      }

      //Return true if s1 < s2; otherwise, return false.
      bool operator()(const std::pair<int, std::shared_ptr<Message> > p1, const std::pair<int, std::shared_ptr<Message> >  p2)
      {
         String sHeader1 = _mapHeaderFields[p1.second->GetID()];
         String sHeader2 = _mapHeaderFields[p2.second->GetID()];
         
         int compare_result = 0;

         if (character_set_.CompareNoCase(_T("US-ASCII")) == 0)
         {
            AnsiString ansi_1 = sHeader1;
            AnsiString ansi_2 = sHeader2;

            return ansi_1 < ansi_2;
         }
         else
         {
            // Use UTF8 as default.
            compare_result = CompareStringW(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, sHeader1, -1, sHeader2, -1);
         }

         if (compare_result == 0)
            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5019, "IMAPSortHeaderField::operator()", "An error occurred while sorting. Check system locale settings.");

         if (compare_result == CSTR_LESS_THAN)
            return true;
         else
            return false;  
      }
   private:

      std::map<__int64, String> &_mapHeaderFields;
      String character_set_;
   };   
  


   void 
   IMAPSort::Sort(std::shared_ptr<IMAPConnection> pConnection, std::vector<std::pair<int, std::shared_ptr<Message> > > &vecMessages, String character_set, std::shared_ptr<IMAPSortParser> pParser)
   {
      // Sort messages according to the sort criteria

      std::vector<std::pair<bool,String> > vecSortTypes = pParser->GetSortTypes();

      auto iterSortType = vecSortTypes.begin();
      // Skip to the last.
      while (iterSortType + 1 != vecSortTypes.end())
         iterSortType++;

      if (iterSortType != vecSortTypes.end())
      {
         std::pair<bool,String> st = (*iterSortType);

         String sHeaderField = st.second;
         SortField sortField = GetSortField_(sHeaderField);

         PersistentMessageMetaData messageMetaData;

         std::map<__int64, String > databaseMetaData;

         bool loadDBCache =  
            (sortField == From ||
            sortField == Subject ||
            sortField == To ||
            sortField == CC ||
            sortField == Date) && 
            Configuration::Instance()->GetMessageIndexing();

         if (loadDBCache && vecMessages.size() > 0)
         {
            int accountID = (int) vecMessages[0].second->GetAccountID();
            int folderID = (int) vecMessages[0].second->GetFolderID();

            messageMetaData.GetMetaData(accountID, folderID, sHeaderField, databaseMetaData);
         }

         std::map<__int64, String> mapHeaderFields;
         
         if (sortField == Subject ||
             sortField == CC ||
             sortField == From ||
             sortField == To ||
             sortField == Date)
         {
            CacheHeaderFields_(pConnection, vecMessages, databaseMetaData, sortField, mapHeaderFields);
         }
         
         if (sortField == Arrival) 
         {
            // The message are already sorted on arrival date, since we're doing
            // a sort by messageid when we read the message list from the database.
            // Hence no reason to resort ...
            // Or may be not.
            // We can't assume that the order in the database is the same as the
            // arrival date. For example, if a message is copied from one folder
            // to another, the message receives a higher db id, but the arrival 
            // date is still the same.

            // Cache dates for all messages.
            std::map<__int64, DateTime> mapDateTimeInfo;
            auto iterMessage = vecMessages.begin();
            while (iterMessage != vecMessages.end())
            {
               // Fetch message.
               std::shared_ptr<Message> p1 = (*iterMessage).second;

               // Retrieve the message date and convert it to a DateTime.
               DateTime dt1 = Time::GetDateFromSystemDate(p1->GetCreateTime());

               // Cache the date connected to the message.
               mapDateTimeInfo[p1->GetID()] = dt1;

               // Continue with the next message.
               iterMessage++;
            }
         
            std::sort(vecMessages.begin(), vecMessages.end(), IMAPSortCachedDateTime(mapDateTimeInfo));

         }
         else if (sortField == Size)
         {
            std::sort(vecMessages.begin(), vecMessages.end(), IMAPSortSize());
         }
         else if (sortField == Subject ||
                  sortField == CC ||
                  sortField == From ||
                  sortField == To)
         {
            std::sort(vecMessages.begin(), vecMessages.end(), IMAPSortHeaderField(mapHeaderFields, character_set));
         }
         else if (sortField == Date)
         {
            // Cache dates for all messages. This is faster than sorting the strings.
            std::map<__int64, DateTime> mapDateTimeInfo;
            auto iterMessage = vecMessages.begin();
            
            while (iterMessage != vecMessages.end())
            {
               // Fetch message.
               std::shared_ptr<Message> p1 = (*iterMessage).second;
               String headerValue = mapHeaderFields[p1->GetID()];
               
               DateTime dt1 = Time::GetDateFromSystemDate(headerValue);

               // Cache the date connected to the message.
               mapDateTimeInfo[p1->GetID()] = dt1;

               // Continue with the next message.
               iterMessage++;
            }

            std::sort(vecMessages.begin(), vecMessages.end(), IMAPSortCachedDateTime(mapDateTimeInfo));
         }


         if (st.first == false && sortField != Unknown)
         {
            // Reversed order:
            std::reverse(vecMessages.begin(), vecMessages.end());
         }
         

         iterSortType++;
      }
   }

   IMAPSort::SortField 
   IMAPSort::GetSortField_(AnsiString sHeaderField)
   {
      SortField sortField;

      sHeaderField.MakeUpper();

      if (sHeaderField == "SUBJECT")
         sortField = Subject;
      else if (sHeaderField == "CC")
         sortField = CC;
      else if (sHeaderField == "DATE")
         sortField = Date;
      else if (sHeaderField == "FROM")
         sortField = From;
      else if (sHeaderField == "TO")
         sortField = To;
      else if (sHeaderField == "ARRIVAL")
         sortField = Arrival;
      else if (sHeaderField == "SIZE")
         sortField = Size;
      else
         sortField = Unknown;

      return sortField;
   }


   void 
   IMAPSort::CacheHeaderFields_(std::shared_ptr<IMAPConnection> pConnection,
                                const std::vector<std::pair<int, std::shared_ptr<Message> > > &vecMessages, 
                                const std::map<__int64, String > &databaseMetaData, 
                                SortField &sortField,
                                std::map<__int64, String> &mapHeaderFields)
   {
      mapHeaderFields.clear();

      String sHeaderField;

      switch (sortField)
      {
      case From:
         sHeaderField = "From";
         break;
      case Subject:
         sHeaderField = "Subject";
         break;
      case To:
         sHeaderField = "To";
         break;
      case CC:
         sHeaderField = "CC";
         break;
      case Date:
         sHeaderField = "Date";
         break;      
      }

      // Cache the header field for all messages.
      bool bTrimLeadingSpecialCharacters = (sortField == From || sortField == To || sortField == CC);

      std::vector<std::pair<int, std::shared_ptr<Message> > >::const_iterator iter = vecMessages.begin();
      std::vector<std::pair<int, std::shared_ptr<Message> > >::const_iterator iterEnd = vecMessages.end();

      for (; iter != iterEnd; iter++)
      {
         // Fetch message.
         const std::shared_ptr<Message> p1 = (*iter).second;

         String sFieldValue = "";

         // Read message and parse out the value from the header.
         std::map<__int64, String >::const_iterator dbMetaIter = databaseMetaData.find(p1->GetID());
         
         if (dbMetaIter == databaseMetaData.end())
         {
            String fileName = PersistentMessage::GetFileName(pConnection->GetAccount(), p1);

            AnsiString sHeader = PersistentMessage::LoadHeader(fileName);

            MimeHeader oHeader;oHeader.Load(sHeader, sHeader.GetLength());
            sFieldValue = oHeader.GetUnicodeFieldValue(sHeaderField);

            if (sortField == Date)
            {
               DateTime dt = Time::GetDateTimeFromMimeHeader(sFieldValue);
               sFieldValue = Time::GetTimeStampFromDateTime(dt);
            }
         }
         else
         {
            sFieldValue = (*dbMetaIter).second;
         }

         if (sortField == Date && sFieldValue.IsEmpty())
         {
             /*
              * RFC 5256 "2.2. Sent Date" chapter. If the sent date cannot be determined (a Date: header is missing or cannot be parsed), 
              * the INTERNALDATE for that message is used as the sent date.
              */
            sFieldValue = p1->GetCreateTime();
         }

         // Convert the string to upper case. The sorting should be
         // case insensitive. Otherwise b will come before A.
         sFieldValue.ToUpper();

         // Remove leading ". If we sort on From, "Test" should come in 
         // same position as Test. (quotes / no quotes)
         if (bTrimLeadingSpecialCharacters)
         {
            sFieldValue.TrimLeft(_T("\""));
            sFieldValue.TrimLeft(_T("<"));
         }

         // Cache the header
         mapHeaderFields[p1->GetID()] = sFieldValue;
      }
   }
}