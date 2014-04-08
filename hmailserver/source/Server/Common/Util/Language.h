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
      String GetName() const {return m_sName; }

      bool GetIsDownloded() {return _isDownloaded; }
      static void LoadEnglish();

      bool Download();

   private:
      
      String m_sName;
      bool m_bIsLoaded;
      bool _isDownloaded;

      static std::pair<int, String> _GetString(const String &sLine);
      static void _CleanString(String &sText);
      std::map<String, String> m_mapStrings;
      
      static std::map<int, String> mapEnglishContent;
   };
}