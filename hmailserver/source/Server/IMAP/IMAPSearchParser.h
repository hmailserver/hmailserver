// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class IMAPCommandArgument;
   class IMAPSimpleCommandParser;
   class IMAPSortParser;

   class IMAPSearchCriteria
   {
   public:

      enum CriteriaType
      {
         CTUnknown = 0,
         CTHeader = 1,
         CTUID = 4,
         CTDeleted = 5,
         CTRecent = 8,
         CTUndeleted = 10,
         CTSeen = 11,
         CTUnseen = 12,
         CTText = 13,
         CTAll = 14,
         CTBody = 15,
         CTSentOn = 16,
         CTSentBefore = 17,
         CTSentSince = 18,
         CTSubject = 19,
         CTFrom = 20,
         CTTo = 21,
         CTCC = 22,
         CTSince = 23,
         
            
         CTAnswered = 24,
         CTBefore = 25,
         CTDraft = 26,
         CTFlagged = 27,
         CTLarger = 28,
         CTNew = 29,
         CTOld = 30,
         CTSmaller = 31,
         CTUnanswered = 32,
         CTUndraft = 33,
         CTUnflagged = 34,
         CTOn = 35,

         CTSequenceSet = 36,

         CTCharset = 40,

         CTSubCriteria = 100
      };

      IMAPSearchCriteria() : 
         m_lUIDLower(-1),
         m_lUIDUpper(-1),
         m_bPositive(true),
         m_bIsOr(false),
         m_Type(CTUnknown) {};

      long GetUIDLower() {return m_lUIDLower; }
      void SetUIDLower(long lNewVal) {m_lUIDLower = lNewVal; }

      long GetUIDUpper() {return m_lUIDUpper; }
      void SetUIDUpper(long lNewVal) {m_lUIDUpper = lNewVal; }

      String GetText() {return m_sText; }
      void SetText(const String &sText) {m_sText = sText; }

      bool GetPositive() {return m_bPositive; }
      void SetPositive(bool bNewVal) {m_bPositive = bNewVal; }

      bool GetIsOR() {return m_bIsOr; }
      void SetIsOR(bool bNewVal) {m_bIsOr = bNewVal; }

      void SetHeaderField(const String &sField) {m_sHeaderField = sField;}
      String GetHeaderField() {return m_sHeaderField;}

      CriteriaType GetType() {return m_Type;}
      void SetType(CriteriaType newVal) {m_Type = newVal; }

      static CriteriaType GetCriteriaTypeByName(const String &sName);

      vector<shared_ptr<IMAPSearchCriteria> > &GetSubCriterias() {return m_vecSubCriterias;}

      void SetSequenceSet(vector<String> newVal) {_sequenceSet = newVal;}
      vector<String> &GetSequenceSet() {return _sequenceSet;}

   private:

      static bool _IsSequenceSet(const String &item);

      long m_lUIDLower;
      long m_lUIDUpper;
      String m_sText;
      bool m_bPositive;
      CriteriaType m_Type;

      String m_sHeaderField;
      vector<shared_ptr<IMAPSearchCriteria> > m_vecSubCriterias;
      vector<String> _sequenceSet;

      bool m_bIsOr;
   };


   class IMAPSearchParser  
   {
   public:
	   IMAPSearchParser();
	   virtual ~IMAPSearchParser();

      IMAPResult ParseCommand(shared_ptr<IMAPCommandArgument> pArgument, bool bIsSort);

      shared_ptr<IMAPSearchCriteria>  GetCriteria() {return m_pResultCriteria;}
      shared_ptr<IMAPSortParser> GetSortParser() {return m_pSortParser; }

      String GetCharsetName() 
      {
         return _charsetName; 
      }

   private:

      bool _IsValidCharset(const String &charsetName);
      bool _NeedsDecoding(IMAPSearchCriteria::CriteriaType criteriaType);
      String _DecodeWordAccordingToCharset(const String &inputValue);
      
      IMAPResult _ParseSegment(shared_ptr<IMAPSimpleCommandParser> pSimpleParser, int &currentWord, shared_ptr<IMAPSearchCriteria> pCriteria, int iRecursion);

      IMAPResult _ParseWord(shared_ptr<IMAPSimpleCommandParser> pSimpleParser, shared_ptr<IMAPSearchCriteria> pNewCriteria, int &iCurrentWord);

      shared_ptr<IMAPSortParser> m_pSortParser;
      shared_ptr<IMAPSearchCriteria> m_pResultCriteria;

      String _charsetName;
   };

}