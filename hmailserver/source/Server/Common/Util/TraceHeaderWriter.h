// Copyright (c) 2008 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2008-12-23

#pragma once

namespace HM
{
   class Message;

   class TraceHeaderWriter
   {
   public:
      TraceHeaderWriter();
      ~TraceHeaderWriter();

      bool Write(const String &messageFileName, std::shared_ptr<Message> message, const std::vector<std::pair<AnsiString, AnsiString> > &headerFields);
   };
}
