// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "ClassTester.h"
#include "../Application/BackupManager.h"
#include "../Application/TimeoutCalculator.h"
#include "../BO/MessageData.h"
#include "../BO/Message.h"
#include "../Mime/MimeTester.h"
#include "../Util/Utilities.h"
#include "../Util/MessageUtilities.h"
#include "../Util/Charset.h"
#include "../Util/RegularExpression.h"
#include "../TCPIP/LocalIPAddresses.h"
#include "Time.h"
#include "Utilities.h"
#include "Parsing\AddresslistParser.h"
#include "../../IMAP/IMAPSimpleCommandParser.h"
#include "BlowFish.h"
#include "../Persistence/PersistentMessage.h"
#include "../../SMTP/SPF/SPF.h"
#include "../../SMTP/BLCheck.h"
#include "../Application/BackupManager.h"
#include "../Util/Encoding/Base64.h"
#include "../Util/Encoding/ModifiedUTF7.h"
#include "../Util/Hashing/HashCreator.h"
#include "../Util/EventTester.h"
#include <boost/pool/object_pool.hpp>

#ifdef _DEBUG
   #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
   #define new DEBUG_NEW
#endif

namespace HM
{

   ClassTester::ClassTester()
   {

   }

   ClassTester::~ClassTester()
   {

   }

   void
   ClassTester::DoTests()
   {
      EventTester *pEventTester = new EventTester;
      pEventTester->Test();
      delete pEventTester;

      OutputDebugString(_T("hMailServer: Testing mime parser\n"));
	   MimeTester *pMimeTester = new MimeTester;
      pMimeTester->TestFolder("C:\\Temp\\Testdata\\martin");
	   delete pMimeTester;

      OutputDebugString(_T("hMailServer: Testing StringParser\n"));
      StringParserTester *pParser = new StringParserTester();
      pParser->Test();
      delete pParser;
  
      OutputDebugString(_T("hMailServer: Testing FileUtilities\n"));
      FileUtilitiesTester fileUtilitiesTester;
      fileUtilitiesTester.Test();

      OutputDebugString(_T("hMailServer: Testing MessageUtilities\n"));
      MessageUtilitiesTester messageUtilitiesTester;
      messageUtilitiesTester.Test();

      OutputDebugString(_T("hMailServer: Testing Formatter\n"));
      FormatterTester formatterTester;
      formatterTester.Test();

      OutputDebugString(_T("hMailServer: Testing TimeoutCalculator\n"));
      TimeoutCalculatorTester timeoutCalculatortester;
      timeoutCalculatortester.Test();

      OutputDebugString(_T("hMailServer: Test DateTime\n"));
      DateTimeTests tests;
      tests.Test();

      OutputDebugString(_T("hMailServer: Testing Utilities\n"));
      UtilitiesTester *pTestUtilities = new UtilitiesTester();
      pTestUtilities->Test();
      delete pTestUtilities;

      OutputDebugString(_T("hMailServer: Test BLChecktester\n"));
      BLCheckTester blchecktester;
      blchecktester.Test();

      OutputDebugString(_T("hMailServer: Testing SPF\n"));
      SPFTester *pSPF = new SPFTester();
      pSPF->Test();
      delete pSPF;

      OutputDebugString(_T("hMailServer: Testing SHA256\n"));
      HashCreatorTester tester;
      tester.Test();


      OutputDebugString(_T("hMailServer: Testing RegularExpressionTester\n"));
      RegularExpressionTester *pRegExTest = new RegularExpressionTester();
      pRegExTest->Test();
      delete pRegExTest;

      OutputDebugString(_T("hMailServer: Testing Base64\n"));
      Base64Tester base64Tester;
      base64Tester.Test();

      OutputDebugString(_T("hMailServer: Testing Base64\n"));
      ModifiedUTF7Tester modifiedUTF7Tester;
      modifiedUTF7Tester.Test();

      OutputDebugString(_T("hMailServer: Testing AddresslistParser\n"));
      AddresslistParserTester *pTest2 = new AddresslistParserTester();
      pTest2->Test();
      delete pTest2;

      OutputDebugString(_T("hMailServer: Testing charset\n"));
      CharsetTester *pCharsetTester = new CharsetTester;
      pCharsetTester->Test();
      delete pCharsetTester;


      OutputDebugString(_T("hMailServer: Testing LocalIPAddresses\n"));
      LocalIPAddressesTester *pTest4 = new LocalIPAddressesTester();
      pTest4->Test();
      delete pTest4;

      OutputDebugString(_T("hMailServer: Testing MessageData\n"));
      MessageDataTester *pMsgData = new MessageDataTester();
      pMsgData->Test();
      delete pMsgData;

      OutputDebugString(_T("hMailServer: Testing Time\n"));
      TimeTester *pTimeT = new TimeTester();
      pTimeT->Test();
      delete pTimeT;




      OutputDebugString(_T("hMailServer: Testing BlowFishEncryptorTester\n"));
      BlowFishEncryptorTester *pTest3 = new BlowFishEncryptorTester();
      pTest3->Test();
      delete pTest3;

      OutputDebugString(_T("hMailServer: Testing IMAPSimpleCommandParserTester\n"));
      IMAPSimpleCommandParserTester *pTest = new IMAPSimpleCommandParserTester();
      pTest->Test();
      delete pTest;

      

   }

   void 
   ClassTester::LoadSettings_()
   {
      String sAppPath = Utilities::GetBinDirectory();
      if (sAppPath.Right(1) != _T("\\"))
         sAppPath += _T("\\");

      String sConfigFile = sAppPath + "test_config.xml";
      String sTestSpec = FileUtilities::ReadCompleteTextFile(sConfigFile);

      if (sTestSpec.IsEmpty())
         return;

      XDoc oDoc;
      oDoc.Load(sTestSpec);

      XNode *pBackupNode = oDoc.GetChild(_T("Config"));

      if (!pBackupNode)
         throw;

      mime_data_path_ = pBackupNode->GetChildValue(_T("MimeDataPath"));
   }

   void 
   ClassTester::TestBackup_()
   {
      std::shared_ptr<BackupManager> pBackupManager = Application::Instance()->GetBackupManager();
      std::shared_ptr<Backup> pBackup = pBackupManager->LoadBackup("C:\\Temp\\Backup\\HMBackup 2006-12-10 091555.zip");
      pBackup->SetRestoreOptions(1 | 2 | 4 | 8 | 16 | 32);
      pBackupManager->StartRestore(pBackup);

      Sleep(1000000);
   }
}
