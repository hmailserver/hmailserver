// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ProcessLauncher
   {
   public:
      ProcessLauncher(const String &commandLine, const String &workingDirectory);
      ProcessLauncher(const String &commandLine);
      ~ProcessLauncher(void);

      bool Launch(unsigned int &exitCode);

      void SetErrorLogTimeout(unsigned int milliseconds);

   private:

      unsigned int error_log_timeout_;

      String command_line_;
      String working_directory_;
   };
}