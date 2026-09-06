// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Language.h"

#pragma warning (disable: 4566)

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   std::map<int, String> Language::mapEnglishContent;
   std::set<String> Language::english_strings_;
   std::set<String> Language::reported_missing_strings_;
   boost::recursive_mutex Language::missing_string_mutex_;

   Language::Language(const String &sName, bool isDownloded) :
      is_loaded_(false),
      name_(sName),
      is_downloaded_(isDownloded)
   {
   }

   Language::~Language(void)
   {
   }

   String
   Language::GetString(const String &sEnglishString)
   {
      if (!is_loaded_)
         Load();

      std::map<String, String>::const_iterator iterString = strings_.find(sEnglishString);
      if (iterString == strings_.end())
      {
#ifdef DEBUG
         ReportMissingString_(sEnglishString);
#endif
         return sEnglishString;
      }
      else
      {
         String translatedString = (*iterString).second;

         if (translatedString.IsEmpty())
            return sEnglishString;

         return translatedString;
      }
   }

   void 
   Language::LoadEnglish()
   {
      String sEnglishLanguageFile = IniFileSettings::Instance()->GetLanguageDirectory() + "\\english.ini";
      String sEnglishContents = FileUtilities::ReadCompleteTextFile(sEnglishLanguageFile);
      
      
      std::vector<String> vecEnglishStrings = StringParser::SplitString(sEnglishContents, "\r\n");

      auto iterEnglishString = vecEnglishStrings.begin();
      auto iterEnglishStringEnd = vecEnglishStrings.end();
      for (; iterEnglishString != iterEnglishStringEnd; iterEnglishString++)
      {
         String sString = (*iterEnglishString);

         if (sString.Left(7) != _T("String_"))
            continue;

         std::pair<int, String> pair = GetString_(sString);

         mapEnglishContent[pair.first] = pair.second;
      }

      // Rebuilt from the map rather than filled in the loop above, so that the two
      // cannot drift apart if LoadEnglish is called more than once.
      boost::lock_guard<boost::recursive_mutex> guard(missing_string_mutex_);

      english_strings_.clear();

      for (const auto &englishString : mapEnglishContent)
         english_strings_.insert(englishString.second);
   }

   void
   Language::ReportMissingString_(const String &sEnglishString)
   {
      // A lookup misses for two different reasons. Either the string is not defined in
      // english.ini at all, in which case it can never be translated, in any language,
      // and whoever added it to the user interface has to add it here as well. Or it is
      // defined but the language file being used has no translation for it yet, which is
      // ordinary and would report most of the file for a partly translated language.
      // Only the first is worth reporting.
      boost::lock_guard<boost::recursive_mutex> guard(missing_string_mutex_);

      if (english_strings_.find(sEnglishString) != english_strings_.end())
         return;

      if (!reported_missing_strings_.insert(sEnglishString).second)
         return;

      LOG_DEBUG("Language::GetString - The user interface asked for the string \"" + sEnglishString +
                "\" which is not defined in english.ini. It is shown in English in every language.");
   }

   void 
   Language::Load()
   {
      String sTranslatedLanguageFile = IniFileSettings::Instance()->GetLanguageDirectory() + "\\" + name_ + ".ini";
      String sTranslatedContents = FileUtilities::ReadCompleteTextFile(sTranslatedLanguageFile);
      

      std::vector<String> vecTranslatedStrings = StringParser::SplitString(sTranslatedContents, "\r\n");

      auto iterTranslatedString = vecTranslatedStrings.begin();
      auto iterTranslatedStringEnd = vecTranslatedStrings.end();
      for (; iterTranslatedString != iterTranslatedStringEnd; iterTranslatedString++)
      {
         String sString = (*iterTranslatedString);
         std::pair<int, String> translatedPair = GetString_(sString);

         auto englishPair = mapEnglishContent.find(translatedPair.first);
         if (englishPair != mapEnglishContent.end())
         {
            strings_[(*englishPair).second] = translatedPair.second;
         }
        
      }

      is_loaded_ = true;
   }

   std::pair<int, String> 
   Language::GetString_(const String &sLine)
   {
      int iValueStart = 7;
      int iEqualsPos = sLine.Find(_T("="));
      int iValueLen = iEqualsPos - iValueStart;

      String sValue = sLine.Mid(iValueStart, iValueLen);
      String sText = sLine.Mid(iEqualsPos+1);

      int iValue = _ttoi(sValue);

      return std::make_pair(iValue, sText);
   }

   void 
   Language::CleanString_(String &sText) 
   {
      // Remove lading "&"
      if (sText.Left(1) == _T("&"))
         sText = sText.Mid(1);

      // Remove trailing "..."
      if (sText.Right(3) == _T("..."))
         sText = sText.Left(sText.GetLength() - 3);

   }
}