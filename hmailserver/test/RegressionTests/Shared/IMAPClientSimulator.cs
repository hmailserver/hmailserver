// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Threading;
using Microsoft.SqlServer.Server;
using NUnit.Framework;
using RegressionTests.Infrastructure;

namespace RegressionTests.Shared
{
   /// <summary>
   /// Summary description for IMAPClientSimulator.
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
         bool bRetVal = _tcpConnection.Connect(iPort);
         _tcpConnection.Disconnect();
         return bRetVal;
      }

      public string Connect()
      {
         _tcpConnection.Connect(_port);
         string sData = _tcpConnection.Receive();

         return sData;
      }

      public string GetWelcomeMessage()
      {
         string result = Connect();
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
         string sData = _tcpConnection.Receive();

         if (sData.StartsWith("*"))
            return true;
         else
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
         _tcpConnection.Send("A01 LOGIN " + sUsername + " {" + sPassword.Length.ToString() + "}\r\n");
         string sData = _tcpConnection.Receive();

         if (sData.IndexOf("+ Ready") != 0)
            Assert.Fail("Literal ready not received.");

         _tcpConnection.Send(sPassword + "\r\n");

         sData = _tcpConnection.Receive();
         if (sData.StartsWith("A01 NO") || sData.StartsWith("+ Ready"))
            Assert.Fail("Logon failed");

         // Logon using two literals.

         _tcpConnection.Send("A02 LOGIN {" + sUsername.Length.ToString() + "}\r\n");
         sData = _tcpConnection.Receive();

         if (sData.IndexOf("+ Ready") != 0)
            Assert.Fail("Literal ready not received.");

         _tcpConnection.Send(sUsername + " {" + sPassword.Length.ToString() + "}\r\n");
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

         string result = SendSingleCommand("A03 CREATE \"" + sFolder + "\"");

         if (result.StartsWith("A03 OK"))
            return true;
         else
            return false;
      }

      public bool SetACL(string sFolder, string identifier, string access)
      {
         string command = string.Format("A04 SETACL {0} {1} {2}\r\n",
                                        sFolder,
                                        identifier,
                                        access
            );
         _tcpConnection.Send(command);
         string result = _tcpConnection.Receive();

         return result.StartsWith("A04 OK");
      }

      public bool DeleteACL(string sFolder, string identifier)
      {
         string command = string.Format("A05 DELETEACL \"{0}\" {1}\r\n",
                                        sFolder,
                                        identifier);
         _tcpConnection.Send(command);
         string result = _tcpConnection.Receive();

         return result.StartsWith("A05 OK");
      }

      public string GetACL(string sFolder)
      {
         string command = string.Format("A06 GETACL \"{0}\"\r\n",
                                        sFolder);
         _tcpConnection.Send(command);
         string result = _tcpConnection.Receive();

         return result;
      }

      public string GetMyRights(string sFolder)
      {
         string command = string.Format("A07 MYRIGHTS \"{0}\"\r\n",
                                        sFolder);
         _tcpConnection.Send(command);
         string result = _tcpConnection.Receive();

         return result;
      }


      public string Status(string folderName, string dataItem)
      {
         string command = string.Format("A08 STATUS \"{0}\" ({1})", folderName, dataItem);

         return SendSingleCommand(command);
      }


      public string GetQuota(string folderName)
      {
         string command = string.Format("A09 GETQUOTA \"{0}\"", folderName);

         return SendSingleCommand(command);
      }

      public string ListRights(string sFolder, string identifier)
      {
         string command = string.Format("A10 LISTRIGHTS \"{0}\" \"{1}\"\r\n",
                                        sFolder, identifier);
         _tcpConnection.Send(command);
         string result = _tcpConnection.Receive();

         return result;
      }

      public bool Subscribe(string sFolder)
      {
         _tcpConnection.Send("A11 SUBSCRIBE " + sFolder + "\r\n");
         string result = _tcpConnection.Receive().Substring(0, 6);

         if (result.StartsWith("A11 OK"))
            return true;
         else
            return false;
      }

      public bool Unsubscribe(string sFolder)
      {
         _tcpConnection.Send("A12 UNSUBSCRIBE \"" + sFolder + "\"\r\n");
         string result = _tcpConnection.Receive().Substring(0, 6);

         if (result.StartsWith("A12 OK"))
            return true;
         else
            return false;
      }


      public bool CheckFolder(string sFolder)
      {
         _tcpConnection.Send("A13 CHECK " + sFolder + "\r\n");
         string result = _tcpConnection.Receive().Substring(0, 6);

         return result.StartsWith("A13 OK");
      }

      public bool Close()
      {
         _tcpConnection.Send("A14 CLOSE\r\n");
         string result = _tcpConnection.ReadUntil(new List<string>() {"A14 BAD", "A14 OK"});

         if (result.Contains("A14 BAD"))
            return false;
         if (result.Contains("A14 OK"))
            return true;

         Assert.Fail(string.Format("IMAPClientSimulator.Close() - Expected BAD/OK, received: \"{0}\"", result));
         return false;
      }

      public bool SelectFolderWithoutLiteral(string sFolder)
      {
         sFolder = sFolder.Replace("\\", "\\\\");
         sFolder = sFolder.Replace("\"", "\\\"");
         string sData = SendSingleCommand("A15 SELECT " + sFolder);
         return sData.StartsWith("*");
      }

      public bool SelectFolder(string folderName, out string text)
      {
         string sData = Send("A16 SELECT {" + folderName.Length + "}");

         if (sData.IndexOf("+ Ready") != 0)
         {
            string message = string.Format("Literal request not received from server. Time: {0}, Response: {1}",
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
         string result = string.Empty;
         SelectFolder(sFolder, out result);
         return result.StartsWith("*");
      }

      public string Fetch(string sParameters)
      {
         string sData = SendSingleCommand("A17 FETCH " + sParameters);

         return sData;
      }

      public string GetFlags(int messageIndex)
      {
         return Fetch(messageIndex + " FLAGS");
      }

      public bool Copy(int messageIndex, string destinationFolder)
      {
         string sData = SendSingleCommand("A18 COPY 1 \"" + destinationFolder + "\"");
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
         string sData = SendSingleCommand("A20 EXAMINE " + sFolder);
         return sData;
      }


      public bool DeleteFolder(string sFolder)
      {
         _tcpConnection.Send("A21 DELETE " + sFolder + "\r\n");
         string sData = _tcpConnection.Receive();

         if (sData.StartsWith("A21 OK"))
            return true;
         else
            return false;
      }

      public bool SetFlagOnMessage(int index, bool bSet, string sFlag)
      {
         string sSetUnset = bSet ? "+" : "-";
         string sData = "A22 STORE " + index.ToString() + " " + sSetUnset + "FLAGS (" + sFlag + ")";
         string result = SendSingleCommand(sData);

         if (result.Contains("A22 OK"))
            return true;
         else
            return false;
      }

      public void SetFlagOnFirstMessage(bool bSet, string sFlag)
      {
         SetFlagOnMessage(1, bSet, sFlag);
      }

      public string Sort(string sSearchString)
      {
         string sData = SendSingleCommand("A23 SORT " + sSearchString);

         int iStart = sData.IndexOf(" ", 4) + 1;
         int iLineEnd = sData.IndexOf("\r\n");

         if (iStart > iLineEnd)
            return "";

         int iLength = iLineEnd - iStart;
         string sMatch = sData.Substring(iStart, iLength);

         return sMatch;
      }

      public string Search(string sSearchString)
      {
         string sData = SendSingleCommand("A24 SEARCH " + sSearchString);

         int iStart = sData.IndexOf(" ", 4) + 1;
         int iLineEnd = sData.IndexOf("\r\n");

         if (iStart > iLineEnd)
            return "";

         int iLength = iLineEnd - iStart;
         string sMatch = sData.Substring(iStart, iLength);

         return sMatch;
      }

      public bool StartIdle()
      {
         _tcpConnection.Send("A25 IDLE\r\n");
         string sData = _tcpConnection.Receive();
         return sData.StartsWith("+ idling");
      }

      public bool EndIdle(bool force, out string output)
      {
         output = string.Empty;

         if (force == false)
         {
            output = _tcpConnection.Receive();
         }

         _tcpConnection.Send("DONE\r\n");

         for (int i = 0; i < 10; i++)
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
      /// Waits for data a total of 8 seconds.
      /// </summary>
      /// <returns>true if data exists</returns>
      public bool AssertPendingDataExists()
      {
         for (int i = 0; i < 40; i++)
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
         string result = SendSingleCommand("A26 LIST \"\" \"" + wildcard + "\"");

         if (unescapeResponse)
         {
            result = result.Replace("\\\\", "\\");
            result = result.Replace("\\\"", "\"");
         }

         return result;
      }

      public string List(string reference, string wildcard, bool unescapeResponse)
      {
         string result = SendSingleCommand("A27 LIST \"" + reference + "\" \"" + wildcard + "\"");

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
         string result = SendSingleCommand("A28 LSUB \"" + reference + "\" \"" + wildcard + "\"");

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
         string result = SendSingleCommand("A29 STORE " + messageIndex + " +FLAGS (\\Deleted)");
         return result.StartsWith("*");
      }

      public bool SetSeenFlag(int messageIndex)
      {
         string result = SendSingleCommand("A30 STORE " + messageIndex + " +FLAGS (\\Seen)");
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
         string sData = _tcpConnection.Receive();
         return sData;
      }

      public int GetMessageCount(string sFolder)
      {
         string sData = SendSingleCommand("A33 SELECT " + sFolder);

         if (!sData.Contains("A33 OK"))
         {
            Assert.Fail("The folder " + sFolder + " was not selectable. Result: " + sData);
            return 0;
         }

         int iStartPos = 2;
         int iEndPos = sData.IndexOf(" ", iStartPos);
         int iLength = iEndPos - iStartPos;

         if (iLength == 0)
         {
            Assert.Fail("Unparseable SELECT response");
         }

         string sValue = sData.Substring(iStartPos, iLength);

         return Convert.ToInt32(sValue);
      }

      public string Send(string s)
      {
         // Capability
         _tcpConnection.Send(s + "\r\n");
         string sData = _tcpConnection.Receive();
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
         string commandName = command.Substring(0, command.IndexOf(" "));

         string result = Send(command);

         bool expectingLiteral = result.StartsWith("+ Ready");

         DateTime startTime = DateTime.Now;

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
         for (int i = 1; i <= 1000; i++)
         {
            if (SelectFolder(folderName))
               return;

            Thread.Sleep(25);
         }

         Assert.Fail("Folder not found: " + folderName);
      }


      public static void AssertMessageCount(string accountName, string accountPassword, string folderName,
                                            int expectedCount)
      {
         if (expectedCount == 0)
         {
            // make sure that we aren't currently delivering messages.
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         }

         var oIMAP = new ImapClientSimulator();
         Assert.IsTrue(oIMAP.ConnectAndLogon(accountName, accountPassword));

         if (expectedCount != 0)
            oIMAP.AssertFolderExists(folderName);

         int currentCount = 0;
         int timeout = 1000; // 1000 * 25 = 25 seconds.
         while (timeout > 0)
         {
            currentCount = oIMAP.GetMessageCount(folderName);

            if (currentCount > expectedCount)
               break;

            if (currentCount == expectedCount)
            {
               oIMAP.Disconnect();
               return;
            }

            timeout--;
            Thread.Sleep(25);
         }

         oIMAP.Disconnect();

         string error = "Wrong number of messages in mailbox " + folderName + " in account " + accountName +
                        " Actual: " + currentCount.ToString() + " Expected: " + expectedCount.ToString();
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