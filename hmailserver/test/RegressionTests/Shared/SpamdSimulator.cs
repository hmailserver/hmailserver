// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.IO;
using System.Text;
using System.Threading;
using hMailServer;

namespace RegressionTests.Shared
{
   /// <summary>
   ///    Simulates spamd. Reads one request and replies with a fixed response.
   /// </summary>
   public class SpamdSimulator : TcpServer
   {
      private readonly string[] _responseParts;

      // Each response part is sent separately, with a pause in between, so that
      // hMailServer receives them in separate reads.
      public SpamdSimulator(int port, params string[] responseParts) :
         base(1, port, eConnectionSecurity.eCSNone)
      {
         _responseParts = responseParts;
      }

      public string Request { get; private set; }

      // True if hMailServer closed the connection within 10 seconds of receiving the response.
      public bool ClientClosedConnection { get; private set; }

      protected override void HandleClient()
      {
         // hMailServer shuts down its sending side once the whole message has been sent.
         var request = new StringBuilder();
         string data;
         while ((data = Receive()) != "")
            request.Append(data);

         Request = request.ToString();

         try
         {
            for (var i = 0; i < _responseParts.Length; i++)
            {
               if (i > 0)
                  Thread.Sleep(500);

               Send(_responseParts[i]);
            }
         }
         catch (IOException)
         {
            // hMailServer closed the connection before the whole response was sent.
         }

         ClientClosedConnection = _tcpConnection.WaitForDisconnect(TimeSpan.FromSeconds(10));
      }
   }
}
