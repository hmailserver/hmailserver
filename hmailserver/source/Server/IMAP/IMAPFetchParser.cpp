// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPFetchParser.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPFetchParser::BodyPart::BodyPart() : m_sName(""), 
      m_iOctetStart(-1),
      m_iOctetCount(-1),
      m_bShowBodyHeaderFields(false),
      m_bShowBodyHeaderFieldsNOT(false),
      m_bShowBodyHeader(false),
      m_bShowBodyText(false),
      m_bShowBodyFull(false)
   {

   }

   IMAPFetchParser::IMAPFetchParser()
   {
      m_bShowEnvelope = false;
      m_bShowRFCSize = false;
      m_bShowUID = false;
      m_bShowFlags = false;
      m_bShowInternalDate = false;
      m_bShowBodyStructure = false;
      m_bSetSeen = false;
      m_bShowBodyStructureNonExtensible = false;
   }

   IMAPFetchParser::~IMAPFetchParser()
   {

   }

   void
   IMAPFetchParser::_CleanFetchString(String &sString)
   {
      if (sString.Left(1) == _T(" "))
         sString = sString.Mid(1);
      
      if (sString.Left(1) == _T("("))
         sString = sString.Mid(1);

      if (sString.Right(1) == _T(")"))
         sString = sString.Mid(0, sString.GetLength() - 1);
      

   }

   IMAPResult
   IMAPFetchParser::_ValidateSyntax(const String &sString)
   {
      long lNoOfLeftPar = sString.NumberOf(_T("("));
      long lNoOfRightPar = sString.NumberOf(_T(")"));

      long lNoOfLeftBrack = sString.NumberOf(_T("["));
      long lNoOfRightBrack = sString.NumberOf(_T("]"));

      if (lNoOfLeftBrack != lNoOfRightBrack)
         return IMAPResult(IMAPResult::ResultBad, "Brackets are mismatching.");
         
      if (lNoOfLeftPar != lNoOfRightPar)
         return IMAPResult(IMAPResult::ResultBad, "Parenthesises are mismatching.");

      return IMAPResult();
    
   }

   std::vector<String>
   IMAPFetchParser::_ParseString(String &sString)
   {

      std::vector<String> vecResult;

      _CleanFetchString(sString);

      while (!sString.IsEmpty())
      {
         long lFirstLeftBracket = sString.Find(_T("["));
         long lFirstSpace = sString.Find(_T(" "));

         if (lFirstLeftBracket >= 0 && lFirstLeftBracket < lFirstSpace)
         {
            // Find end bracket.
            long lFirstRightBracket = sString.Find(_T("]"), lFirstLeftBracket);

            // Check if we got a <> directly after the []
            if (sString.SafeGetAt(lFirstRightBracket + 1) == '<')
               lFirstRightBracket = sString.Find(_T(">"), lFirstRightBracket);

            // Parse out string between brackets.
            if (lFirstRightBracket <= 0)
            {
               sString = sString.Mid(lFirstRightBracket + 1);
               continue;
            }

            // String between brackets.
            String sResString = sString.Mid(0, lFirstRightBracket+1 );
            vecResult.push_back(sResString);

            // Cut away this from the start string.
            sString = sString.Mid(lFirstRightBracket + 1);
         }
         else if (lFirstSpace >= 0)
         {
            // Copy string from here to end.
            String sResString = sString.Mid(0, lFirstSpace );

            vecResult.push_back(sResString);

            sString = sString.Mid(lFirstSpace + 1);

         }
         else
         {
            vecResult.push_back(sString);
            sString.Empty();
         }
         

         _CleanFetchString(sString);


      }

      return vecResult;
   }


   IMAPResult
   IMAPFetchParser::ParseCommand(const String &sCommand)
   {
      String sStringToParse = sCommand;
      
      IMAPResult result = _ValidateSyntax(sStringToParse);
      if (result.GetResult() != IMAPResult::ResultOK)
         return result;
      
      std::vector<String> vecResult = _ParseString(sStringToParse);
      std::vector<String>::iterator iter = vecResult.begin();
      while (iter != vecResult.end())
      {
         String sPart = (*iter);

         
         ePartType iType = _GetPartType(sPart);

         switch (iType)
         {
            case BODYPEEK:
            {
               IMAPFetchParser::BodyPart oPart = _ParseBODYPEEK(sPart);
               m_vecPartsToLookAt.push_back(oPart);
               break;
            }
            
            case ENVELOPE:
            {
               m_bShowEnvelope = true;
               break;
            }
            
            case RFC822SIZE:
            {
               m_bShowRFCSize = true;
               break;
            }

            case UID:
            {
               m_bShowUID = true;
               break;
            }

            case FLAGS:
            {
               m_bShowFlags = true;
               break;
            }

            case INTERNALDATE:
            {
               m_bShowInternalDate = true;
               break;
            }

            case BODYSTRUCTURE:
            {
               m_bShowBodyStructure = true;
               break;
            }
            
            case BODYSTRUCTURENONEXTENSIBLE:
            {
               m_bShowBodyStructureNonExtensible = true;
               break;
            }

            case BODY:
            {
               IMAPFetchParser::BodyPart oPart = _ParseBODY(sPart);
               m_vecPartsToLookAt.push_back(oPart);
               break;
            }
            case RFC822:
            {
               // Same as:
               IMAPFetchParser::BodyPart oPart = _ParseBODY(sPart);
               oPart.SetDescription("RFC822");
               m_vecPartsToLookAt.push_back(oPart);
               break;

            }
            case ALL:
            {
               // ALL
               // Macro equivalent to: (FLAGS INTERNALDATE RFC822.SIZE ENVELOPE)

               m_bShowFlags = true;
               m_bShowInternalDate = true;
               m_bShowRFCSize = true;
               m_bShowEnvelope = true;
               break;
            }

            case FAST:
            {
               // FAST
               // Macro equivalent to: (FLAGS INTERNALDATE RFC822.SIZE)

               m_bShowFlags = true;
               m_bShowInternalDate = true;
               m_bShowRFCSize = true;
               break;
            }

            case FULL:
            {
               // FULL
               // Macro equivalent to: (FLAGS INTERNALDATE RFC822.SIZE ENVELOPE BODY)
               m_bShowFlags = true;
               m_bShowInternalDate = true;
               m_bShowRFCSize = true;
               m_bShowEnvelope = true;
               m_bShowBodyStructure = true;
               break;
            }
            case RFC822HEADER:
            {
               /* 
                  RFC822.HEADER
                  Functionally equivalent to BODY.PEEK[HEADER], differing in the
                  syntax of the resulting untagged FETCH data (RFC822.HEADER is
                  returned).
                  */
            
               IMAPFetchParser::BodyPart oPart = _ParseBODYPEEK("BODY[HEADER]");
               oPart.SetDescription("RFC822.HEADER");
               m_vecPartsToLookAt.push_back(oPart);
               break;
            }
            case RFC822TEXT:
               {
                  /* 
                  Functionally equivalent to BODY[TEXT], differing in the syntax
                  of the resulting untagged FETCH data (RFC822.TEXT is returned).
                  */

                  IMAPFetchParser::BodyPart oPart = _ParseBODY("BODY[TEXT]");
                  oPart.SetDescription("RFC822.TEXT");
                  m_vecPartsToLookAt.push_back(oPart);

                  break;
               }
         }

         iter++;
      }

      return IMAPResult();
   }

   IMAPFetchParser::BodyPart
   IMAPFetchParser::_ParseBODY(const String &sString)
   {
      BodyPart oPart;
      
      // Set the description.

      String sNewName = sString;
      sNewName.ReplaceNoCase(_T("BODY.PEEK["), _T("BODY["));

      oPart.SetDescription(sNewName);

      // Locate the start of the peek-part.
      long lBodyStart = sNewName.Find(_T("[")) + 1;
      
      // Locate the end of the part.
      long lBodyEnd = sNewName.Find(_T("]"), lBodyStart) - 1;

      if (sNewName.Find(_T("<"), lBodyEnd) == lBodyEnd +2)
      {
         int iStart = lBodyEnd+3;
         int iEnd = sNewName.Find(_T(">"), iStart);

         String sPartial = sNewName.Mid(iStart, iEnd - iStart);
         int iDotPos = sPartial.Find(_T("."));
         
         oPart.m_iOctetStart = _ttoi(sPartial.Mid(0, iDotPos));
         oPart.m_iOctetCount = _ttoi(sPartial.Mid(iDotPos+1));

         // Remove the octets part from the description.
         String sBefore = sNewName.Mid(0, iStart - 1);
         String sAfter = sNewName.Mid(iEnd + 1);

         String sDescWithoutOctets = sBefore + sAfter;

         oPart.SetDescription(sDescWithoutOctets);
      }

      // Extract the body  part.
      long lBodyLen = lBodyEnd - lBodyStart +1 ;
      String sBody = sNewName.Mid(lBodyStart, lBodyLen);

      if (sBody.IsEmpty())
      {
         oPart.SetShowBodyFull(true);
         m_bSetSeen = true;
      }
      else
      {
         // Determine what to look at.
         long lTemp  = 0;

         // Should we show all header fields except for...
         lTemp = sBody.FindNoCase(_T("HEADER.FIELDS.NOT"));
         if (lTemp >= 0)
         {
            int lStart = sBody.Find(_T("("), lTemp) + 1;
            int lEnd = sBody.Find(_T(")"), lStart) ;
            int lLength = lEnd - lStart;
            
            String sFields = sBody.Mid(lStart, lLength);
            oPart.GetHeaderFieldsNOT() = StringParser::SplitString(sFields, " ");
            oPart.SetShowBodyHeaderFieldsNOT(true);

            // Strip away the header fields part from the Body.
            // If we don't do this, we will parse the same string
            // as header.fields below.
            String sBefore = sBody.Mid(0, lTemp);
            String sAfter = sBody.Mid(lEnd + 2);
            sBody = sBefore + sAfter;

         }

         // Should we show header fields?
         lTemp = sBody.FindNoCase(_T("HEADER.FIELDS"));
         if (lTemp >= 0)
         {
            int lStart = sBody.Find(_T("("), lTemp) + 1;
            int lEnd = sBody.Find(_T(")"), lStart) ;
            int lLength = lEnd - lStart;
            
            String sFields = sBody.Mid(lStart, lLength);
            oPart.GetHeaderFields() = StringParser::SplitString(sFields, " ");
            oPart.SetShowBodyHeaderFields(true);

            // Strip away the header fields part from the Body.
            String sBefore = sBody.Mid(0, lTemp);
            String sAfter = sBody.Mid(lEnd + 2);
            sBody = sBefore + sAfter;
         }

         lTemp = sBody.FindNoCase(_T("HEADER"));
         if (lTemp >= 0)
         {
            oPart.SetShowBodyHeader(true);
            
            String sBefore = sBody.Mid(0, lTemp);
            String sAfter = sBody.Mid(lTemp + 7);
            sBody = sBefore + sAfter;
         }

         lTemp = sBody.FindNoCase(_T("MIME"));
         if (lTemp >= 0)
         {
            oPart.SetShowBodyHeader(true);

            String sBefore = sBody.Mid(0, lTemp);
            String sAfter = sBody.Mid(lTemp + 7);
            sBody = sBefore + sAfter;
         }

         lTemp = sBody.FindNoCase(_T("TEXT"));
         if (lTemp >= 0)
         {
            oPart.SetShowBodyText(true);
            
            String sBefore = sBody.Mid(0, lTemp);
            String sAfter = sBody.Mid(lTemp + 5);
            sBody = sBefore + sAfter;

            m_bSetSeen = true;
         }

         if (!oPart.GetShowBodyText() &&
             !oPart.GetShowBodyHeader() &&
             !oPart.GetShowBodyHeaderFields() &&
             !oPart.GetShowBodyHeaderFieldsNOT())
         {
             oPart.SetShowBodyText(true);
             m_bSetSeen = true;
         }
         
         sBody = sBody.TrimLeft(_T("."));
         sBody = sBody.TrimRight(_T("."));

         oPart.SetName(sBody);
      }

      return oPart;

   }



   
   IMAPFetchParser::ePartType
   IMAPFetchParser::_GetPartType(const String &sPart)
   {
      if (sPart.FindNoCase(_T("BODY.PEEK")) >= 0)
         return BODYPEEK;

      if (sPart.CompareNoCase(_T("BODYSTRUCTURE")) == 0)
         return BODYSTRUCTURE;      

      if (sPart.CompareNoCase(_T("BODY")) == 0)
         return BODYSTRUCTURENONEXTENSIBLE;

      if (sPart.FindNoCase(_T("BODY")) >= 0)
         return BODY;

      if (sPart.CompareNoCase(_T("ENVELOPE")) == 0)
         return ENVELOPE;      

      if (sPart.CompareNoCase(_T("RFC822.SIZE")) == 0)
         return RFC822SIZE;         

      if (sPart.CompareNoCase(_T("UID")) == 0)
         return UID;  

      if (sPart.CompareNoCase(_T("FLAGS")) == 0)
         return FLAGS;  

      if (sPart.CompareNoCase(_T("INTERNALDATE")) == 0)
         return INTERNALDATE;  

      if (sPart.CompareNoCase(_T("RFC822")) == 0)
         return RFC822;  

      if (sPart.CompareNoCase(_T("ALL")) == 0)
         return ALL;  

      if (sPart.CompareNoCase(_T("FAST")) == 0)
         return FAST;  

      if (sPart.CompareNoCase(_T("FULL")) == 0)
         return FULL;  

      if (sPart.CompareNoCase(_T("RFC822.HEADER")) == 0)
         return RFC822HEADER;  

      if (sPart.CompareNoCase(_T("RFC822.TEXT")) == 0)
         return RFC822TEXT;  

      return PARTUNKNOWN;
   }


   IMAPFetchParser::BodyPart
   IMAPFetchParser::_ParseBODYPEEK(const String &sString)
   {
      BodyPart oPart = _ParseBODY(sString);
      m_bSetSeen = false;

      return oPart;
   }

   bool 
   IMAPFetchParser::_IsPartSpecifier(const String &sString)
   {
      String sTemp = sString;
      sTemp.ToUpper();
      sTemp.Replace(_T("."), _T(""));
      sTemp.Replace(_T("MIME"), _T(""));
      sTemp.Replace(_T("HEADER"), _T(""));
      sTemp.Replace(_T("TEXT"), _T(""));

      if (sTemp.GetLength() == 0)
         return false;

      if (StringParser::IsNumeric(sTemp))      
         return true;

      return false;

      
      
   }
}

