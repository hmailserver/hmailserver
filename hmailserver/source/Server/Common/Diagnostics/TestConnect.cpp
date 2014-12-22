// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "TestConnect.h"

#include "TestConnection.h"
#include "TestConnectionResult.h"

#include "../TCPIP/IOService.h"

using boost::asio::ip::tcp;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   TestConnect::PerformTest(ConnectionSecurity connection_security, const String  &localAddressStr, const String &server, int port, String &result)
   {
      std::shared_ptr<IOService> io_service_wrapper = Application::Instance()->GetIOService();

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
      tcp::resolver resolver(io_service_wrapper->GetIOService());
      tcp::resolver::query query(AnsiString(server), AnsiString(StringParser::IntToString(port)), tcp::resolver::query::numeric_service);
      boost::system::error_code errorResolve = boost::asio::error::host_not_found;
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, errorResolve);
      tcp::resolver::iterator end;

      if (errorResolve || endpoint_iterator == end)
      {
         // Host was not found.
         String formattedString;
         formattedString.Format(_T("ERROR: The host name %s could not be resolved.\r\n"), server.c_str());
         
         result.append(formattedString);
         return false;
      }
      
      String last_error_message;
      
      // Try each endpoint until we successfully establish a connection.
      boost::system::error_code error = boost::asio::error::host_not_found;
      while (error && endpoint_iterator != end)
      {
         boost::asio::ip::address adr = (*endpoint_iterator).endpoint().address();

         String ipAddressString = adr.to_string();
         String formattedString;
         formattedString.Format(_T("Trying to connect to TCP/IP address %s on port %d.\r\n"), ipAddressString.c_str(), port);
         result.append(formattedString);


         std::shared_ptr<Event> disconnectEvent = std::shared_ptr<Event>(new Event());
         
         std::shared_ptr<TestConnectionResult> connection_result = std::make_shared<TestConnectionResult>();

         std::shared_ptr<TestConnection> connection = std::make_shared<TestConnection>(connection_security, io_service_wrapper->GetIOService(), io_service_wrapper->GetClientContext(), disconnectEvent, server, connection_result);
         if (connection->Connect(ipAddressString, port, localAddress))
         {
            connection.reset();

            disconnectEvent->Wait();
            
            if (connection_result->GetConnectedSuccesfully())
            {
               result.append(_T("Connected successfully.\r\n"));

               if (connection_security == CSSSL)
               {
                  if (connection_result->GetHandshakeCompletedSuccesfully())
                  {
                     result.append(_T("SSL/TLS handshake completed successfully.\r\n"));
                     return true;
                  }
                  else
                  {
                     result.append(_T("ERROR: Handshake failed.\r\n"));
                     return false;
                  }
               }
               else
               {
                  return true;
               }
            }
            else
            {
               result.append(Formatter::Format("ERROR: It was not possible to connect. Error: {0}.\r\n", connection_result->GetErrorMessage()));
            }
         }

         endpoint_iterator++;
      }

      // We were unable to connect.
      result.append(_T("ERROR: Failed to connect to all servers.\r\n"));
      return false;
   }

   
      
}
