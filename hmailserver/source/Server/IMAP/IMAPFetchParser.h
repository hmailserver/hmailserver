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

         
         int m_iOctetStart;
         int m_iOctetCount;

         const String &GetName() const {return m_sName; }
         void SetName(const String &sName) {m_sName = sName; }

         bool GetShowBodyHeaderFields() { return m_bShowBodyHeaderFields; }
         bool GetShowBodyHeaderFieldsNOT() { return m_bShowBodyHeaderFieldsNOT; }
         bool GetShowBodyHeader() { return m_bShowBodyHeader; }
         bool GetShowBodyText() { return m_bShowBodyText; }
         bool GetShowBodyFull() { return m_bShowBodyFull; }

         void SetShowBodyHeaderFields(bool bValue) {m_bShowBodyHeaderFields = bValue; }
         void SetShowBodyHeaderFieldsNOT(bool bValue) {m_bShowBodyHeaderFieldsNOT = bValue; }
         void SetShowBodyHeader(bool bValue) {m_bShowBodyHeader = bValue; }
         void SetShowBodyText(bool bValue) {m_bShowBodyText = bValue; }
         void SetShowBodyFull(bool bValue) {m_bShowBodyFull = bValue; }
         
         std::vector<String> &GetHeaderFields() { return m_vecHeaderFields; }
         std::vector<String> &GetHeaderFieldsNOT() { return m_vecHeaderFieldsNOT; }

         void SetDescription(const String &sDescription ) {m_sDescription = sDescription; }
         String &GetDescription() {return m_sDescription; }

         bool GetBodyTextNeeded()
         {
            // Returns true if we need to load the entire body part, false otherwise.
            return m_bShowBodyText || m_bShowBodyFull;
         }

      private:

         String m_sName;

         bool m_bShowBodyHeaderFields;
         bool m_bShowBodyHeaderFieldsNOT;
         bool m_bShowBodyHeader;
         bool m_bShowBodyText;
         bool m_bShowBodyFull;

         std::vector<String> m_vecHeaderFields;
         std::vector<String> m_vecHeaderFieldsNOT;

         String m_sDescription;

      };

      IMAPResult ParseCommand(const String &sCommand);

      bool GetShowEnvelope() { return m_bShowEnvelope; }
      bool GetShowRFCSize() { return m_bShowRFCSize; }
      bool GetShowUID() { return m_bShowUID; }
      bool GetShowFlags() { return m_bShowFlags; }
      bool GetShowInternalDate() { return m_bShowInternalDate; }
      
      bool GetShowBodyStructure() { return m_bShowBodyStructure; }
      bool GetShowBodyStructureNonExtensible() { return m_bShowBodyStructureNonExtensible; }
      

      bool GetSetSeenFlag() { return m_bSetSeen; }

      std::vector<BodyPart> GetPartsToLookAt() { return m_vecPartsToLookAt; }
      
   private:

      ePartType _GetPartType(const String &sPart);
      bool _IsPartSpecifier(const String &sString);
      
      void _CleanFetchString(String &sString);
      std::vector<String> _ParseString(String &sString);
      IMAPResult _ValidateSyntax(const String &sString);
      
      // Additional parsing of commands that create more complex 
      // structure than just single words.
      BodyPart _ParseBODY(const String &sString);
      BodyPart _ParseBODYPEEK(const String &sString);

      bool m_bShowEnvelope;
      bool m_bShowRFCSize;
      bool m_bShowUID;
      bool m_bShowFlags;
      bool m_bShowInternalDate;
      bool m_bShowBodyStructure;
      bool m_bShowBodyStructureNonExtensible;

      bool m_bSetSeen;
      
      std::vector<BodyPart> m_vecPartsToLookAt;
   };

}
