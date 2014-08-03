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
         ResultNo = 3,
         ResultOKSupressRead = 4
      };
      
      IMAPResult();
      IMAPResult(Result result, const String &message);
      IMAPResult(const IMAPResult &otherCopy);
      IMAPResult& operator= (const IMAPResult& f);

	   virtual ~IMAPResult();

      const Result& GetResult() const {return result_;}
      const AnsiString& GetMessage() const {return message_;}
      
   private:

      Result result_;
      AnsiString message_;

   };

}
