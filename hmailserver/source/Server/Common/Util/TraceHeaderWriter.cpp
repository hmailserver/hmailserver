// Copyright (c) 2008 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com
// Created 2008-12-23

#include "StdAfx.h"
#include "TraceHeaderWriter.h"
#include "../BO/Message.h"
#include "../Persistence/PersistentMessage.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   TraceHeaderWriter::TraceHeaderWriter()
   {

   }

   TraceHeaderWriter::~TraceHeaderWriter(void)
   {

   }

   bool 
   TraceHeaderWriter::Write(const String &messageFileName, shared_ptr<Message> message, const vector<pair<AnsiString, AnsiString> > &headerFields)
   {
      if (headerFields.size() == 0)
         return true;

      // Add a return-path header. 
      String tempFile = messageFileName + ".tmp";

      File temporaryFile;
      if (!temporaryFile.Open(tempFile, File::OTCreate))
         return false;

      typedef pair<AnsiString, AnsiString> headerField;

      AnsiString prependString;
      boost_foreach(headerField field, headerFields)
      {
         prependString += field.first + ": " + field.second + "\r\n";
      }
      
      if (!temporaryFile.Write(prependString))
         return false;

      File messageFile;
      if (!messageFile.Open(messageFileName, File::OTReadOnly))
         return false;

      if (!temporaryFile.Write(messageFile))
         return false;

      temporaryFile.Close();
      messageFile.Close();

      return FileUtilities::Move(tempFile, messageFileName, true);

      return true;
   }

}