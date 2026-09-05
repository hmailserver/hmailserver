// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Security.Cryptography.X509Certificates;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;

namespace RegressionTests.Shared
{
   public class TcpServer : IDisposable
   {
      private readonly ManualResetEvent _clientCompleted = new ManualResetEvent(false);
      private readonly ManualResetEvent _listenThreadStarted = new ManualResetEvent(false);

      private readonly int _maxNumberOfConnections;
      private readonly int _port;
      private readonly ManualResetEvent _stopServerEvent = new ManualResetEvent(false);

      private readonly ManualResetEvent _workerThreadFinished = new ManualResetEvent(false);

      protected eConnectionSecurity _connectionSecurity;

      private X509Certificate2 _localCertificate;

      private int _numberOfConnectedClients;
      private Thread _serverThread;
      protected TcpConnection _tcpConnection;
      private TcpListener _tcpListener;
      private Exception _workerThreadException;

      public TcpServer(int maxNumberOfConnections, int port, eConnectionSecurity connectionSecurity)
      {
         _maxNumberOfConnections = maxNumberOfConnections;
         _port = port;
         _connectionSecurity = connectionSecurity;

         SecondsToWaitBeforeTerminate = 15;
      }

      public int SecondsToWaitBeforeTerminate { get; set; }

      public string Conversation { get; private set; }

      public void Dispose()
      {
         _stopServerEvent.Set();

         Assert.IsTrue(_workerThreadFinished.WaitOne(TimeSpan.FromSeconds(15), false));

         // The listen thread stops the listener before signalling that it has finished.
         DisposeSocket();
      }

      public void SetCertificate(X509Certificate2 certificate)
      {
         _localCertificate = certificate;
      }

      public void StartListen()
      {
         Console.WriteLine("Starting listen...");

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
            // Stop before signalling. TcpListener.Stop is not thread safe, and Dispose
            // continues as soon as _workerThreadFinished is set.
            if (_tcpListener != null)
               _tcpListener.Stop();

            _listenThreadStarted.Set();
            _workerThreadFinished.Set();
         }
      }

      private void HandleClients()
      {
         for (var i = 0; i < _maxNumberOfConnections; i++)
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
            _tcpConnection = null;

            try
            {
               _tcpConnection = new TcpConnection(_tcpListener.EndAcceptTcpClient(asyncResult));
            }
            catch (ObjectDisposedException)
            {
               return;
            }

            if (_connectionSecurity == eConnectionSecurity.eCSTLS) _tcpConnection.HandshakeAsServer(_localCertificate);

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
         if (_tcpConnection != null)
         {
            _tcpConnection.Dispose();
            _tcpConnection = null;
         }
      }

      protected virtual void HandleClient()
      {
      }

      public void WaitForCompletion()
      {
         if (_workerThreadException != null)
            throw new Exception("An error occurred when calling AcceptSocket.", _workerThreadException);

         for (var i = 0; i < SecondsToWaitBeforeTerminate; i++)
            if (_workerThreadFinished.WaitOne(1000, true))
               return;

         var log = LogHandler.ReadCurrentDefaultLog();

         if (_numberOfConnectedClients < _maxNumberOfConnections)
            Assert.Fail(
               "At {0} - Client did not connect to simulated server. Expected connection count: {1}, Actual: {2}\r\nLog:\r\n{3}",
               DateTime.Now, _maxNumberOfConnections, _numberOfConnectedClients, log);
         else
            Assert.Fail(
               "At {0} - Client did not disconnect from simulated server. Expected connection count: {1}, Actual: {2}\r\nLog:\r\n{3}",
               DateTime.Now, _maxNumberOfConnections, _numberOfConnectedClients, log);
      }

      public void Disconnect()
      {
         _tcpConnection.Disconnect();
      }

      public void Send(string s)
      {
         Conversation += s;
         _tcpConnection.Send(s);
      }

      public string Receive()
      {
         var data = _tcpConnection.Receive();
         Conversation += data;
         return data;
      }

      public string ReadUntil(string text)
      {
         var data = _tcpConnection.ReadUntil(text);
         Conversation += data;
         return data;
      }

      public string ReadUntil(List<string> possibleReplies)
      {
         var data = _tcpConnection.ReadUntil(possibleReplies);
         Conversation += data;
         return data;
      }
   }
}