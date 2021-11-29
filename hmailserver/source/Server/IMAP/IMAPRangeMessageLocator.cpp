// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPRangeMessageLocator.h"
#include "IMAPMessageInRange.h"
#include "../Common/BO/Message.h"
#include "../Common/BO/Messages.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPRangeMessageLocator::IMAPRangeMessageLocator()
   {
      
   }

   std::vector<IMAPMessageInRange>
   IMAPRangeMessageLocator::GetMessageInRanges(std::shared_ptr<Messages> messages, bool isUid, std::vector<IMAPRange> ranges)
   {
      std::vector<IMAPMessageInRange> result;

      if (isUid)
      {
         for (auto range: ranges)
         {
            auto start = range.GetStart();
            auto end = range.GetEnd();

            if (start != end)
            {
               std::vector<std::shared_ptr<Message>> messagesCopy = messages->GetCopy();

               unsigned int index = 0;
               for (std::shared_ptr<Message> pMessage : messagesCopy)
               {
                  index++;
                  unsigned int uid = pMessage->GetUID();

                  if (uid >= start && uid <= end)
                  {
                     IMAPMessageInRange messageInRange;
                     messageInRange.Set(pMessage, uid, index);
                     result.push_back(messageInRange);
                  }
               }

            }
            else
            {
               unsigned int foundIndex = 0;
               std::shared_ptr<Message> message = messages->GetItemByUID(start, foundIndex);
               if (message)
               {
                  IMAPMessageInRange messageInRange;
                  messageInRange.Set(message, start, foundIndex);
                  result.push_back(messageInRange);
               }
            }
         }

      }
      else
      {
         for (auto range : ranges)
         {
            auto start = range.GetStart();
            auto end = range.GetEnd();

            if (start != end)
            {
               std::vector<std::shared_ptr<Message>> messagesCopy = messages->GetCopy();

               unsigned int index = 0;
               for (std::shared_ptr<Message> message : messagesCopy)
               {
                  index++;

                  if (index >= start && index <= end)
                  {
                     IMAPMessageInRange messageInRange;
                     messageInRange.Set(message, message->GetUID(), index);
                     result.push_back(messageInRange);
                  }
               }

            }
            else
            {
               std::shared_ptr<Message> message = messages->GetItem(start - 1);

               if (message)
               {
                  IMAPMessageInRange messageInRange;
                  messageInRange.Set(message, message->GetUID(), start);
                  result.push_back(messageInRange);
               }
            }
         }

      }

      return result;
   }
}
