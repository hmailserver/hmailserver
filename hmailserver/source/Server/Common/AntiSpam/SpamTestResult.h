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

   private:

      int spam_score_;
      String message_;
      String testName_;
      Result result_;

   };

}