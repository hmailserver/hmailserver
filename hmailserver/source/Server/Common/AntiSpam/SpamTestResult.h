// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SpamTestResult
   {
   public:

      enum Result
      {
         Neutral = 0,
         Fail = 1,
         Pass = 2
      };

      SpamTestResult(const String &testName, Result result, int iSpamScore, const String &sMessage);
      virtual ~SpamTestResult();

      Result GetResult() {return result_; }
      int GetSpamScore() {return spam_score_; }
      String GetMessage() {return message_; }
      String GetTestName() {return testName_; }

      // Set when a test wants the message rejected or marked as spam regardless
      // of the spam score. Used to enforce published DMARC policies.
      void SetRejectMessage(bool newValue) {reject_message_ = newValue; }
      bool GetRejectMessage() const {return reject_message_; }

      void SetMarkAsSpam(bool newValue) {mark_as_spam_ = newValue; }
      bool GetMarkAsSpam() const {return mark_as_spam_; }

   private:

      int spam_score_;
      String message_;
      String testName_;
      Result result_;

      bool reject_message_;
      bool mark_as_spam_;

   };

}