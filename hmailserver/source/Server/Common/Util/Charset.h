// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class MimeField;

   class Charset  
   {
   public:
	   Charset();
	   virtual ~Charset();


      static bool IsProperlyEncoded(const String &sInput);
      // Runs some checks to see if sInput is properly MIME-encoded. hMailServer normally
      // just transmits data in the email message directly to the client without any
      // decoding and encoding. However, often message ehaders are not properly formatted. 
      // In these cases, hMailServer needs to format them. Otherwise, certain email clients
      // such as Outlook Express will fail to display the messages.
      
      static String Encode(const String &sUnencoded);
   
      static String GetIMAPEncoded(const String &sUnencoded);

      static AnsiString ToMultiByte(const String &sWideChar, const AnsiString &sCharacterSet);
      static String ToWideChar(const AnsiString &sMultiByte, const AnsiString &sCharacterSet);

   private:

      static String B64Encode(const String &sInput);
   };

   class CharsetTester
   {
   public:

      CharsetTester () {};
      ~CharsetTester () {};      

      void Test();
   };

}
