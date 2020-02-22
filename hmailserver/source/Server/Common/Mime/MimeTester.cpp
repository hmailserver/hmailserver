// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "MimeTester.h"
#include "Mime.h"
#include "MimeCode.h"
#include "../BO/Message.h"
#include "../BO/MessageData.h"
#include "../Persistence/PersistentMessage.h"

#ifdef _DEBUG
   #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
   #define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   MimeTester::MimeTester(void)
   {
   }

   MimeTester::~MimeTester(void)
   {
   }
   
   bool 
   MimeTester::TestFolder(const String &sFolderName)
   {
      return true;

      String sCleanFolder = sFolderName;
      if (sCleanFolder.Right(1) == _T("\\"))
         sCleanFolder = sCleanFolder.Left(sCleanFolder.GetLength() - 1);

      if (sCleanFolder.Right(1) != _T("\\"))
         sCleanFolder += "\\";

      String sWildCard = sCleanFolder + "*.*";

      // Locate first match
      WIN32_FIND_DATA ffData;
      HANDLE hFileFound = FindFirstFile(sWildCard, &ffData);

      if (hFileFound == INVALID_HANDLE_VALUE)
         return FALSE;

      while (hFileFound && FindNextFile(hFileFound, &ffData))
      {
         String sFullPath = sCleanFolder + ffData.cFileName;

         if (ffData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
         {
            if( (_tcscmp(ffData.cFileName, _T(".")) != 0) &&
               (_tcscmp(ffData.cFileName, _T("..")) != 0) ) 
            {
               if( !TestFolder(sFullPath) )
                  return false;
            }

         }
         else
         { 
            TestLoadFile(sFullPath);
         }
      }

      FindClose(hFileFound);

      return true;
   
   }

   void 
   MimeTester::TestLoadFile(const String &sFilename)
   {
	   try
	   {
         OutputDebugString("Loading file " + sFilename +"\n");

		   std::shared_ptr<Message> pMessage = std::shared_ptr<Message>(new Message(false));

		   std::shared_ptr<MessageData> pMsgData = std::shared_ptr<MessageData>(new MessageData());
		   pMsgData->LoadFromMessage(sFilename, pMessage);
	   }
	   catch (...)
	   {
		   assert(0);
		   MessageBox(0,_T("ERROR"), _T("ERROR"), 0);
		   throw;
	   }
   }

   void 
   MimeTester::TestFile(const String &sFilename)
   {
      try
      {
         std::shared_ptr<Message> pMessage = std::shared_ptr<Message>(new Message(false));
         
         std::shared_ptr<MessageData> pMsgData = std::shared_ptr<MessageData>(new MessageData());
         pMsgData->LoadFromMessage(sFilename, pMessage);

         String sOutput = "hMailServer: [MimeTester] --> "; 
         sOutput += sFilename + " --> "; 
         sOutput += pMsgData->GetSubject();
         sOutput += "\n";
         OutputDebugString(sOutput);

         // Add a message header
         pMsgData->SetFieldValue("X-MyHeader", "ValueOfMyHeader");

         // New message
         std::shared_ptr<Message> pNewMessage = std::shared_ptr<Message>(new Message());
         std::shared_ptr<Account> account;
         String newFileName = PersistentMessage::GetFileName(account, pNewMessage);

         pMsgData->Write(newFileName);

         // Delete the new message.
         FileUtilities::DeleteFile(newFileName);

      }
      catch (...)
      {
         assert(0);
         MessageBox(0,_T("ERROR"), _T("ERROR"), 0);
         throw;
      }
   }

}