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

#include <string>
#include <list>
using namespace std;

// RFC 1521 - Mechanisms for Specifying and Describing the Format of Internet Message Bodies
// RFC 2045 - Format of Internet Message Bodies
// RFC 2046 - Media Types
// RFC 2047 - Message Header Extensions for Non-ASCII Text
// RFC 2049 - Conformance Criteria and Examples
// RFC 822 - Standard For The Format of ARPA Internet Text Message
namespace HM
{
   class CMimeConst
   {
   public:
	   // field names
	   static inline const char* MimeVersion() { return "MIME-Version"; }
	   static inline const char* ContentType() { return "Content-Type"; }
	   static inline const char* TransferEncoding() { return "Content-Transfer-Encoding"; }
	   static inline const char* ContentID() { return "Content-ID"; }
	   static inline const char* ContentDescription() { return "Content-Description"; }
	   static inline const char* ContentDisposition() { return "Content-Disposition"; }

	   // parameter names
	   static inline const char* Charset() { return "charset"; }
	   static inline const char* Name() { return "name"; }
	   static inline const char* Filename() { return "filename"; }
	   static inline const char* Boundary() { return "boundary"; }
	   static inline const char* Inline() { return "inline"; }
      static inline const char* Attachment() { return "attachment"; }

	   // parameter values
	   static inline const char* Encoding7Bit() { return "7bit"; }
	   static inline const char* Encoding8Bit() { return "8bit"; }
	   static inline const char* EncodingBinary() { return "binary"; }
	   static inline const char* EncodingQP() { return "quoted-printable"; }
	   static inline const char* EncodingBase64() { return "base64"; }

	   static inline const char* MediaText() { return "text"; }
	   static inline const char* MediaImage() { return "image"; }
	   static inline const char* MediaAudio() { return "audio"; }
	   static inline const char* MediaVedio() { return "vedio"; }
	   static inline const char* MediaApplication() { return "application"; }
	   static inline const char* MediaMultiPart() { return "multipart"; }
	   static inline const char* MediaMessage() { return "message"; }
   };

   //////////////////////////////////////////////////////////////////////
   // MimeField class - Represents a field of a MIME body part header
   //////////////////////////////////////////////////////////////////////
   class MimeField
   {
   public:
	   MimeField() {}
	   ~MimeField() {}

   public:
	   void SetName(const char* pszName);
	   const char* GetName() const;
	   void SetValue(const char* pszValue);
	   const char* GetValue() const;
	   void GetValue(string& strValue) const;

	   void SetParameter(const char* pszAttr, const char* pszValue);
	   bool GetParameter(const char* pszAttr, AnsiString& strValue) const;
      /*
      encodedParameter indicates whether the content is encoding according to 

      MIME Parameter Value and Encoded Word Extensions:
      Character Sets, Languages, and Continuations

      http://tools.ietf.org/html/draft-freed-pvcsc-03

      */
	   void SetCharset(const char* pszCharset);
	   const char* GetCharset() const;

	   void Clear();
	   int GetLength() const;
	   void Store(AnsiString &output) const;
	   int Load(const char* pszData, int nDataSize, bool unfold);
      
      static void UnfoldField(string& strField);

   private:
	   string m_strName;				// field name
	   string m_strValue;		   // encoded value
	   string m_strCharset;			// charset for non-ascii text

      string m_strDecodedValue;

   private:

	   bool FindParameter(const char* pszAttr, int& nPos, int& nSize, bool &encodedParameter) const;
      /*
         encodedParameter indicates whether the content is encoding according to 

         MIME Parameter Value and Encoded Word Extensions:
            Character Sets, Languages, and Continuations
      
         http://tools.ietf.org/html/draft-freed-pvcsc-03

      */
   };

   inline void MimeField::SetName(const char* pszName)
   { m_strName = pszName; }

   inline const char* MimeField::GetName() const
   { return m_strName.data(); }

   inline void MimeField::SetValue(const char* pszValue)
   { m_strValue = pszValue; }

   inline const char* MimeField::GetValue() const
   { return m_strValue.data(); }

   inline void MimeField::SetCharset(const char* pszCharset)
   { m_strCharset = pszCharset; }

   inline const char* MimeField::GetCharset() const
   { return m_strCharset.c_str(); }

   inline void MimeField::Clear()
   { m_strName.empty(); m_strValue.empty(); m_strCharset.empty(); }

   class MIMEUnicodeEncoder
   {
   public:

      static AnsiString EncodeValue(const AnsiString &sCharSet, const String &sUnicodeValue);

   };


   class MimeHeader
   {
   public:
	   MimeHeader() {}
	   virtual ~MimeHeader() { Clear(); }

   public:
	   enum MediaType
	   {
		   MEDIA_TEXT, MEDIA_IMAGE, MEDIA_AUDIO, MEDIA_VIDEO, MEDIA_APPLICATION,
		   MEDIA_MULTIPART, MEDIA_MESSAGE,
		   MEDIA_UNKNOWN
	   };
	   MediaType GetMediaType() const;

	   // set/get the values of header fields
	   void SetField(const MimeField& field);
	   MimeField* GetField(const char* pszFieldName) const;
      MimeField* GetField(unsigned int iIndex);
	   
      int GetFieldCount() const;

      void SetUnicodeFieldValue(const AnsiString &sFieldName, const String &sFieldValue, const AnsiString & sCharset);
      void SetRawFieldValue(const AnsiString &pszFieldName, const AnsiString &pszFieldValue, const AnsiString & pszCharset);
	   
      const char* GetRawFieldValue(const String &pszFieldName) const;
      
      String GetUnicodeFieldValue(const AnsiString &pszFieldName) const;
      

      bool SetParameter(const AnsiString &pszFieldName, const AnsiString & pszAttr, const AnsiString & pszValue);
	   string GetParameter(const char* pszFieldName, const char* pszAttr) const;
	   void SetFieldCharset(const char* pszFieldName, const char* pszCharset);
	   const char* GetFieldCharset(const char* pszFieldName) const;
      AnsiString GetHeaderContents();
      String GetUnicodeHeaderContents();

	   // helper functions for standard body part fields
	   void SetContentType(const AnsiString &pszValue, const AnsiString &pszCharset);
	   const char* GetContentType() const;			// Content-Type: mediatype/subtype
	   string GetMainType() const;
	   string GetSubType() const;
	   void SetCharset(const char* pszCharset);	// Content-Type: text/...; charset=...
	   string GetCharset() const;
	   void SetName(const char* pszName);			// Content-Type: image/...; name=...
	   string GetName() const;
	   void SetBoundary(const char* pszBoundary=NULL);	// Content-Type: multipart/...; boundary=...
	   string GetBoundary() const;

	   void SetTransferEncoding(const char* pszValue);
	   const char* GetTransferEncoding() const;	// Content-Transfer-Encoding: ...
	   void SetDisposition(const char* pszValue, const char* pszCharset=NULL);
	   const char* GetDisposition() const;			// Content-Disposition: ...
	   virtual String GetUnicodeFilename() const;					// Content-Disposition: ...; filename=...
      virtual String GetRawFilename() const;					// Content-Disposition: ...; filename=...
	   void SetDescription(const char* pszValue, const char* pszCharset=NULL);
	   const char* GetDescription() const;			// Content-Description: ...

	   typedef vector<MimeField> CFieldList;
	   CFieldList& Fields() { return m_listFields; }
   
      bool FieldExists(const char *pszFieldName) const;

      void DeleteField(MimeField *pField);
      void DeleteField(const AnsiString &fieldName);

   public:
	   // overrides
	   virtual void Clear();
	   virtual int GetLength() const;
	   // serialization
	   virtual void Store(AnsiString &output) const;
	   virtual int Load(const char* pszData, int nDataSize, bool unfold = true);

      AnsiString Store() const;

   protected:
	   vector<MimeField> m_listFields;	// list of all header fields
	   vector<MimeField>::iterator FindField(const char* pszFieldName) const;

	   struct MediaTypeCvt
	   {
		   int nMediaType;				// media type
		   const char* pszSubType;		// subtype
		   const char* pszFileExt;		// file extension name
	   };
	   static const MediaTypeCvt m_TypeCvtTable[];
	   static const char* m_TypeTable[];

   private:
      static String GetUnicodeFieldValue(const AnsiString &pszFieldName, const AnsiString &sRawFieldValue);

	   MimeHeader& operator=(const MimeHeader&);		// forbid operator =
   };

   // add a new field or update an existing field
   inline void MimeHeader::SetField(const MimeField& field)
   {
	   vector<MimeField>::iterator it = FindField(field.GetName());
	   if (it != m_listFields.end())
		   *it = field;
	   else
		   m_listFields.push_back(field);
   }

   // find a field by name
   inline MimeField* MimeHeader::GetField(const char* pszFieldName) const
   {
	   vector<MimeField>::iterator it = FindField(pszFieldName);
   
      if ((vector<MimeField>::const_iterator) it != m_listFields.end())
		   return &(*it);
	   return NULL;
   }

   inline int MimeHeader::GetFieldCount() const
   {
      return (int) m_listFields.size();
   }

   inline MimeField* MimeHeader::GetField(unsigned int iIndex) 
   {
      if (iIndex <= m_listFields.size() -1)
      {
         HM::MimeField *pField = &m_listFields[iIndex];

         return pField;
      }

      return NULL;
   }


   // add a new field or update an existing field
   inline void MimeHeader::SetRawFieldValue(const AnsiString & pszFieldName, const AnsiString & pszFieldValue, const AnsiString & pszCharset)
   {
	   MimeField fd;
	   fd.SetName(pszFieldName);
	   fd.SetValue(pszFieldValue);
	   if (!pszCharset.IsEmpty())
		   fd.SetCharset(pszCharset);

	   SetField(fd);
   }

   inline const char* MimeHeader::GetRawFieldValue(const String &pszFieldName) const
   {
      AnsiString sHeaderName = pszFieldName;
	   MimeField* pfd = GetField(sHeaderName);
	   return pfd != NULL ? pfd->GetValue() : NULL;
   }


   

   inline void MimeHeader::SetFieldCharset(const char* pszFieldName, const char* pszCharset)
   {
	   MimeField *pfd = GetField(pszFieldName);
	   if (pfd)
		   pfd->SetCharset(pszCharset);
	   else
	   {
		   MimeField fd;
		   fd.SetName(pszFieldName);
		   fd.SetCharset(pszCharset);
		   SetField(fd);
	   }
   }

   inline const char* MimeHeader::GetFieldCharset(const char* pszFieldName) const
   {
	   MimeField* pfd = GetField(pszFieldName);
	   return pfd != NULL ? pfd->GetCharset() : NULL;
   }

   inline bool MimeHeader::SetParameter(const AnsiString & pszFieldName, const AnsiString & pszAttr, const AnsiString & pszValue)
   {
	   MimeField *pfd = GetField(pszFieldName);
	   if (pfd)
	   {
		   pfd->SetParameter(pszAttr, pszValue);
		   return true;
	   }
	   return false;
   }

   inline string MimeHeader::GetParameter(const char* pszFieldName, const char* pszAttr) const
   {
      AnsiString strVal;
	   MimeField *pfd = GetField(pszFieldName);
	   if (pfd)
		   pfd->GetParameter(pszAttr, strVal);

	   return strVal;
   }
   
   

   inline void MimeHeader::SetContentType(const AnsiString &pszValue, const AnsiString &pszCharset)
   { SetRawFieldValue(CMimeConst::ContentType(), pszValue, pszCharset); }

   inline const char* MimeHeader::GetContentType() const
   { return GetRawFieldValue(CMimeConst::ContentType()); }

   inline string MimeHeader::GetCharset() const
   { return GetParameter(CMimeConst::ContentType(), CMimeConst::Charset()); }

   inline string MimeHeader::GetName() const
   { return GetParameter(CMimeConst::ContentType(), CMimeConst::Name()); }

   inline string MimeHeader::GetBoundary() const
   { return GetParameter(CMimeConst::ContentType(), CMimeConst::Boundary()); }

   inline void MimeHeader::SetTransferEncoding(const char* pszValue)
   { SetRawFieldValue(CMimeConst::TransferEncoding(), pszValue, ""); }

   inline const char* MimeHeader::GetTransferEncoding() const
   { return GetRawFieldValue(CMimeConst::TransferEncoding()); }

   // Content-Disposition header field specifies how to present this body part. it could be
   // inline or attachment. 'inline' indicates this body part should be displayed in the main body;
   // 'attachment' indicates it is separate from the main body. (RFC 2183)

   inline void MimeHeader::SetDisposition(const char* pszValue, const char* pszCharset)
   { SetRawFieldValue(CMimeConst::ContentDisposition(), pszValue, pszCharset); }

   inline const char* MimeHeader::GetDisposition() const
   { return GetRawFieldValue(CMimeConst::ContentDisposition()); }

   inline void MimeHeader::SetDescription(const char* pszValue, const char* pszCharset)
   { SetRawFieldValue(CMimeConst::ContentDescription(), pszValue, pszCharset); }

   inline const char* MimeHeader::GetDescription() const
   { return GetRawFieldValue(CMimeConst::ContentDescription()); }

   //////////////////////////////////////////////////////////////////////
   // MimeBody class - Represents a body part in a MIME message
   //////////////////////////////////////////////////////////////////////
   class CMimeMessage;
   class MimeBody : public MimeHeader
   {
   public:
	   MimeBody() :				// instantiate a MimeBody object explicitly is not allowed. call CreatePart()
         m_iPartIndex(0) {}
	   virtual ~MimeBody() { Clear(); }

   public:
	   int GetContentLength() const;
      int GetContentEncodedLength() const;
	   const AnsiString& GetContent() const;

      virtual String GetUnicodeFilename() const;
      virtual String GetRawFilename() const;

	   // operations for 'text' or 'message' media
	   bool IsText() const;
	   
      int SetRawText(const AnsiString &pbText);
      string GetRawText();

      int SetUnicodeText(const String &sText);
      String GetUnicodeText();

	   // operations for 'message' media
	   bool IsMessage() const;
	   bool SetMessage(const MimeBody* pMM);
	   void GetMessage(MimeBody* pMM) const;

	   // operations for 'image/audio/vedio/application' (attachment) media
	   bool IsAttachment() const;
	   bool ReadFromFile(const String &pszFilename);
	   bool WriteToFile(const String  &sFilename);

	   // operations for 'multipart' media
	   bool IsMultiPart() const;
	   void DeleteAll();
	   shared_ptr<MimeBody> CreatePart(const char* pszMediaType, shared_ptr<MimeBody> pWhere);
      void AddPart(shared_ptr<MimeBody> );
      int GetPartCount();

	   void ErasePart(shared_ptr<MimeBody> pBP);
	   shared_ptr<MimeBody> FindFirstPart();
	   shared_ptr<MimeBody> FindNextPart();
      size_t GetNumberOfParts();

	   typedef list<shared_ptr<MimeBody> > BodyList;
	   int GetAttachmentList(shared_ptr<MimeBody> pThis, BodyList& rList) const;
      void ClearAttachments();
      void RemoveAttachment(shared_ptr<MimeBody> pAttachment);
      int LoadFromFile(const AnsiString &pszFilename);
      bool SaveAllToFile(const AnsiString &pszFilename);

      String GetCleanContentType() const;
      // Returns a clean content type like text/html or text/plain,
      // not other things that exists in the Content-Type header.

      bool IsEncapsulatedRFC822Message() const;
      shared_ptr<MimeBody> LoadEncapsulatedMessage() const;

   public:
	   // overrides
	   virtual void Clear();
	   virtual int GetLength(bool bIncludeHeader=true) const;
	   // serialization
	   virtual void Store(AnsiString &output, bool bIncludeHeader=true) const;
   
	   virtual int Load(const char* pszData, int nDataSize, int &index);

   protected:

	   AnsiString m_pbText;		// content (text) of the body part
      int m_iPartIndex;
	   BodyList m_listBodies;			// list of all child body parts
	   BodyList::iterator m_itFind;

   protected:
	   bool AllocateBuffer(int nBufSize);
	   void FreeBuffer();

      String GenerateFileNameFromEncapsulatedSubject(bool unicode) const;
   };

   inline int MimeBody::GetContentLength() const
   { return (int) m_pbText.size(); }

   inline const AnsiString& MimeBody::GetContent() const
   { return m_pbText; }

   inline bool MimeBody::IsText() const
   { return GetMediaType() == MEDIA_TEXT; }

   inline bool MimeBody::IsMessage() const
   { return GetMediaType() == MEDIA_MESSAGE; }

   inline bool MimeBody::IsMultiPart() const
   { return GetMediaType() == MEDIA_MULTIPART; }

   inline size_t MimeBody::GetNumberOfParts()
   {
      return m_listBodies.size();
   }

   inline shared_ptr<MimeBody> MimeBody::FindFirstPart()
   {
	   m_itFind = m_listBodies.begin();
	   return FindNextPart();
   }

   inline shared_ptr<MimeBody> MimeBody::FindNextPart()
   {
	   if (m_itFind != m_listBodies.end())
		   return *m_itFind++;
	   
      shared_ptr<MimeBody> pEmpty;
      return pEmpty;
   }

   inline bool MimeBody::AllocateBuffer(int nBufSize)
   {

      m_pbText.reserve(nBufSize);

      return true;
   }

   inline void MimeBody::FreeBuffer()
   {
      m_pbText = "";
   }

}