// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class IMAPResult
   {
   public:
	   enum Result
      {
         ResultOK = 1,
         ResultBad = 2,
         ResultNo = 3
      };
      
      IMAPResult();
      IMAPResult(Result result, const String &message);
      IMAPResult(const IMAPResult &otherCopy);
      IMAPResult& operator= (const IMAPResult& f);

	   virtual ~IMAPResult();

      const Result& GetResult() const {return _result;}
      const AnsiString& GetMessage() const {return _message;}

   private:

      Result _result;
      AnsiString _message;
      
   };

}
