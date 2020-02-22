// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class IMAPSortParser;
   class Message;
   class MessageMetaData;
   class IMAPConnection;

   class IMAPSort
   {
   public:
      IMAPSort(void);
      ~IMAPSort(void);

      enum SortField
      {
         Unknown = 0,
         From = 1,
         Subject = 2,
         CC = 3,
         To = 4,
         Date = 5,
         Arrival = 6,
         Size = 7

      };

      void Sort(std::shared_ptr<IMAPConnection> pConnection, std::vector<std::pair<int, std::shared_ptr<Message> > > &vecMessages, String character_set, std::shared_ptr<IMAPSortParser> pParser);

   private:

      void CacheHeaderFields_(std::shared_ptr<IMAPConnection> pConnection, const std::vector<std::pair<int, std::shared_ptr<Message> > > &vecMessages, const std::map<__int64, String > &databaseMetaData, SortField &sortField, std::map<__int64, String> &mapHeaderFields);

      SortField GetSortField_(AnsiString sHeaderField);
   };
}