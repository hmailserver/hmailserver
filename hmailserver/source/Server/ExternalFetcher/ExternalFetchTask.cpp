// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\externalfetchtask.h"

#include "ExternalFetch.h"
#include "..\Common\BO\FetchAccount.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ExternalFetchTask::ExternalFetchTask(std::shared_ptr<FetchAccount> pFA) :
      Task("ExternalFetchTask"),
      fetch_account_(pFA),
      lock_(pFA)
   {
   }

   ExternalFetchTask::~ExternalFetchTask(void)
   {
   }

   void
   ExternalFetchTask::DoWork()
   {
      // Do the actual delivery of the message. The account is unlocked when this task is
      // destroyed, also if the fetch fails.
      ExternalFetch oFetcher;
      oFetcher.Start(fetch_account_);
   }

}
