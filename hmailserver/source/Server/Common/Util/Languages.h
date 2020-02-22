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

      std::shared_ptr<Language> GetLanguage(const String &sLanguage);
      std::shared_ptr<Language> GetLanguage(int index);
      size_t GetCount() { return languages_.size(); }

     
   private:

      bool IsValidLangauge_(const String &sLanguage) const;

      std::map<String, std::shared_ptr<Language> > languages_;


      
   };
}