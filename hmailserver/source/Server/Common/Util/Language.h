// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Language
   {
   public:
      Language(const String &sName, bool isDownloded);
      ~Language(void);

      void Load();
      String GetString(const String &sEnglishString);
      String GetName() const {return name_; }

      bool GetIsDownloded() {return is_downloaded_; }
      static void LoadEnglish();

      bool Download();

   private:
      
      String name_;
      bool is_loaded_;
      bool is_downloaded_;

      static std::pair<int, String> GetString_(const String &sLine);
      static void CleanString_(String &sText);
      std::map<String, String> strings_;
      
      static std::map<int, String> mapEnglishContent;
   };
}