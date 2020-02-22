// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "OutOfMemoryHandler.h"
#include "../../IMAP/IMAPFolderContainer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   _PNH OutOfMemoryHandler::pOriginalNewHandler = 0;
   boost::recursive_mutex _outOfMemoryHandlerMutex;
   
   BYTE * pMemoryChunk;

   OutOfMemoryHandler::OutOfMemoryHandler(void)
   {
   }

   OutOfMemoryHandler::~OutOfMemoryHandler(void)
   {
   }

   int OnOutOfMemory( size_t )
   {
	  // Hopefully the scope is smaller then the buffer we've attempted to allocate
     boost::lock_guard<boost::recursive_mutex> guard(_outOfMemoryHandlerMutex);

	  // Start of by deleting the chunk of memory
	  // to ensure that we got something to work with.
	  delete [] pMemoryChunk;
	  pMemoryChunk = 0;

	  LOG_APPLICATION("OutOfMemoryHandler - hMailServer has run out of memory, clearing caches.");

	  // And now try to free up some memory.
	  bool bCleared = false;

	  bCleared = IMAPFolderContainer::Instance()->Clear() ? true : bCleared;

	  // If memory was cleared, allocate up the memory chunk again,
	  // if we get here a second time.
	  pMemoryChunk = new BYTE[1024 * 1024];

	  // Return 1 if something was removed from cache.
	  return bCleared ? 1 : 0;

   }

   void 
   OutOfMemoryHandler::Initialize()
   {     
	  pMemoryChunk = new BYTE[5 * 1024 * 1024];

      pOriginalNewHandler = _set_new_handler( OnOutOfMemory );
      _set_new_mode(1);
   }

   void 
   OutOfMemoryHandler::Terminate()
   {
      _set_new_handler(pOriginalNewHandler);

      delete [] pMemoryChunk;
   }
}