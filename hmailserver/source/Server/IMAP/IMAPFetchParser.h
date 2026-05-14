// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class IMAPFetchParser  
   {
   public:
      
	   IMAPFetchParser();
	   virtual ~IMAPFetchParser();

      enum ePartType
      {
         PARTUNKNOWN = 0,
         BODYPEEK = 201,
         ENVELOPE = 202,
         RFC822SIZE = 203,
         UID = 204,
         FLAGS = 205,
         INTERNALDATE = 206,
         BODYSTRUCTURE = 207,
         BODY = 208,
         RFC822 = 209,
         ALL = 210,
         FAST = 211,
         FULL = 212,
         RFC822HEADER = 213,
         RFC822TEXT = 214,
         BODYSTRUCTURENONEXTENSIBLE = 215
            
      };

      class BodyPart
      {
      public:
         BodyPart();

         
         int octet_start_;
         int octet_count_;

         const String &GetName() const {return name_; }
         void SetName(const String &sName) {name_ = sName; }

         bool GetShowBodyHeaderFields() { return show_body_header_fields_; }
         bool GetShowBodyHeaderFieldsNOT() { return show_body_header_fields_NOT; }
         bool GetShowBodyHeader() { return show_body_header_; }

         // BODY[X.MIME] — the MIME entity headers of part X itself (e.g. Content-Type:
         // message/rfc822). Does NOT load any encapsulated message; returns the outer
         // part's own headers, not those of the message it contains.
         bool GetShowBodyMime() { return show_body_mime_; }

         // BODY[X.TEXT] — explicitly requested text body. For message/rfc822 parts,
         // loads the encapsulated message and returns its body without RFC2822 headers.
         bool GetShowBodyText() { return show_body_text_; }

         // BODY[] — no section specifier; returns the entire raw message from disk,
         // headers and body included.
         bool GetShowBodyFull() { return show_body_full_; }

         // BODY[X] — numeric-only specifier with no sub-keyword. For message/rfc822
         // parts, returns the full inner RFC2822 message (inner headers + body).
         // For all other parts, returns the body content without the MIME entity header.
         // Differs from GetShowBodyText() in that inner headers are included for
         // message/rfc822, and from GetShowBodyFull() in that it navigates to a
         // specific part rather than reading the whole file.
         bool GetShowBodyContent() { return show_body_content_; }

         void SetShowBodyHeaderFields(bool bValue) {show_body_header_fields_ = bValue; }
         void SetShowBodyHeaderFieldsNOT(bool bValue) {show_body_header_fields_NOT = bValue; }
         void SetShowBodyHeader(bool bValue) {show_body_header_ = bValue; }
         void SetShowBodyMime(bool bValue) {show_body_mime_ = bValue; }
         void SetShowBodyText(bool bValue) {show_body_text_ = bValue; }
         void SetShowBodyFull(bool bValue) {show_body_full_ = bValue; }
         void SetShowBodyContent(bool bValue) {show_body_content_ = bValue; }

         std::vector<String> &GetHeaderFields() { return header_fields_; }
         std::vector<String> &GetHeaderFieldsNOT() { return header_fields_NOT; }

         void SetDescription(const String &sDescription ) {description_ = sDescription; }
         String &GetDescription() {return description_; }

         bool GetBodyTextNeeded()
         {
            // Returns true if we need to load the entire body part, false otherwise.
            return show_body_text_ || show_body_full_ || show_body_content_;
         }

      private:

         String name_;

         bool show_body_header_fields_;
         bool show_body_header_fields_NOT;
         bool show_body_header_;
         bool show_body_mime_;
         bool show_body_text_;
         bool show_body_full_;
         bool show_body_content_;

         std::vector<String> header_fields_;
         std::vector<String> header_fields_NOT;

         String description_;

      };

      IMAPResult ParseCommand(const String &sCommand);

      bool GetShowEnvelope() { return show_envelope_; }
      bool GetShowRFCSize() { return show_rfcsize_; }
      bool GetShowUID() { return show_uid_; }
      bool GetShowFlags() { return show_flags_; }
      bool GetShowInternalDate() { return show_internal_date_; }
      
      bool GetShowBodyStructure() { return show_body_structure_; }
      bool GetShowBodyStructureNonExtensible() { return show_body_structure_NonExtensible; }
      

      bool GetSetSeenFlag() { return set_seen_; }

      std::vector<BodyPart> GetPartsToLookAt() { return parts_to_look_at_; }
      
   private:

      ePartType GetPartType_(const String &sPart);
      bool IsPartSpecifier_(const String &sString);
      
      void CleanFetchString_(String &sString);
      std::vector<String> ParseString_(String &sString);
      IMAPResult ValidateSyntax_(const String &sString);
      
      // Additional parsing of commands that create more complex 
      // structure than just single words.
      BodyPart ParseBODY_(const String &sString);
      BodyPart ParseBODY_PEEK(const String &sString);

      bool show_envelope_;
      bool show_rfcsize_;
      bool show_uid_;
      bool show_flags_;
      bool show_internal_date_;
      bool show_body_structure_;
      bool show_body_structure_NonExtensible;

      bool set_seen_;
      
      std::vector<BodyPart> parts_to_look_at_;
   };

}
