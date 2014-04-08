/*
	Copyright (c) 2003, Jeff Lee
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <utility>
#include <string>
#include <list>
using namespace std;

#if !defined(ASSERT)
#if defined(_DEBUG)
	#include <assert.h>
	#define ASSERT(exp)	assert(exp)
#else
	#define ASSERT(exp)	((void)0)
#endif
#endif

// maximum length of an encoded line (RFC 2045)
#define MAX_MIME_LINE_LEN	76
#define MAX_ENCODEDWORD_LEN	75

//////////////////////////////////////////////////////////////////////
// MimeEnvironment - global environment to manage encoding/decoding
//////////////////////////////////////////////////////////////////////
namespace HM
{
   class MimeCodeBase;
   class FieldCodeBase;
   class MimeBody;
   class MimeEnvironment
   {
   public:
	   MimeEnvironment();

   public:
	   // global options
	   static void SetAutoFolding(bool bAutoFolding=true);
	   static bool AutoFolding() { return m_bAutoFolding; }
	   static void SetGlobalCharset(const char* pszCharset) { m_strCharset = pszCharset; }
	   static const char* GetGlobalCharset() { return m_strCharset.c_str(); }

	   // Content-Transfer-Encoding coder management
	   typedef MimeCodeBase* (*CODER_FACTORY)();
	   static void RegisterCoder(const char* pszCodingName, CODER_FACTORY pfnCreateObject=NULL);
	   static MimeCodeBase* CreateCoder(const char* pszCodingName);

	   // header fields encoding/folding management
	   typedef FieldCodeBase* (*FIELD_CODER_FACTORY)();
	   static void RegisterFieldCoder(const char* pszFieldName, FIELD_CODER_FACTORY pfnCreateObject=NULL);
	   static FieldCodeBase* CreateFieldCoder(const char* pszFieldName);

	   // media type management
	   typedef shared_ptr<MimeBody> (*BODY_PART_FACTORY)();
	   static void RegisterMediaType(const char* pszMediaType, BODY_PART_FACTORY pfnCreateObject=NULL);
	   static shared_ptr<MimeBody> CreateBodyPart(const char* pszMediaType);

   private:
	   static bool m_bAutoFolding;
	   static string m_strCharset;

	   typedef pair<const char*, CODER_FACTORY> CODER_PAIR;
	   static list<CODER_PAIR> m_listCoders;

	   typedef pair<const char*, FIELD_CODER_FACTORY> FIELD_CODER_PAIR;
	   static list<FIELD_CODER_PAIR> m_listFieldCoders;

	   typedef pair<const char*, BODY_PART_FACTORY> MEDIA_TYPE_PAIR;
	   static list<MEDIA_TYPE_PAIR> m_listMediaTypes;

	   static MimeEnvironment m_globalMgr;
   };

   #define DECLARE_MIMECODER(class_name) \
	   public: static MimeCodeBase* CreateObject() { return new class_name; }

   #define REGISTER_MIMECODER(coding_name, class_name) \
	   MimeEnvironment::RegisterCoder(coding_name, class_name::CreateObject)

   #define DEREGISTER_MIMECODER(coding_name) \
	   MimeEnvironment::RegisterCoder(coding_name, 0)

   #define DECLARE_FIELDCODER(class_name) \
	   public: static FieldCodeBase* CreateObject() { return new class_name; }

   #define REGISTER_FIELDCODER(field_name, class_name) \
	   MimeEnvironment::RegisterFieldCoder(field_name, class_name::CreateObject)

   #define DEREGISTER_FIELDCODER(field_name) \
	   MimeEnvironment::RegisterFieldCoder(field_name, 0)

   #define DECLARE_MEDIATYPE(class_name) \
	   public: static shared_ptr<MimeBody> CreateObject() { return new class_name; }

   #define REGISTER_MEDIATYPE(media_type, class_name) \
	   MimeEnvironment::RegisterMediaType(media_type, class_name::CreateObject)

   #define DEREGISTER_MEDIATYPE(media_type) \
	   MimeEnvironment::RegisterMediaType(media_type, 0)

   //////////////////////////////////////////////////////////////////////
   // MimeCodeBase - base class for MIME encoding/decoding
   // default implementation for binary/unknown encoding mechanism
   //////////////////////////////////////////////////////////////////////
   class MimeCodeBase
   {
   public:
	   MimeCodeBase() :
		   m_pbInput(NULL),
		   m_nInputSize(0),
		   m_bIsEncoding(false) {}

   public:
	   void SetInput(const char* pbInput, int nInputSize, bool bEncoding)
	   {
		   m_pbInput = (const unsigned char*) pbInput;
		   m_nInputSize = nInputSize;
		   m_bIsEncoding = bEncoding;
	   }

      void GetOutput(AnsiString &output)
	   {
		   return m_bIsEncoding ? Encode(output) : Decode(output);
	   }

   protected:
	   // overrides
	   virtual void Encode(AnsiString &output) const
	   {
         output.append((char*) m_pbInput, m_nInputSize+1);
	   }
	   virtual void Decode(AnsiString & output)
	   {
		   return MimeCodeBase::Encode(output);
	   }

   protected:
	   const unsigned char* m_pbInput;
	   int m_nInputSize;
	   bool m_bIsEncoding;
   };

   //////////////////////////////////////////////////////////////////////
   // MimeCode7bit - for 7bit/8bit encoding mechanism (fold long line)
   //////////////////////////////////////////////////////////////////////
   class MimeCode7bit : public MimeCodeBase
   {
	   DECLARE_MIMECODER(MimeCode7bit)

   protected:
	   virtual void Encode(AnsiString &output) const;
   };

   //////////////////////////////////////////////////////////////////////
   // MimeCodeQP - for quoted-printable encoding mechanism
   //////////////////////////////////////////////////////////////////////
   class MimeCodeQP : public MimeCodeBase
   {
   public:
	   MimeCodeQP() :
		   m_bQuoteLineBreak(false),
         m_bAddLineBreak(false) {}

   public:
	   DECLARE_MIMECODER(MimeCodeQP)
	   void QuoteLineBreak(bool bQuote=true) { m_bQuoteLineBreak = bQuote; }

      void AddLineBreak(bool bNewVal) {m_bAddLineBreak = bNewVal; }
   protected:
	   virtual void Encode(AnsiString &output) const;
	   virtual void Decode(AnsiString &output);

   private:
	   bool m_bQuoteLineBreak;
	   bool m_bAddLineBreak;
   };

   //////////////////////////////////////////////////////////////////////
   // MimeCodeQ - for q encoding mechanism
   //////////////////////////////////////////////////////////////////////
   class MimeCodeQ : public MimeCodeQP
   {
   public:
   protected:
      virtual void Decode(AnsiString &output);
   };

   //////////////////////////////////////////////////////////////////////
   // MimeCodeBase64 - for base64 encoding mechanism
   //////////////////////////////////////////////////////////////////////
   class MimeCodeBase64 : public MimeCodeBase
   {
   public:
	   MimeCodeBase64() :
		   m_bAddLineBreak(true) {}

   public:
	   DECLARE_MIMECODER(MimeCodeBase64)
	   void AddLineBreak(bool bAdd=true) { m_bAddLineBreak = bAdd; }

   protected:
	   virtual void Encode(AnsiString &result) const;
	   virtual void Decode(AnsiString &result);

   private:
	   bool m_bAddLineBreak;

   private:
	   static inline int DecodeBase64Char(unsigned int nCode)
	   {
		   if (nCode >= 'A' && nCode <= 'Z')
			   return nCode - 'A';
		   if (nCode >= 'a' && nCode <= 'z')
			   return nCode - 'a' + 26;
		   if (nCode >= '0' && nCode <= '9')
			   return nCode - '0' + 52;
		   if (nCode == '+')
			   return 62;
		   if (nCode == '/')
			   return 63;
		   return 64;
	   }
   };

   //////////////////////////////////////////////////////////////////////
   // MimeEncodedWord - encoded word for non-ascii text (RFC 2047)
   //////////////////////////////////////////////////////////////////////
   class MimeEncodedWord : public MimeCodeBase
   {
   public:
	   MimeEncodedWord() :
		   m_nEncoding(0) {}

	   void SetEncoding(int nEncoding, const char* pszCharset)
	   {
		   m_nEncoding = nEncoding;
		   m_strCharset = pszCharset;
	   }
	   int GetEncoding() const { return m_nEncoding; }
	   const char* GetCharset() const { return m_strCharset.c_str(); }

   protected:
	   virtual void Encode(AnsiString &output) const;
	   virtual void Decode(AnsiString &output);

   private:
	   int m_nEncoding;
	   AnsiString m_strCharset;

	   void BEncode(AnsiString &output) const;
	   void QEncode(AnsiString &utput) const;
   };

   //////////////////////////////////////////////////////////////////////
   // FieldCodeBase - base class to encode/decode header fields
   // default coder for any unregistered fields
   //////////////////////////////////////////////////////////////////////
   class FieldCodeBase : public MimeCodeBase
   {
   public:
	   void SetCharset(const char* pszCharset) { m_strCharset = pszCharset; }
	   const char* GetCharset() const { return m_strCharset.c_str(); }

   protected:
	   string m_strCharset;

	   virtual bool IsFoldingChar(char ch) const { return false; }
	   virtual int GetDelimeter() const { return 0; }
	   int FindSymbol(const char* pszData, int nSize, int& nDelimeter, int& nNonAscChars) const;
	   void UnfoldField(string& strField) const;
	   int SelectEncoding(int nLength, int nNonAsciiChars) const
	   {
		   int nQEncodeSize = nLength + nNonAsciiChars * 2;
		   int nBEncodeSize = (nLength + 2) / 3 * 4;
		   return (nQEncodeSize <= nBEncodeSize || nNonAsciiChars*5 <= nLength) ? 'Q' : 'B';
	   }

   protected:
	   virtual void Encode(AnsiString &output) const;
	   virtual void Decode(AnsiString &output);
   };

   //////////////////////////////////////////////////////////////////////
   // FieldCodeText - encode/decode header fields defined as *text
   //////////////////////////////////////////////////////////////////////
   class FieldCodeText : public FieldCodeBase
   {
	   DECLARE_FIELDCODER(FieldCodeText)

   protected:
	   virtual int GetDelimeter() const { return 0xff; }
   };

   //////////////////////////////////////////////////////////////////////
   // FieldCodeAddress - encode/decode header fields defined as address
   //////////////////////////////////////////////////////////////////////
   class FieldCodeAddress : public FieldCodeBase
   {
	   DECLARE_FIELDCODER(FieldCodeAddress)

   protected:
	   virtual bool IsFoldingChar(char ch) const { return ch == ',' || ch == ':'; }
   };

   //////////////////////////////////////////////////////////////////////
   // FieldCodeParameter - encode/decode header fields have parameters
   //////////////////////////////////////////////////////////////////////
   class FieldCodeParameter : public FieldCodeBase
   {
	   DECLARE_FIELDCODER(FieldCodeParameter)

   protected:
	   virtual bool IsFoldingChar(char ch) const { return ch == ';'; }
   };

   class MimeParameterRFC2184Decoder
   {
   public:
      AnsiString Decode(const AnsiString &input);

   };

}
