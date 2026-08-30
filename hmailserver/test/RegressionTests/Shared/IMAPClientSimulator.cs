// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Infrastructure;

namespace RegressionTests.Shared
{
   /// <summary>
   ///    Summary description for IMAPClientSimulator.
   /// </summary>
   public class ImapClientSimulator
   {
      private readonly int _port = 143;
      private readonly TcpConnection _tcpConnection;

      public ImapClientSimulator()
      {
         _tcpConnection = new TcpConnection();
      }

      public ImapClientSimulator(string username, string password, string mailbox)
      {
         _tcpConnection = new TcpConnection();

         Assert.IsTrue(ConnectAndLogon(username, password));
         Assert.IsTrue(SelectFolder(mailbox));
      }

      public ImapClientSimulator(bool useSSL, int port)
      {
         _tcpConnection = new TcpConnection(useSSL);
         _port = port;
      }


      public bool TestConnect(int iPort)
      {
         var bRetVal = _tcpConnection.Connect(iPort);
         _tcpConnection.Disconnect();
         return bRetVal;
      }

      public static bool ValidatePassword(string username, string password)
      {
         var client = new ImapClientSimulator();
         return client.ConnectAndLogon(username, password);
      }

      public string Connect()
      {
         _tcpConnection.Connect(_port);
         var sData = _tcpConnection.Receive();

         return sData;
      }

      public string GetWelcomeMessage()
      {
         var result = Connect();
         Disconnect();
         return result;
      }

      public void Disconnect()
      {
         _tcpConnection.Disconnect();
      }

      public bool Logout()
      {
         _tcpConnection.Send("A99 LOGOUT\r\n");
         var sData = _tcpConnection.Receive();

         if (sData.StartsWith("*"))
            return true;
         return false;
      }

      public bool Logon(string sUsername, string sPassword, out string errorMessage)
      {
         errorMessage = SendSingleCommand("A01 LOGIN " + sUsername + " \"" + sPassword + "\"");

         return errorMessage.StartsWith("A01 OK");
      }

      public bool Logon(string sUsername, string sPassword)
      {
         string errorMessage;
         return Logon(sUsername, sPassword, out errorMessage);
      }

      public void LogonWithLiteral(string sUsername, string sPassword)
      {
         _tcpConnection.Send("A01 LOGIN " + sUsername + " {" + sPassword.Length + "}\r\n");
         var sData = _tcpConnection.Receive();

         if (sData.IndexOf("+ Ready") != 0)
            Assert.Fail("Literal ready not received.");

         _tcpConnection.Send(sPassword + "\r\n");

         sData = _tcpConnection.Receive();
         if (sData.StartsWith("A01 NO") || sData.StartsWith("+ Ready"))
            Assert.Fail("Logon failed");

         // Logon using two literals.

         _tcpConnection.Send("A02 LOGIN {" + sUsername.Length + "}\r\n");
         sData = _tcpConnection.Receive();

         if (sData.IndexOf("+ Ready") != 0)
            Assert.Fail("Literal ready not received.");

         _tcpConnection.Send(sUsername + " {" + sPassword.Length + "}\r\n");
         sData = _tcpConnection.Receive();

         if (sData.IndexOf("+ Ready") != 0)
            Assert.Fail("Literal ready not received.");

         _tcpConnection.Send(sPassword + "\r\n");

         sData = _tcpConnection.Receive();
         if (sData.StartsWith("A02 NO") || sData.StartsWith("+ Ready"))
            Assert.Fail("Logon failed.");
      }

      public bool CreateFolder(string sFolder)
      {
         sFolder = sFolder.Replace("\\", "\\\\");
         sFolder = sFolder.Replace("\"", "\\\"");

         var result = SendSingleCommand("A03 CREATE \"" + sFolder + "\"");

         if (result.StartsWith("A03 OK"))
            return true;
         return false;
      }

      public bool SetACL(string sFolder, string identifier, string access)
      {
         var command = string.Format("A04 SETACL {0} {1} {2}\r\n",
            sFolder,
            identifier,
            access
         );
         _tcpConnection.Send(command);
         var result = _tcpConnection.Receive();

         return result.StartsWith("A04 OK");
      }

      public bool DeleteACL(string sFolder, string identifier)
      {
         var command = string.Format("A05 DELETEACL \"{0}\" {1}\r\n",
            sFolder,
            identifier);
         _tcpConnection.Send(command);
         var result = _tcpConnection.Receive();

         return result.StartsWith("A05 OK");
      }

      public string GetACL(string sFolder)
      {
         var command = string.Format("A06 GETACL \"{0}\"\r\n",
            sFolder);
         _tcpConnection.Send(command);
         var result = _tcpConnection.Receive();

         return result;
      }

      public string GetMyRights(string sFolder)
      {
         var command = string.Format("A07 MYRIGHTS \"{0}\"\r\n",
            sFolder);
         _tcpConnection.Send(command);
         var result = _tcpConnection.Receive();

         return result;
      }


      public string Status(string folderName, string dataItem)
      {
         var command = string.Format("A08 STATUS \"{0}\" ({1})", folderName, dataItem);

         return SendSingleCommand(command);
      }


      public string GetQuota(string folderName)
      {
         var command = string.Format("A09 GETQUOTA \"{0}\"", folderName);

         return SendSingleCommand(command);
      }

      public string ListRights(string sFolder, string identifier)
      {
         var command = string.Format("A10 LISTRIGHTS \"{0}\" \"{1}\"\r\n",
            sFolder, identifier);
         _tcpConnection.Send(command);
         var result = _tcpConnection.Receive();

         return result;
      }

      public bool Subscribe(string sFolder)
      {
         _tcpConnection.Send("A11 SUBSCRIBE " + sFolder + "\r\n");
         var result = _tcpConnection.Receive().Substring(0, 6);

         if (result.StartsWith("A11 OK"))
            return true;
         return false;
      }

      public bool Unsubscribe(string sFolder)
      {
         _tcpConnection.Send("A12 UNSUBSCRIBE \"" + sFolder + "\"\r\n");
         var result = _tcpConnection.Receive().Substring(0, 6);

         if (result.StartsWith("A12 OK"))
            return true;
         return false;
      }


      public bool CheckFolder(string sFolder)
      {
         _tcpConnection.Send("A13 CHECK " + sFolder + "\r\n");
         var result = _tcpConnection.Receive().Substring(0, 6);

         return result.StartsWith("A13 OK");
      }

      public bool Close()
      {
         _tcpConnection.Send("A14 CLOSE\r\n");
         var result = _tcpConnection.ReadUntil(new List<string> { "A14 BAD", "A14 OK" });

         if (result.Contains("A14 BAD"))
            return false;
         if (result.Contains("A14 OK"))
            return true;

         Assert.Fail("IMAPClientSimulator.Close() - Expected BAD/OK, received: \"{0}\"", result);
         return false;
      }

      public bool SelectFolderWithoutLiteral(string sFolder)
      {
         sFolder = sFolder.Replace("\\", "\\\\");
         sFolder = sFolder.Replace("\"", "\\\"");
         var sData = SendSingleCommand("A15 SELECT " + sFolder);
         return sData.StartsWith("*");
      }

      public bool SelectFolder(string folderName, out string text)
      {
         var sData = Send("A16 SELECT {" + folderName.Length + "}");

         if (sData.IndexOf("+ Ready") != 0)
         {
            var message = string.Format("Literal request not received from server. Time: {0}, Response: {1}",
               DateTime.Now.ToShortDateString(),
               sData);

            Assert.Fail(message);
         }

         _tcpConnection.Send(folderName + "\r\n");

         text = _tcpConnection.Receive();
         return text.StartsWith("*");
      }


      public bool SelectFolder(string sFolder)
      {
         var result = string.Empty;
         SelectFolder(sFolder, out result);
         return result.StartsWith("*");
      }

      public string Fetch(string sParameters)
      {
         var sData = SendSingleCommand("A17 FETCH " + sParameters);

         return sData;
      }

      public string GetFlags(int messageIndex)
      {
         return Fetch(messageIndex + " FLAGS");
      }

      public bool Copy(int messageIndex, string destinationFolder)
      {
         var sData = SendSingleCommand("A18 COPY 1 \"" + destinationFolder + "\"");
         return sData.StartsWith("A18 OK");
      }

      public bool RenameFolder(string from, string to)
      {
         string result;
         return RenameFolder(from, to, out result);
      }

      public bool RenameFolder(string from, string to, out string result)
      {
         result = SendSingleCommand(string.Format("A19 RENAME \"{0}\" \"{1}\"", from, to));
         return result.StartsWith("A19 OK");
      }

      public string ExamineFolder(string sFolder)
      {
         var sData = SendSingleCommand("A20 EXAMINE " + sFolder);
         return sData;
      }


      public bool DeleteFolder(string sFolder)
      {
         _tcpConnection.Send("A21 DELETE " + sFolder + "\r\n");
         var sData = _tcpConnection.Receive();

         if (sData.StartsWith("A21 OK"))
            return true;
         return false;
      }

      public bool SetFlagOnMessage(int index, bool bSet, string sFlag)
      {
         var sSetUnset = bSet ? "+" : "-";
         var sData = "A22 STORE " + index + " " + sSetUnset + "FLAGS (" + sFlag + ")";
         var result = SendSingleCommand(sData);

         if (result.Contains("A22 OK"))
            return true;
         return false;
      }

      public void SetFlagOnFirstMessage(bool bSet, string sFlag)
      {
         SetFlagOnMessage(1, bSet, sFlag);
      }

      public string Sort(string sSearchString)
      {
         var sData = SendSingleCommand("A23 SORT " + sSearchString);

         var iStart = sData.IndexOf(" ", 4) + 1;
         var iLineEnd = sData.IndexOf("\r\n");

         if (iStart > iLineEnd)
            return "";

         var iLength = iLineEnd - iStart;
         var sMatch = sData.Substring(iStart, iLength);

         return sMatch;
      }

      public string Search(string sSearchString)
      {
         var sData = SendSingleCommand("A24 SEARCH " + sSearchString);

         var iStart = sData.IndexOf(" ", 4) + 1;
         var iLineEnd = sData.IndexOf("\r\n");

         if (iStart > iLineEnd)
            return "";

         var iLength = iLineEnd - iStart;
         var sMatch = sData.Substring(iStart, iLength);

         return sMatch;
      }

      public bool StartIdle()
      {
         _tcpConnection.Send("A25 IDLE\r\n");
         var sData = _tcpConnection.Receive();
         return sData.StartsWith("+ idling");
      }

      public bool EndIdle(bool force, out string output)
      {
         output = string.Empty;

         if (force == false) output = _tcpConnection.Receive();

         _tcpConnection.Send("DONE\r\n");

         for (var i = 0; i < 10; i++)
         {
            output += _tcpConnection.Receive();

            if (output.Contains("OK IDLE terminated"))
               return true;
         }

         return false;
      }

      public bool GetPendingDataExists()
      {
         return _tcpConnection.Peek();
      }

      /// <summary>
      ///    Waits for data a total of 8 seconds.
      /// </summary>
      /// <returns>true if data exists</returns>
      public bool AssertPendingDataExists()
      {
         for (var i = 0; i < 40; i++)
         {
            if (GetPendingDataExists())
               return true;

            Thread.Sleep(200);
         }

         return false;
      }

      public bool ConnectAndLogon(string sUsername, string sPassword, out string errorMessage)
      {
         errorMessage = "";
         if (!Connect().StartsWith("* OK"))
            return false;

         return Logon(sUsername, sPassword, out errorMessage);
      }

      public bool ConnectAndLogon(string username, string password)
      {
         string errorMessage;

         return ConnectAndLogon(username, password, out errorMessage);
      }

      public string List(string wildcard)
      {
         return List(wildcard, true);
      }

      public string List(string wildcard, bool unescapeResponse)
      {
         var result = SendSingleCommand("A26 LIST \"\" \"" + wildcard + "\"");

         if (unescapeResponse)
         {
            result = result.Replace("\\\\", "\\");
            result = result.Replace("\\\"", "\"");
         }

         return result;
      }

      public string List(string reference, string wildcard, bool unescapeResponse)
      {
         var result = SendSingleCommand("A27 LIST \"" + reference + "\" \"" + wildcard + "\"");

         if (unescapeResponse)
         {
            result = result.Replace("\\\\", "\\");
            result = result.Replace("\\\"", "\"");
         }

         return result;
      }

      public string List()
      {
         return List("*");
      }

      public string LSUB(string wildcard)
      {
         return LSUB("", wildcard);
      }

      public string LSUB(string reference, string wildcard)
      {
         var result = SendSingleCommand("A28 LSUB \"" + reference + "\" \"" + wildcard + "\"");

         result = result.Replace("\\\\", "\\");
         result = result.Replace("\\\"", "\"");

         return result;
      }

      public string LSUB()
      {
         return LSUB("*");
      }

      public bool SetDeletedFlag(int messageIndex)
      {
         var result = SendSingleCommand("A29 STORE " + messageIndex + " +FLAGS (\\Deleted)");
         return result.StartsWith("*");
      }

      public bool SetSeenFlag(int messageIndex)
      {
         var result = SendSingleCommand("A30 STORE " + messageIndex + " +FLAGS (\\Seen)");
         return result.StartsWith("*");
      }

      public bool Expunge()
      {
         string result;
         return Expunge(out result);
      }

      public bool Expunge(out string result)
      {
         result = SendSingleCommand("A31 EXPUNGE");
         return result.StartsWith("*");
      }

      public string GetCapabilities()
      {
         // Capability
         _tcpConnection.Send("A32 CAPABILITY\r\n");
         var sData = _tcpConnection.Receive();
         return sData;
      }

      public int GetMessageCount(string sFolder)
      {
         var sData = SendSingleCommand("A33 SELECT " + sFolder);

         if (!sData.Contains("A33 OK"))
            throw new ArgumentException("The folder " + sFolder + " was not selectable. Result: " + sData);

         var iStartPos = 2;
         var iEndPos = sData.IndexOf(" ", iStartPos);
         var iLength = iEndPos - iStartPos;

         if (iLength == 0) Assert.Fail("Unparseable SELECT response");

         var sValue = sData.Substring(iStartPos, iLength);

         return Convert.ToInt32(sValue);
      }

      public string Send(string s)
      {
         // Capability
         _tcpConnection.Send(s + "\r\n");
         var sData = _tcpConnection.Receive();
         return sData;
      }

      public void SendRaw(string s)
      {
         _tcpConnection.Send(s);
      }

      public string SendSingleCommand(string command)
      {
         return SendSingleCommandWithLiteral(command, "Some data");
      }

      public string SendSingleCommandWithLiteral(string command, string literalData)
      {
         var commandName = command.Substring(0, command.IndexOf(" "));

         var result = Send(command);

         var expectingLiteral = result.StartsWith("+ Ready");

         var startTime = DateTime.Now;

         // If the commandName is found in the middle of the stream, we shouldn't consider
         // the command completed. Otherwise this code will fail if the subject or message
         // ID happens to contain the command name.
         while ((!result.StartsWith(commandName) && !result.Contains("\r\n" + commandName)) ||
                GetPendingDataExists())
         {
            if (expectingLiteral)
               result = Send(literalData);
            else
               result += Receive();

            if (!_tcpConnection.IsConnected)
               return result;

            if (DateTime.Now - startTime > new TimeSpan(0, 0, 30))
               Assert.Fail("Timeout while waiting for data.");
         }

         return result;
      }


      public string Receive()
      {
         return _tcpConnection.Receive();
      }

      public string ReceiveUntil(string characters)
      {
         return _tcpConnection.ReadUntil(characters);
      }

      private void AssertFolderExists(string folderName)
      {
         // wait for the folder to appear.
         if (Poll.Until(TimeSpan.FromSeconds(25), () => SelectFolder(folderName)))
            return;

         Assert.Fail("Folder not found: " + folderName);
      }


      public static void AssertMessageCount(string accountName, string accountPassword, string folderName,
         int expectedCount)
      {
         if (expectedCount == 0)
            // make sure that we aren't currently delivering messages.
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         var imap = new ImapClientSimulator();
         Assert.IsTrue(imap.ConnectAndLogon(accountName, accountPassword));

         if (expectedCount != 0)
            imap.AssertFolderExists(folderName);

         var currentCount = 0;

         Poll.Until(TimeSpan.FromSeconds(25), () =>
         {
            currentCount = imap.GetMessageCount(folderName);
            return currentCount >= expectedCount;
         });

         if (currentCount == expectedCount)
         {
            imap.Disconnect();
            return;
         }

         imap.Disconnect();

         var error = "Wrong number of messages in mailbox " + folderName + " in account " + accountName +
                     " Actual: " + currentCount + " Expected: " + expectedCount;
         Assert.Fail(error);
      }

      public string NOOP()
      {
         return SendSingleCommand("A34 NOOP");
      }

      public void Handshake()
      {
         _tcpConnection.HandshakeAsClient();
      }
   }
}