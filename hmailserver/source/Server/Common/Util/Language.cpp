// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "HTTPClient.h"
#include "Unicode.h"
#include "Charset.h"

#include "Language.h"

#pragma warning (disable: 4566)

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   std::map<int, String> Language::mapEnglishContent;

   Language::Language(const String &sName, bool isDownloded) :
      m_bIsLoaded(false),
      m_sName(sName),
      _isDownloaded(isDownloded)
   {
   }

   Language::~Language(void)
   {
   }

   String
   Language::GetString(const String &sEnglishString)
   {
      if (!m_bIsLoaded)
         Load();

      std::map<String, String>::const_iterator iterString = m_mapStrings.find(sEnglishString);
      if (iterString == m_mapStrings.end())
         return sEnglishString;
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

      std::vector<String>::iterator iterEnglishString = vecEnglishStrings.begin();
      std::vector<String>::iterator iterEnglishStringEnd = vecEnglishStrings.end();
      for (; iterEnglishString != iterEnglishStringEnd; iterEnglishString++)
      {
         String sString = (*iterEnglishString);

         if (sString.Left(7) != _T("String_"))
            continue;

         std::pair<int, String> pair = _GetString(sString);

         mapEnglishContent[pair.first] = pair.second;
      }

   }

   void 
   Language::Load()
   {
      String sTranslatedLanguageFile = IniFileSettings::Instance()->GetLanguageDirectory() + "\\" + m_sName + ".ini";
      String sTranslatedContents = FileUtilities::ReadCompleteTextFile(sTranslatedLanguageFile);
      

      std::vector<String> vecTranslatedStrings = StringParser::SplitString(sTranslatedContents, "\r\n");

      std::vector<String>::iterator iterTranslatedString = vecTranslatedStrings.begin();
      std::vector<String>::iterator iterTranslatedStringEnd = vecTranslatedStrings.end();
      for (; iterTranslatedString != iterTranslatedStringEnd; iterTranslatedString++)
      {
         String sString = (*iterTranslatedString);
         std::pair<int, String> translatedPair = _GetString(sString);

         std::map<int, String>::iterator englishPair = mapEnglishContent.find(translatedPair.first);
         if (englishPair != mapEnglishContent.end())
         {
            m_mapStrings[(*englishPair).second] = translatedPair.second;
         }
        
      }

      m_bIsLoaded = true;
   }

   std::pair<int, String> 
   Language::_GetString(const String &sLine)
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
   Language::_CleanString(String &sText) 
   {
      // Remove lading "&"
      if (sText.Left(1) == _T("&"))
         sText = sText.Mid(1);

      // Remove trailing "..."
      if (sText.Right(3) == _T("..."))
         sText = sText.Left(sText.GetLength() - 3);

   }



   bool
   Language::Download()
   {
      AnsiString output;
      HTTPClient client;
      
      bool result = client.ExecuteScript("www.hmailserver.com", "/devnet/translation_getlanguage.php?language=" + m_sName, output);

      String unicodeString = output;
      Unicode::MultiByteToWide(output, unicodeString);

      FileUtilities::WriteToFile("C:\\test.txt", unicodeString, true);

      

      return result;
   }
}