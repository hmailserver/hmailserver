// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "StringTreeParser.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   StringTreeParser::StringTreeParser(void) :
      c_iCurState(0)
   {
   }

   StringTreeParser::~StringTreeParser(void)
   {

   }
   
 void 
   StringTreeParser::Parse(const String &sCommand, std::vector<std::shared_ptr<StringNode> > & vecNodes)
   {

      StringNode oNode;


      int iPos = 0;
      int iCurWordStart = 0;

      bool bQuit = false;
      while (iPos < sCommand.GetLength())
      {
         wchar_t sChar = sCommand.GetAt(iPos);

         // Is this a paranthezed
         if (sChar == '(')
         {
            iCurWordStart = iPos + 1;
            bool bFoundEnd = false;
            bool bInString = false;
            while (iPos < sCommand.GetLength())
            {
               wchar_t sTmp = sCommand.GetAt(iPos);
               if (sTmp == '"')
                  bInString = !bInString;
               
               if (bInString)
               {
                  // Find the end of the string.
                  iPos++;
                  continue;
               }
               
               if (sTmp == ')')
               {
                  // Found end of this sub string.
                  break;
               }
               
               iPos++;
            }

            String sSubString = sCommand.Mid(iCurWordStart, iPos - iCurWordStart - 1);

            // We have now find something between two parantheses.
            // Split up the string 
         }
         
         if (sChar == '"')
         {
            // Find end of this word.
            iCurWordStart = iPos + 1;

            int iWordEndPos = sCommand.Find('"', iCurWordStart) -1;

            String sSubString = sCommand.Mid(iCurWordStart, iWordEndPos - iCurWordStart);

            // Parse sub string
            Parse(sSubString, vecNodes);
         }

         iPos++;
      }
   }  

}