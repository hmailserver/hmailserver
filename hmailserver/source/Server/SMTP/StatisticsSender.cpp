// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "StatisticsSender.h"
#include "../Common/Util/HTTPClient.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   StatisticsSender::StatisticsSender()
   {

   }

   StatisticsSender::~StatisticsSender()
   {

   }

   bool
   StatisticsSender::SendStatistics(int iNoOfMessages) const
   {
      String sPage; 
      sPage.Format(_T("/statistics/update.php?hm_version=%s&hm_messages=%d"), Application::Instance()->GetVersionNumber().c_str(), iNoOfMessages);

      AnsiString output;
      HTTPClient Client;
      return Client.ExecuteScript("www.hmailserver.com", sPage, output);

   }


   

}
