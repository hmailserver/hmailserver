// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "ServerInfo.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ServerInfo::ServerInfo(bool fixed, const String &hostName, int port, const String&userName, const String &passWord, bool useSSL) :
      _fixed (fixed),
      _hostName(hostName),
      _port(port),
      _userName(userName),
      _passWord(passWord),
      _useSSL(useSSL)
   {

   }

   ServerInfo::~ServerInfo()
   {

   }

   bool
   ServerInfo::GetFixed()
   {
      return _fixed;
   }

   String 
   ServerInfo::GetHostName()
   {
      return _hostName;
   }

   void 
   ServerInfo::SetHostName(const String &hostName)
   {
      _hostName = hostName;
   }

   int 
   ServerInfo::GetPort()
   {
      return _port;
   }

   String 
   ServerInfo::GetUsername()
   {
      return _userName;
   }

   String 
   ServerInfo::GetPassword()
   {
      return _passWord;
   }

   bool 
   ServerInfo::GetUseSSL()
   {
      return _useSSL;
   }

   bool 
   ServerInfo::operator== (const ServerInfo &other) const
   {
      return _fixed == other._fixed &&
         _hostName == other._hostName &&
         _port == other._port &&
         _userName == other._userName &&
         _passWord == other._passWord &&
         _useSSL == other._useSSL;
   }
}
