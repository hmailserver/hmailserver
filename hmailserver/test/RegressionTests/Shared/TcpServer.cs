// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using NUnit.Framework;

namespace RegressionTests.Shared
{
   internal class TcpServer : IDisposable
   {
      private readonly ManualResetEvent _listenThreadStarted = new ManualResetEvent(false);
      private readonly ManualResetEvent _clientCompleted = new ManualResetEvent(false);

      private readonly ManualResetEvent _workerThreadFinished = new ManualResetEvent(false);
      private readonly ManualResetEvent _stopServerEvent = new ManualResetEvent(false);

      private readonly int _maxNumberOfConnections;
      private readonly int _port;

      private int _numberOfConnectedClients;
      private Thread _serverThread;
      private TcpListener _tcpListener;
      private Exception _workerThreadException;
      private TcpSocket _socket;

      private string _conversation;

      public TcpServer(int maxNumberOfConnections, int port)
      {
         _maxNumberOfConnections = maxNumberOfConnections;
         _port = port;

         SecondsToWaitBeforeTerminate = 15;
      }

      public int SecondsToWaitBeforeTerminate { get; set; }

      public void StartListen()
      {
         Trace.WriteLine("Starting listen...");

         _listenThreadStarted.Reset();
                 

         _serverThread = new Thread(StartListenWorker);
         _serverThread.Name = "Listen thread";
         _serverThread.Start();

         // Don't return until we have started to listen. This is done to prevent
         // someone from trying to continue the test before we're ready.
         Assert.IsTrue(_listenThreadStarted.WaitOne(TimeSpan.FromSeconds(15), false));

         if (_workerThreadException != null)
            throw _workerThreadException;
      }

      private void StartListenWorker()
      {
         try
         {
            var local = IPAddress.Parse("0.0.0.0");

            _tcpListener = new TcpListener(local, _port)
               {
                  ExclusiveAddressUse = false
               };

            _tcpListener.Start();

            HandleClients();
         }
         catch (Exception e)
         {
            _workerThreadException = e;
         }
         finally
         {
            _listenThreadStarted.Set();
            _workerThreadFinished.Set();

            _tcpListener.Stop();
         }
      }

      private void HandleClients()
      {
         for (int i = 0; i < _maxNumberOfConnections; i++)
         {
            _clientCompleted.Reset();

            try
            {
               _tcpListener.BeginAcceptSocket(OnAcceptSocket, null);
            }
            finally
            {
               _listenThreadStarted.Set();
            }

            WaitHandle.WaitAny(new WaitHandle[]
               {
                  _stopServerEvent,
                  _clientCompleted
               });

            if (_stopServerEvent.WaitOne(TimeSpan.FromMilliseconds(1), false))
               return;
         }
      }

      private void OnAcceptSocket(IAsyncResult asyncResult)
      {
         try
         {
            _socket = null;

            try
            {
               _socket = new TcpSocket(_tcpListener.EndAcceptSocket(asyncResult));
            }
            catch (ObjectDisposedException)
            {
               return;
            }

            _numberOfConnectedClients++;

            HandleClient();
         }
         finally
         {
            DisposeSocket();

            _clientCompleted.Set();
         }
      }

      private void DisposeSocket()
      {
         if (_socket != null)
         {
            _socket.Dispose();
            _socket = null;
         }   
      }

      protected virtual void HandleClient()
      {

      }
      
      public void WaitForCompletion()
      {
         if (_workerThreadException != null)
            throw new Exception("An error occurred when calling AcceptSocket.", _workerThreadException);

         for (int i = 0; i < SecondsToWaitBeforeTerminate; i++)
         {
            if (_workerThreadFinished.WaitOne(1000, true))
            {
               return;
            }
         }

         string log = TestSetup.ReadCurrentDefaultLog();

         if (_numberOfConnectedClients < _maxNumberOfConnections)
            Assert.Fail(
               string.Format(
                  "Client did not connect to simulated server. Expected connection count: {0}, Actual: {1}\r\nLog:\r\n{2}",
                  _maxNumberOfConnections, _numberOfConnectedClients, log));
         else
            Assert.Fail(
               string.Format(
                  "Client did not disconnect from simulated server. Expected connection count: {0}, Actual: {1}\r\nLog:\r\n{2}",
                  _maxNumberOfConnections, _numberOfConnectedClients, log));
      }

      public void Dispose()
      {
         _stopServerEvent.Set();

         Assert.IsTrue(_workerThreadFinished.WaitOne(TimeSpan.FromSeconds(15), false));

         if (_tcpListener != null)
            _tcpListener.Stop();

         DisposeSocket();
      }

      public void Disconnect()
      {
         _socket.Disconnect();
      }

      public void Send(string s)
      {
         _conversation += s;
         _socket.Send(s);
      }
      
      public string Receive()
      {
         string data = _socket.Receive();
         _conversation += data;
         return data;
      }

      public string ReadUntil(string text)
      {
         string data = _socket.ReadUntil(text);
         _conversation += data;
         return data;
      }

      public string ReadUntil(List<string> possibleReplies)
      {
         string data =_socket.ReadUntil(possibleReplies);
         _conversation += data;
         return data;
      }

      public string Conversation
      {
         get { return _conversation; }
      }
   }
}