// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Security;
using System.Net.Sockets;
using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading;
using NUnit.Framework;

namespace RegressionTests.Shared
{
   /// <summary>
   /// Summary description for ClientSocket.
   /// </summary>
   public class TcpSocket : IDisposable
   {
      private readonly bool _useSslSocket;
      protected Socket _socket;

      public TcpSocket()
      {

      }
      
      public TcpSocket(Socket socket)
      {
         _socket = socket;
      }

      public TcpSocket(bool useSSL)
      {
         _useSslSocket = useSSL;
      }

      public bool Connected
      {
         get { return _socket.Connected; }
      }

      public bool Connect(int iPort)
      {
         return Connect(null, iPort);
      }

      private IPAddress GetHostAddress(string hostName, bool allowIPv6)
      {
         IPAddress[] addresses = Dns.GetHostEntry(hostName).AddressList;

         foreach (IPAddress address in addresses)
         {
            if (address.AddressFamily == AddressFamily.InterNetworkV6 && allowIPv6)
               return address;
            else if (address.AddressFamily == AddressFamily.InterNetwork)
               return address;
         }

         return null;
      }

      public bool Connect(IPAddress ipaddress, int iPort)
      {
         IPEndPoint endPoint = null;

         if (ipaddress != null)
            endPoint = new IPEndPoint(ipaddress, iPort);
         else
            endPoint = new IPEndPoint(GetHostAddress("localhost", false), iPort);

         if (_useSslSocket)
         {
            _tcpClient = new TcpClient(endPoint.Address.ToString(), iPort);

            // Create an SSL stream that will close the client's stream.
            _sslStream = new SslStream(_tcpClient.GetStream(), false,
                                       ValidateServerCertificate, null);

            try
            {
               _sslStream.AuthenticateAsClient("localhost");
            }
            catch (AuthenticationException)
            {
               return false;
            }

            _tcpClient.Client.Blocking = true;

            return true;
         }
         else
         {
            var tmpS = new Socket(endPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            try
            {
               tmpS.Connect(endPoint);
            }
            catch (SocketException exp)
            {
               int i = exp.ErrorCode;
            }


            if (tmpS.Connected)
            {
               _socket = tmpS;
               _socket.Blocking = true;
               return true;
            }
         }

         return false;
      }

      public bool CanConnect(int iPort)
      {
         if (!Connect(iPort))
            return false;

         try
         {
            for (int i = 0; i < 40; i++)
            {
               if (_socket.Available > 0)
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
         {
            _sslStream.Close();
            _tcpClient.Close();
         }
         else
            _socket.Close();
      }

      public void Send(string s)
      {
         
         if (_useSslSocket)
         {
            Assert.IsTrue(_tcpClient.Connected);

            byte[] message = Encoding.UTF8.GetBytes(s);
            _sslStream.Write(message);
            _sslStream.Flush();
         }
         else
         {
            Assert.IsTrue(_socket.Connected);

            Byte[] buf = Encoding.UTF8.GetBytes(s);
            _socket.Send(buf, buf.Length, 0);
         }
      }

      public string ReadUntil(string text)
      {
         string result = Receive();

         for (int i = 0; i < 1000; i++)
         {
            if (result.Contains(text))
               return result;

            if (!_socket.Connected)
               return "";

            result += Receive();

            Thread.Sleep(10);
         }

         Assert.Fail("Timeout while waiting for server response: " + text);
         return "";
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

         Assert.Fail("Timeout while waiting for server response.");
         return "";
      }

      public string Receive()
      {
         var messageData = new StringBuilder();
         var buffer = new byte[2048];
         int bytes;

         if (_useSslSocket)
         {
            if (!_tcpClient.Connected)
               return "";

            do
            {
               bytes = _sslStream.Read(buffer, 0, buffer.Length);
               Decoder decoder = Encoding.UTF8.GetDecoder();
               var chars = new char[decoder.GetCharCount(buffer, 0, bytes)];
               decoder.GetChars(buffer, 0, bytes, chars, 0);
               messageData.Append(chars);
            } while (_tcpClient.Available > 0);
         }
         else
         {
            if (!_socket.Connected)
               return "";

            do
            {
               bytes = _socket.Receive(buffer, SocketFlags.None);
               char[] chars = Encoding.ASCII.GetChars(buffer);
               var s = new string(chars, 0, bytes);

               messageData.Append(s);
            } while (_socket.Available > 0);
         }

         

         return messageData.ToString();
      }

      public bool Peek()
      {
         if (_useSslSocket)
         {
            if (_tcpClient.Available > 0)
               return true;
            else
               return false;
         }
         else
         {
            if (_socket.Available > 0)
               return true;
            else
               return false;
         }
      }

      #region SSL_LOGICS

      private static Hashtable certificateErrors = new Hashtable();

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

      #endregion

      public void Dispose()
      {
         CloseSocket();
      }

      protected void CloseSocket()
      {
         if (_socket == null)
            return;

         try
         {
            _socket.Shutdown(SocketShutdown.Both);
         }
         catch (ObjectDisposedException)
         {
         }

         _socket.Close();
         _socket = null;
      }
   }
}