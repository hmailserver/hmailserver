// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "SynchronousConnection.h"
#include "../Util/ByteBuffer.h"
#include <Boost/optional.hpp>
#include <Boost/system/error_code.hpp>

using namespace boost::system;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   void set_result(optional<boost::system::error_code>* a, boost::system::error_code b) 
   { 
      a->reset(b); 
   } 

   SynchronousConnection::SynchronousConnection(int timeoutSeconds) :
      m_socket(m_ioservice),
      m_timeoutSeconds(timeoutSeconds)
   {
      
   }

   SynchronousConnection::~SynchronousConnection()
   {
      boost::system::error_code err;
      m_socket.shutdown(tcp::socket::shutdown_both, err);
      m_socket.close(err);
   }
   
   bool 
   SynchronousConnection::Connect(const AnsiString &hostName, int port)
   {
      tcp::resolver resolver(m_ioservice);
      tcp::resolver::query query(hostName, 
            AnsiString(StringParser::IntToString(port)), 
            tcp::resolver::query::numeric_service);
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
      tcp::resolver::iterator end;

      boost::system::error_code error = boost::asio::error::host_not_found;
      while (error && endpoint_iterator != end)
      {
         m_socket.close();
         m_socket.connect(*endpoint_iterator++, error);
      }
      
      if (error)
         return false;
      else
         return true;
   }

   bool
   SynchronousConnection::Write(const AnsiString &data)
   {
      return _Write((const unsigned char*) data.data(), data.GetLength());

   }

   bool
   SynchronousConnection::Write(const ByteBuffer &data)
   {
      return _Write(data.GetBuffer(), data.GetSize());
   }

   bool 
   SynchronousConnection::_Write(const unsigned char *buf, int bufSize)
   {
      try
      {
         optional<error_code> timer_result; 

         // Create the timeout timer.
         boost::asio::deadline_timer timer(m_ioservice); 
         timer.expires_from_now(boost::posix_time::seconds(m_timeoutSeconds)); 
         timer.async_wait(boost::bind(set_result, &timer_result, _1)); 

         // Start an asynchronous write.
         boost::asio::streambuf readBuffer;
         optional<error_code> write_result; 
         async_write(m_socket, boost::asio::buffer(buf, bufSize), boost::bind(set_result, &write_result, _1)); 
         m_ioservice.reset(); 

         // Wait for data to be written. 
         while (m_ioservice.run_one()) 
         { 
            if (write_result) 
               timer.cancel(); 
         } 

         // Check if an error has been set.
         if (*write_result) 
            return false;

         return true;
      }
      catch (boost::system::system_error&)
      {
         return false;
      } 
   }

   bool 
   SynchronousConnection::ReadUntil(const AnsiString &delimiter, AnsiString &readData)
   {
      readData.clear();

      try
      {
         optional<error_code> timer_result; 
         
         // Create the timeout timer.
         boost::asio::deadline_timer timer(m_ioservice); 
         timer.expires_from_now(boost::posix_time::seconds(m_timeoutSeconds)); 
         timer.async_wait(boost::bind(set_result, &timer_result, _1)); 

         // Start an asynchronous read.
         boost::asio::streambuf readBuffer;
         optional<error_code> read_result; 
         async_read_until(m_socket, readBuffer, delimiter, boost::bind(set_result, &read_result, _1)); 
         m_ioservice.reset(); 

         // Wait for input. 
         while (m_ioservice.run_one()) 
         { 
            if (read_result) 
               timer.cancel(); 
         } 

         // Check if an error has been set.
         if (*read_result) 
            return false;

         std::istream is(&readBuffer);

         readData.append((istreambuf_iterator<char>(is)), istreambuf_iterator<char>());

         return true;
      }
      catch (boost::system::system_error&)
      {
         return false;
      }  
   }

   void
   SynchronousConnection::Close()
   {
      boost::system::error_code err;
      m_socket.close(err);
   }


}
