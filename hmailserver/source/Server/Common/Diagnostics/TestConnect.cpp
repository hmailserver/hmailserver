// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "TestConnect.h"

using boost::asio::ip::tcp;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   TestConnect::PerformTest(const String  &localAddressStr, const String &server, int port, String &result)
   {
      boost::asio::io_service io_service;

      IPAddress localAddress;
      if (!localAddressStr.IsEmpty())
      {
         if (!localAddress.TryParse(localAddressStr, false))
         {
            result.append(Formatter::Format("ERROR: Unable to parse address {0}.\r\n", localAddressStr));
            return false;
         }
         else
            result.append(Formatter::Format("Local address is {0}.\r\n", localAddress.ToString()));
      }

      result.append(Formatter::Format("Trying to connect to host {0}...\r\n", server));
      

      // Get a list of endpoints corresponding to the server name.
      tcp::resolver resolver(io_service);
      tcp::resolver::query query(AnsiString(server), AnsiString(StringParser::IntToString(port)), tcp::resolver::query::numeric_service);
      boost::system::error_code errorResolve = boost::asio::error::host_not_found;
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, errorResolve);
      tcp::resolver::iterator end;

      if (errorResolve || endpoint_iterator == end)
      {
         // Host was not found.
         String formattedString;
         formattedString.Format(_T("ERROR: The host name %s could not be resolved.\r\n"), server);
         
         result.append(formattedString);
         return false;
      }

      // Try each endpoint until we successfully establish a connection.
      tcp::socket socket(io_service);
      boost::system::error_code error = boost::asio::error::host_not_found;
      while (error && endpoint_iterator != end)
      {
         boost::asio::ip::address adr = (*endpoint_iterator).endpoint().address();

         String ipAddressString = adr.to_string();
         String formattedString;
         formattedString.Format(_T("Trying to connect to TCP/IP address %s on port %d.\r\n"), ipAddressString, port);

         result.append(formattedString);

         socket.close();

         IPAddress emptyAddr;
         bool any = emptyAddr.IsAny();

         if (!localAddress.IsAny())
         {
            socket.open(boost::asio::ip::tcp::v4());
            boost::system::error_code tempError;
            socket.bind(boost::asio::ip::tcp::endpoint(localAddress.GetAddress(), 0), tempError);

            if (tempError)
            {
               result.append(Formatter::Format("ERROR: Unable to bind to address {0}.\r\n", localAddress.ToString()));
               socket.close();
               return false;
            }
         }

         socket.connect(*endpoint_iterator++, error);
      }

      if (error)
      {
         // We were unable to connect.
         result.append(_T("ERROR: It was not possible to connect.\r\n"));
         return false;
      }

      // Read the response status line.
      boost::asio::streambuf response;
      boost::asio::read_until(socket, response, "\r\n");

      std::string s;
      std::istream is(&response);
      std::getline(is, s, '\r');
      result.append(Formatter::Format("Received: {0}.\r\n", String(s)));

      // Close the socket again.
      socket.close();

      result.append(_T("Connected successfully.\r\n"));
      return true;

   }

   
      
}
