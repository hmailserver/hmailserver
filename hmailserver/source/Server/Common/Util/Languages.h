// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Language;

   class Languages : public Singleton<Languages>
   {
   public:
      Languages(void);
      ~Languages(void);

      void Load();

      shared_ptr<Language> GetLanguage(const String &sLanguage);
      shared_ptr<Language> GetLanguage(int index);
      int GetCount() {return m_mapLanguages.size(); }

     
   private:

      bool _IsValidLangauge(const String &sLanguage) const;

      map<String, shared_ptr<Language> > m_mapLanguages;


      
   };
}