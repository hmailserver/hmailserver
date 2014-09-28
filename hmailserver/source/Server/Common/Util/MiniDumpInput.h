// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MiniDumpInput
   {
   public:
      int ProcessId;
      int ThreadId;
      EXCEPTION_RECORD ExceptionRecord;
      CONTEXT ContextRecord;

      wchar_t DumpFile[2048];

      static const std::string SharedMemoryName;
   };

   
}