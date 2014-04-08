// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ServerInfo
   {
   public:
	   ServerInfo(bool fixed, const String &hostName, int port, const String&userName, const String &passWord, bool useSSL);
	   virtual ~ServerInfo();

      bool GetFixed();
      String GetHostName();
      int GetPort ();
      String GetUsername();
      String GetPassword();
      bool GetUseSSL();
      void SetHostName(const String &hostName);
         
      bool operator== (const ServerInfo &other) const;

   private:

      bool _fixed;
      String _hostName;
      int _port;
      String _userName;
      String _passWord;
      bool _useSSL;

   };
}
