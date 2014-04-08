// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "Languages.h"
#include "Language.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Languages::Languages(void)
   {
      
   }

   Languages::~Languages(void)
   {
   }

   void 
   Languages::Load()
   {
      Language::LoadEnglish();

      vector<String> languageFiles = 
         FileUtilities::GetFilesInDirectory(IniFileSettings::Instance()->GetLanguageDirectory());

      vector<String>::iterator iter = languageFiles.begin();
      vector<String>::iterator iterEnd = languageFiles.end();

      for (; iter != iterEnd; iter++)
      {
         String sFileName = (*iter);

         int dotPos = sFileName.Find(_T("."));
         if (dotPos <= 0)
            continue;

         String sLanguage = sFileName.Mid(0, dotPos);
         String sFormattedLanguage = sLanguage;
         sFormattedLanguage.ToLower();

         if (!_IsValidLangauge(sFormattedLanguage))
            continue;

         shared_ptr<Language> pLanguage = shared_ptr<Language>(new Language(sFormattedLanguage, true));
         m_mapLanguages[sFormattedLanguage] = pLanguage;
      }
      
      /*
      vector<String> validLanguages = IniFileSettings::Instance()->GetValidLanguages();
      boost_foreach(String language, validLanguages)
      {
         language.ToLower();

         if (m_mapLanguages.find(language) == m_mapLanguages.end())
         {
            shared_ptr<Language> pLanguage = shared_ptr<Language>(new Language(language, false));
            m_mapLanguages[language] = pLanguage;
         }
      }
      */

   }

   bool 
   Languages::_IsValidLangauge(const String &sLanguage) const
   {
      vector<String> validLanguages = IniFileSettings::Instance()->GetValidLanguages();
      vector<String>::iterator iter = validLanguages.begin();
      vector<String>::iterator iterEnd = validLanguages.end();

      for (; iter != iterEnd; iter++)
      {
         String validLanguage = (*iter);

         if (validLanguage.CompareNoCase(sLanguage) == 0)
            return true;
      }

      return false;

   }

   shared_ptr<Language> 
   Languages::GetLanguage(const String &sLanguage)
   {
      String sFormattedLanguage = sLanguage;
      sFormattedLanguage.ToLower();

      map<String, shared_ptr<Language> >::iterator iterLanguage = m_mapLanguages.find(sFormattedLanguage);
      if (iterLanguage != m_mapLanguages.end())
         return (*iterLanguage).second;
     
      shared_ptr<Language> pEmpty;
      return pEmpty;
   }

   shared_ptr<Language> 
   Languages::GetLanguage(int index)
   {
      map<String, shared_ptr<Language> >::iterator iter = m_mapLanguages.begin();
      map<String, shared_ptr<Language> >::iterator iterEnd = m_mapLanguages.end();
      
      int current = 0;
      for (; iter != iterEnd; iter++)
      {
         if (current == index)
         {
            return (*iter).second;
         }

         current++;
      }

      shared_ptr<Language> empty;
      return empty;
   }
}