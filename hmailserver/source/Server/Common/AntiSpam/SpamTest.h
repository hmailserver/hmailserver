// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class SpamTestData;
   class SpamTestResult;

   class SpamTest
   {
   public:
      
      SpamTest();
      virtual ~SpamTest();

      enum SpamTestType
      {
         PreTransmission = 1,
         PostTransmission = 2,
      };
      
      virtual String GetName() const = 0;
      // Returns the name of the spam protection mechanism. Used for logging
      
      virtual bool GetIsEnabled() = 0;
      // Returns true if this spam protection mechanism is enabled. False otherwise.
      
      virtual SpamTestType GetTestType() = 0;

      virtual bool GetAlwaysRun() {return false; }
      // Returns true if this test should run even when the spam score threshold
      // has already been reached.

      virtual std::set<std::shared_ptr<SpamTestResult> > RunTest(std::shared_ptr<SpamTestData> pTestData) = 0;
         
   private:

   };

}