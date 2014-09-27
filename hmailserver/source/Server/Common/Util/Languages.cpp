// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "Languages.h"
#include "Language.h"
#include "FileInfo.h"


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

      std::vector<FileInfo> languageFiles = 
         FileUtilities::GetFilesInDirectory(IniFileSettings::Instance()->GetLanguageDirectory(), "*.ini");

      auto iter = languageFiles.begin();
      auto iterEnd = languageFiles.end();


      for (; iter != iterEnd; iter++)
      {
         FileInfo fileInfo = (*iter);
         String sFileName = fileInfo.GetName();

         int dotPos = sFileName.Find(_T("."));
         if (dotPos <= 0)
            continue;

         String sLanguage = sFileName.Mid(0, dotPos);
         String sFormattedLanguage = sLanguage;
         sFormattedLanguage.ToLower();

         if (!IsValidLangauge_(sFormattedLanguage))
            continue;

         std::shared_ptr<Language> pLanguage = std::shared_ptr<Language>(new Language(sFormattedLanguage, true));
         languages_[sFormattedLanguage] = pLanguage;
      }
   }

   bool 
   Languages::IsValidLangauge_(const String &sLanguage) const
   {
      std::vector<String> validLanguages = IniFileSettings::Instance()->GetValidLanguages();
      std::vector<String>::iterator iter = validLanguages.begin();
      std::vector<String>::iterator iterEnd = validLanguages.end();

      for (; iter != iterEnd; iter++)
      {
         String validLanguage = (*iter);

         if (validLanguage.CompareNoCase(sLanguage) == 0)
            return true;
      }

      return false;

   }

   std::shared_ptr<Language> 
   Languages::GetLanguage(const String &sLanguage)
   {
      String sFormattedLanguage = sLanguage;
      sFormattedLanguage.ToLower();

      std::map<String, std::shared_ptr<Language> >::iterator iterLanguage = languages_.find(sFormattedLanguage);
      if (iterLanguage != languages_.end())
         return (*iterLanguage).second;
     
      std::shared_ptr<Language> pEmpty;
      return pEmpty;
   }

   std::shared_ptr<Language> 
   Languages::GetLanguage(int index)
   {
      std::map<String, std::shared_ptr<Language> >::iterator iter = languages_.begin();
      std::map<String, std::shared_ptr<Language> >::iterator iterEnd = languages_.end();
      
      int current = 0;
      for (; iter != iterEnd; iter++)
      {
         if (current == index)
         {
            return (*iter).second;
         }

         current++;
      }

      std::shared_ptr<Language> empty;
      return empty;
   }
}