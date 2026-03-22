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
   void set_result(boost::optional<boost::system::error_code>* a, boost::system::error_code b) 
   { 
      a->reset(b); 
   } 

   SynchronousConnection::SynchronousConnection(int timeoutSeconds) :
      socket_(io_context_),
      seconds_(timeoutSeconds)
   {
      
   }

   SynchronousConnection::~SynchronousConnection()
   {
      try
      {
         boost::system::error_code err;
         socket_.shutdown(tcp::socket::shutdown_both, err);
         socket_.close(err);
      }
      catch (...)
      {

      }
   }
   
   bool 
   SynchronousConnection::Connect(const AnsiString &hostName, int port)
   {
      tcp::resolver resolver(io_context_);
      boost::system::error_code resolve_error;
      auto endpoints = resolver.resolve(hostName, AnsiString(StringParser::IntToString(port)), resolve_error);
      if (resolve_error)
         return false;

      boost::system::error_code error = boost::asio::error::host_not_found;
      for (auto& endpoint : endpoints)
      {
         socket_.close();
         socket_.connect(endpoint, error);
         if (!error)
            break;
      }

      return !error;
   }

   bool
   SynchronousConnection::Write(const AnsiString &data)
   {
      return Write_((const unsigned char*) data.data(), data.GetLength());

   }

   bool
   SynchronousConnection::Write(const ByteBuffer &data)
   {
      return Write_(data.GetBuffer(), data.GetSize());
   }

   bool 
   SynchronousConnection::Write_(const unsigned char *buf, size_t bufSize)
   {
      try
      {
         boost::optional<error_code> timer_result; 

         // Create the timeout timer.
         boost::asio::steady_timer timer(io_context_);
         timer.expires_after(std::chrono::seconds(seconds_));
         timer.async_wait(std::bind(set_result, &timer_result, std::placeholders::_1)); 

         // Start an asynchronous write.
         boost::asio::streambuf readBuffer;
         boost::optional<error_code> write_result; 
         async_write(socket_, boost::asio::buffer(buf, bufSize), std::bind(set_result, &write_result, std::placeholders::_1));
         io_context_.restart();

         // Wait for data to be written. 
         while (io_context_.run_one()) 
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
         boost::optional<error_code> timer_result; 
         
         // Create the timeout timer.
         boost::asio::steady_timer timer(io_context_);
         timer.expires_after(std::chrono::seconds(seconds_));
         timer.async_wait(std::bind(set_result, &timer_result, std::placeholders::_1));

         // Start an asynchronous read.
         boost::asio::streambuf readBuffer;
         boost::optional<error_code> read_result; 
         async_read_until(socket_, readBuffer, delimiter, std::bind(set_result, &read_result, std::placeholders::_1));
         io_context_.restart();

         // Wait for input. 
         while (io_context_.run_one()) 
         { 
            if (read_result) 
               timer.cancel(); 
         } 

         // Check if an error has been set.
         if (*read_result) 
            return false;

         std::istream is(&readBuffer);

         readData.append((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

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
      socket_.close(err);
   }


}
