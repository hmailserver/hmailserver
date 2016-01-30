// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Security;
using System.Net.Sockets;
using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading;

namespace RegressionTests.Shared
{
   /// <summary>
   /// Summary description for ClientSocket.
   /// </summary>
   public class TcpConnection : IDisposable
   {
      private bool _useSslSocket;
      private SslProtocols _sslProtocols = SslProtocols.Default;

      public TcpConnection()
      {

      }

      public TcpConnection(bool useSSL) 
         : this(useSSL, SslProtocols.Default)
         
      {
         _useSslSocket = useSSL;
      }

      public TcpConnection(bool useSSL, SslProtocols protocols)
      {
         _useSslSocket = useSSL;
         _sslProtocols = protocols;
      }

      public TcpConnection(TcpClient client)
      {
         _tcpClient = client;
      }

      public bool IsConnected
      {
         get { return _tcpClient != null && _tcpClient.Connected; }
      }

      public bool Connect(int iPort)
      {
         return Connect(IPAddress.Parse("127.0.0.1"), iPort);
      }

      public bool Connect(IPAddress ipaddress, int iPort)
      {
         if (ipaddress == null)
            throw new ArgumentNullException("ipaddress");

         try
         {
            _tcpClient = new TcpClient(ipaddress.AddressFamily);

            var result = _tcpClient.BeginConnect(ipaddress, iPort, null, null);

            var success = result.AsyncWaitHandle.WaitOne(TimeSpan.FromSeconds(10), false);

            if (!success)
            {
               return false;
            }

            if (!_tcpClient.Connected)
            {
               return false;
            }
         }
         catch
         {
            return false;
         }

         _tcpClient.Client.Blocking = true;

         if (_useSslSocket)
            HandshakeAsClient();
         
         return true;
      }

      public bool IsPortOpen(int iPort)
      {
         if (!Connect(iPort))
            return false;

         try
         {
            for (int i = 0; i < 40; i++)
            {
               if (_tcpClient.Available > 0)
                  return true;

               Thread.Sleep(25);
            }
         }
         finally
         {
            Disconnect();
         }

         return false;
      }

      public void Disconnect()
      {
         if (_useSslSocket)
            _sslStream.Close();

         if (_tcpClient != null)
            _tcpClient.Close();
      }

      public void HandshakeAsClient()
      {
         // Create an SSL stream that will close the client's stream.
         _sslStream = new SslStream(_tcpClient.GetStream(), false,
                                    ValidateServerCertificate, null);

         _sslStream.AuthenticateAsClient("localhost", null, _sslProtocols, false);
         
         _useSslSocket = true;
      }

      public void HandshakeAsServer(X509Certificate2 certificate)
      {
         // Create an SSL stream that will close the client's stream.
         _sslStream = new SslStream(_tcpClient.GetStream(), false,
                                    ValidateServerCertificate, null);

         _sslStream.AuthenticateAsServer(certificate, false, _sslProtocols, false);

         _useSslSocket = true;
      }

      public bool IsSslConnection
      {
         get { return _useSslSocket; }
      }

      public string SendAndReceive(string sData)
      {
         Send(sData);
         return Receive();
      }

      public void Send(string s)
      {
         if (!_tcpClient.Connected)
            throw new InvalidOperationException("Connection closed - Unable to send data.");

         if (_useSslSocket)
         {
            var message = Encoding.UTF8.GetBytes(s);
            _sslStream.Write(message);
            _sslStream.Flush();
         }
         else
         {
            var buf = Encoding.UTF8.GetBytes(s);
            var stream = _tcpClient.GetStream();

            stream.Write(buf, 0, buf.Length);
         }
      }

      public string ReadUntil(string text)
      {
         return ReadUntil(text, TimeSpan.FromSeconds(10));
      }

      public string ReadUntil(string text, TimeSpan timeout)
      {
         DateTime stopTime = DateTime.Now + timeout;

         string result = Receive();

         while (DateTime.Now < stopTime)
         {
            if (result.Contains(text))
               return result;

            if (!_tcpClient.Connected)
               return "";

            result += Receive();

            Thread.Sleep(10);
         }

         throw new TimeoutException("Timeout while waiting for server response: " + text);
      }


      public string ReadUntil(List<string> possibleReplies)
      {
         string result = Receive();

         for (int i = 0; i < 1000; i++)
         {
            foreach (string s in possibleReplies)
            {
               if (result.Contains(s))
                  return result;
            }

            Thread.Sleep(10);

            result += Receive();
         }

         throw new InvalidOperationException("Timeout while waiting for server response");
      }

      public string Receive()
      {
         var messageData = new StringBuilder();

         var buffer = new byte[2048];
         int bytesRead;

         if (_useSslSocket)
         {
            do
            {
               if (!_sslStream.CanRead)
                  return "";

               bytesRead = _sslStream.Read(buffer, 0, buffer.Length);
               Decoder decoder = Encoding.UTF8.GetDecoder();
               var chars = new char[decoder.GetCharCount(buffer, 0, bytesRead)];
               decoder.GetChars(buffer, 0, bytesRead, chars, 0);
               messageData.Append(chars);
            } while (_tcpClient.Available > 0);
         }
         else
         {
            do
            {
               var stream = _tcpClient.GetStream();

               if (!stream.CanRead)
                  return "";

               bytesRead = stream.Read(buffer, 0, buffer.Length);
               char[] chars = Encoding.ASCII.GetChars(buffer);
               var s = new string(chars, 0, bytesRead);
               messageData.Append(s);
            } while (_tcpClient.Available > 0);
         }

         return messageData.ToString();

      }

      public bool Peek()
      {
         return _tcpClient.Available > 0;
      }

      private SslStream _sslStream;
      private TcpClient _tcpClient;

      // The following method is invoked by the RemoteCertificateValidationDelegate.
      public static bool ValidateServerCertificate(
         object sender,
         X509Certificate certificate,
         X509Chain chain,
         SslPolicyErrors sslPolicyErrors)
      {
         return true;
      }

      public void Dispose()
      {
         Disconnect();
      }


      public bool TestConnect(int iPort)
      {
         bool bRetVal = Connect(iPort);
         Disconnect();
         return bRetVal;
      }


   }
}