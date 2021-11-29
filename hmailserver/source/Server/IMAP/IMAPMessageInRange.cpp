// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPMessageInRange.h"

#include "../Common/BO/Message.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPMessageInRange::IMAPMessageInRange() :
      messageUid_(0),
      messageIndex_(0)
   {
      
   }
   void
   IMAPMessageInRange::Set(std::shared_ptr<Message> message, unsigned int messageUid, unsigned int messageIndex)
   {
      message_ = message;
      messageUid_ = messageUid;
      messageIndex_ = messageIndex;
   }
}
