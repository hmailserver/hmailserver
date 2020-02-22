// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class DatabaseSettings;

   class SQLScriptParser  
   {
   public:
      SQLScriptParser(std::shared_ptr<DatabaseSettings> pSettings, const String &sFile);
      virtual ~SQLScriptParser();

      bool Parse(String &sErrorMessage);

      int GetNoOfCommands() {return (int)commands_.size(); }
      String GetCommand(int index) {return commands_[index]; }

   private:

      bool PreprocessLine_(String &sLine);

      std::shared_ptr<DatabaseSettings> settings_;
      String file_;
      std::vector<String> commands_;

   };

}
