// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class IMAPCommandArgument;

   class IMAPSimpleWord
   {
   public:
      IMAPSimpleWord();
      virtual ~IMAPSimpleWord();

      bool Quoted();
      void Quoted(bool bNewVal) {m_bIsQuoted = bNewVal; }

      bool Paranthezied();
      void Paranthezied(bool bNewVal) {m_bIsParanthezied = bNewVal; }

      bool Clammerized ();
      void Clammerized(bool bNewVal) {m_bIsClammerized = bNewVal; }

      String Value();
      void Value(const String &sNewVal);

      String LiteralData() {return m_sLiteralData;}
      void LiteralData(const String &sNewVal) {m_sLiteralData = sNewVal; }

   private:
      String m_sWord;
      String m_sLiteralData;

      bool m_bIsQuoted;
      bool m_bIsParanthezied;
      bool m_bIsClammerized;

   };

   class IMAPSimpleCommandParser  
   {
   public:
	   IMAPSimpleCommandParser();
	   virtual ~IMAPSimpleCommandParser();

      void Parse(shared_ptr<IMAPCommandArgument> pArgument);
      int WordCount() {return m_vecParsedWords.size(); }
      int ParamCount() {return m_vecParsedWords.size() - 1; }
      
      String GetParamValue(shared_ptr<IMAPCommandArgument> pArguments, int iParamIndex);

      shared_ptr<IMAPSimpleWord> Word(int iIndex) {return m_vecParsedWords[iIndex]; }

      shared_ptr<IMAPSimpleWord> QuotedWord();
      shared_ptr<IMAPSimpleWord> ParantheziedWord();
      shared_ptr<IMAPSimpleWord> ClammerizedWord();

      void AddWord(shared_ptr<IMAPSimpleWord> pWord) {m_vecParsedWords.push_back(pWord); }

      void RemoveWord(int iWordIdx);

      void UnliteralData();

   private:


      int _FindEndOfQuotedString(const String &sInputString, int iWordStartPos);
      std::vector<shared_ptr<IMAPSimpleWord> > m_vecParsedWords;

      bool _Validate(const String &command);

   };

   class IMAPSimpleCommandParserTester
   {
   public:
      IMAPSimpleCommandParserTester();

      bool Test();
   };

}
;