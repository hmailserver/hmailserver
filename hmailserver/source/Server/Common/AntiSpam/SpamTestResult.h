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

      Result GetResult() {return _result; }
      int GetSpamScore() {return m_iSpamScore; }
      String GetMessage() {return m_sMessage; }
      String GetTestName() {return _testName; }

   private:

      int m_iSpamScore;
      String m_sMessage;
      String _testName;
      Result _result;

   };

}