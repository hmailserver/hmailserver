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

   private:
      
      String name_;
      bool is_loaded_;
      bool is_downloaded_;

      static std::pair<int, String> GetString_(const String &sLine);
      static void CleanString_(String &sText);
      static void ReportMissingString_(const String &sEnglishString);
      std::map<String, String> strings_;
      
      static std::map<int, String> mapEnglishContent;

      // The values of mapEnglishContent, so that a lookup miss can be classified
      // without searching that map by value on every miss.
      static std::set<String> english_strings_;

      // Strings already reported, so that each one is logged once per process
      // instead of every time the user interface re-reads the pane it is on.
      static std::set<String> reported_missing_strings_;
      static boost::recursive_mutex missing_string_mutex_;
   };
}