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
      int GetSpamScore() {return m_iSpamScore; }
      String GetMessage() {return m_sMessage; }
      String GetTestName() {return testName_; }

   private:

      int m_iSpamScore;
      String m_sMessage;
      String testName_;
      Result result_;

   };

}