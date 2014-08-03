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
         uidlower_(-1),
         uidupper_(-1),
         positive_(true),
         is_or_(false),
         type_(CTUnknown) {};

      long GetUIDLower() {return uidlower_; }
      void SetUIDLower(long lNewVal) {uidlower_ = lNewVal; }

      long GetUIDUpper() {return uidupper_; }
      void SetUIDUpper(long lNewVal) {uidupper_ = lNewVal; }

      String GetText() {return text_; }
      void SetText(const String &sText) {text_ = sText; }

      bool GetPositive() {return positive_; }
      void SetPositive(bool bNewVal) {positive_ = bNewVal; }

      bool GetIsOR() {return is_or_; }
      void SetIsOR(bool bNewVal) {is_or_ = bNewVal; }

      void SetHeaderField(const String &sField) {header_field_ = sField;}
      String GetHeaderField() {return header_field_;}

      CriteriaType GetType() {return type_;}
      void SetType(CriteriaType newVal) {type_ = newVal; }

      static CriteriaType GetCriteriaTypeByName(const String &sName);

      vector<shared_ptr<IMAPSearchCriteria> > &GetSubCriterias() {return sub_criterias_;}

      void SetSequenceSet(vector<String> newVal) {_sequenceSet = newVal;}
      vector<String> &GetSequenceSet() {return _sequenceSet;}

   private:

      static bool IsSequenceSet_(const String &item);

      long uidlower_;
      long uidupper_;
      String text_;
      bool positive_;
      CriteriaType type_;

      String header_field_;
      vector<shared_ptr<IMAPSearchCriteria> > sub_criterias_;
      vector<String> _sequenceSet;

      bool is_or_;
   };


   class IMAPSearchParser  
   {
   public:
	   IMAPSearchParser();
	   virtual ~IMAPSearchParser();

      IMAPResult ParseCommand(shared_ptr<IMAPCommandArgument> pArgument, bool bIsSort);

      shared_ptr<IMAPSearchCriteria>  GetCriteria() {return result_criteria_;}
      shared_ptr<IMAPSortParser> GetSortParser() {return sort_parser_; }

      String GetCharsetName() 
      {
         return _charsetName; 
      }

   private:

      bool IsValidCharset_(const String &charsetName);
      bool NeedsDecoding_(IMAPSearchCriteria::CriteriaType criteriaType);
      String DecodeWordAccordingToCharset_(const String &inputValue);
      
      IMAPResult ParseSegment_(shared_ptr<IMAPSimpleCommandParser> pSimpleParser, int &currentWord, shared_ptr<IMAPSearchCriteria> pCriteria, int iRecursion);

      IMAPResult ParseWord_(shared_ptr<IMAPSimpleCommandParser> pSimpleParser, shared_ptr<IMAPSearchCriteria> pNewCriteria, int &iCurrentWord);

      shared_ptr<IMAPSortParser> sort_parser_;
      shared_ptr<IMAPSearchCriteria> result_criteria_;

      String _charsetName;
   };

}