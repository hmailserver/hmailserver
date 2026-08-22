// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


namespace HM
{
   class Base64
   {
   public:
      static AnsiString Encode(const char *input, int inputLength);
      static AnsiString Decode(const char *input, int inputLength);

      static AnsiString EncodeUnpadded(const char *input, int inputLength);
      // As Encode, but without the trailing padding. Used by formats which specify
      // unpadded base64, such as the PHC string format used for password hashes.

      static bool DecodeUnpadded(const AnsiString &input, AnsiString &output);
      // The exact inverse of EncodeUnpadded, and unlike Decode it validates what it
      // is given: anything outside the base64 alphabet, any padding, any embedded
      // line break and any non-canonical trailing bits make it return false rather
      // than decode as much as it can. Callers parsing a stored credential need the
      // malformed case to be rejected, not silently truncated.

   private:
   };

   class Base64Tester
   {
   public:
      void Test();

   private:
      void TestUnpadded_();
   };

}