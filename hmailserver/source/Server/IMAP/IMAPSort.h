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

      void Sort(shared_ptr<IMAPConnection> pConnection, vector<pair<int, shared_ptr<Message> > > &vecMessages, shared_ptr<IMAPSortParser> pParser);

   private:

      void _CacheHeaderFields(shared_ptr<IMAPConnection> pConnection, const vector<pair<int, shared_ptr<Message> > > &vecMessages, const map<__int64, String > &databaseMetaData, SortField &sortField, std::map<__int64, String> &mapHeaderFields);

      SortField _GetSortField(AnsiString sHeaderField);
   };
}