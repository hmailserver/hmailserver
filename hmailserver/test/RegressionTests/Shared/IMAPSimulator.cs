// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Threading;
using NUnit.Framework;

namespace RegressionTests.Shared
{
   /// <summary>
   /// Summary description for IMAPSimulator.
   /// </summary>
   public class IMAPSimulator
   {
      private readonly int _port = 143;
      private readonly TcpSocket _socket;

      public IMAPSimulator()
      {
         _socket = new TcpSocket();
      }

      public IMAPSimulator(string username, string password, string mailbox)
      {
         _socket = new TcpSocket();

         Assert.IsTrue(ConnectAndLogon(username, password));
         Assert.IsTrue(SelectFolder(mailbox));
      }

      public IMAPSimulator(bool useSSL, int port)
      {
         _socket = new TcpSocket(useSSL);
         _port = port;
      }


      public bool TestConnect(int iPort)
      {
         bool bRetVal = _socket.Connect(iPort);
         _socket.Disconnect();
         return bRetVal;
      }

      public string Connect()
      {
         _socket.Connect(_port);
         string sData = _socket.Receive();

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
         _socket.Disconnect();
      }

      public bool Logout()
      {
         _socket.Send("A01 LOGOUT\r\n");
         string sData = _socket.Receive();

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
         _socket.Send("A01 LOGIN " + sUsername + " {" + sPassword.Length.ToString() + "}\r\n");
         string sData = _socket.Receive();

         if (sData.IndexOf("+ Ready") != 0)
            throw new Exception("Literal ready not received");

         _socket.Send(sPassword + "\r\n");

         sData = _socket.Receive();
         if (sData.StartsWith("A01 NO") || sData.StartsWith("+ Ready"))
            throw new Exception("Logon failed");

         // Logon using two literals.

         _socket.Send("A01 LOGIN {" + sUsername.Length.ToString() + "}\r\n");
         sData = _socket.Receive();

         if (sData.IndexOf("+ Ready") != 0)
            throw new Exception("Literal ready not received");

         _socket.Send(sUsername + " {" + sPassword.Length.ToString() + "}\r\n");
         sData = _socket.Receive();

         if (sData.IndexOf("+ Ready") != 0)
            throw new Exception("Literal ready not received");

         _socket.Send(sPassword + "\r\n");

         sData = _socket.Receive();
         if (sData.StartsWith("A01 NO") || sData.StartsWith("+ Ready"))
            throw new Exception("Logon failed");
      }

      public bool CreateFolder(string sFolder)
      {
         sFolder = sFolder.Replace("\\", "\\\\");
         sFolder = sFolder.Replace("\"", "\\\"");

         string result = SendSingleCommand("A01 CREATE \"" + sFolder + "\"");

         if (result.StartsWith("A01 OK"))
            return true;
         else
            return false;
      }

      public bool SetACL(string sFolder, string identifier, string access)
      {
         string command = string.Format("A01 SETACL {0} {1} {2}\r\n",
                                        sFolder,
                                        identifier,
                                        access
            );
         _socket.Send(command);
         string result = _socket.Receive();

         return result.StartsWith("A01 OK");
      }

      public bool DeleteACL(string sFolder, string identifier)
      {
         string command = string.Format("A01 DELETEACL \"{0}\" {1}\r\n",
                                        sFolder,
                                        identifier);
         _socket.Send(command);
         string result = _socket.Receive();

         return result.StartsWith("A01 OK");
      }

      public string GetACL(string sFolder)
      {
         string command = string.Format("A01 GETACL \"{0}\"\r\n",
                                        sFolder);
         _socket.Send(command);
         string result = _socket.Receive();

         return result;
      }

      public string GetMyRights(string sFolder)
      {
         string command = string.Format("A01 MYRIGHTS \"{0}\"\r\n",
                                        sFolder);
         _socket.Send(command);
         string result = _socket.Receive();

         return result;
      }


      public string Status(string folderName, string dataItem)
      {
         string command = string.Format("A01 STATUS \"{0}\" ({1})", folderName, dataItem);

         return SendSingleCommand(command);
      }


      public string GetQuota(string folderName)
      {
         string command = string.Format("A01 GETQUOTA \"{0}\"", folderName);

         return SendSingleCommand(command);
      }

      public string ListRights(string sFolder, string identifier)
      {
         string command = string.Format("A01 LISTRIGHTS \"{0}\" \"{1}\"\r\n",
                                        sFolder, identifier);
         _socket.Send(command);
         string result = _socket.Receive();

         return result;
      }

      public bool Subscribe(string sFolder)
      {
         _socket.Send("A01 SUBSCRIBE " + sFolder + "\r\n");
         string result = _socket.Receive().Substring(0, 6);

         if (result.StartsWith("A01 OK"))
            return true;
         else
            return false;
      }

      public bool Unsubscribe(string sFolder)
      {
         _socket.Send("A01 UNSUBSCRIBE \"" + sFolder + "\"\r\n");
         string result = _socket.Receive().Substring(0, 6);

         if (result.StartsWith("A01 OK"))
            return true;
         else
            return false;
      }


      public bool CheckFolder(string sFolder)
      {
         _socket.Send("A01 CHECK " + sFolder + "\r\n");
         string result = _socket.Receive().Substring(0, 6);

         return result.StartsWith("A01 OK");
      }

      public bool Close()
      {
         _socket.Send("A01 CLOSE\r\n");
         string result = _socket.Receive();

         if (result.StartsWith("A01 BAD"))
            return false;
         else if (result.StartsWith("A01 OK"))
            return true;

         Assert.Fail("IMAPSimulator.Close() - Expected BAD/OK, received: \"{0}\"", result);
         return false;
      }

      public bool SelectFolderWithoutLiteral(string sFolder)
      {
         sFolder = sFolder.Replace("\\", "\\\\");
         sFolder = sFolder.Replace("\"", "\\\"");
         string sData = SendSingleCommand("A01 SELECT " + sFolder);
         return sData.StartsWith("*");
      }

      public bool SelectFolder(string folderName, out string text)
      {
         string sData = Send("A01 SELECT {" + folderName.Length + "}");

         if (sData.IndexOf("+ Ready") != 0)
         {
            string message = string.Format("Literal request not received from server. Time: {0}, Response: {1}",
                                           DateTime.Now.ToShortDateString(),
                                           sData);

            throw new Exception(message);
         }

         _socket.Send(folderName + "\r\n");

         text = _socket.Receive();
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
         string sData = SendSingleCommand("A01 FETCH " + sParameters);

         return sData;
      }

      public string GetFlags(int messageIndex)
      {
         return Fetch(messageIndex + " FLAGS");
      }

      public bool Copy(int messageIndex, string destinationFolder)
      {
         string sData = SendSingleCommand("A01 COPY 1 \"" + destinationFolder + "\"");
         return sData.StartsWith("A01 OK");
      }

      public bool RenameFolder(string from, string to)
      {
         string result;
         return RenameFolder(from, to, out result);
      }

      public bool RenameFolder(string from, string to, out string result)
      {
         result = SendSingleCommand(string.Format("A01 RENAME \"{0}\" \"{1}\"", from, to));
         return result.StartsWith("A01 OK");
      }

      public string ExamineFolder(string sFolder)
      {
         string sData = SendSingleCommand("A01 EXAMINE " + sFolder);
         return sData;
      }


      public bool DeleteFolder(string sFolder)
      {
         _socket.Send("A01 DELETE " + sFolder + "\r\n");
         string sData = _socket.Receive();

         if (sData.StartsWith("A01 OK"))
            return true;
         else
            return false;
      }

      public bool SetFlagOnMessage(int index, bool bSet, string sFlag)
      {
         string sSetUnset = bSet ? "+" : "-";
         string sData = "A01 STORE " + index.ToString() + " " + sSetUnset + "FLAGS (" + sFlag + ")";
         string result = SendSingleCommand(sData);

         if (result.Contains("A01 OK"))
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
         string sData = SendSingleCommand("A01 SORT " + sSearchString);

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
         string sData = SendSingleCommand("A01 SEARCH " + sSearchString);

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
         _socket.Send("A01 IDLE\r\n");
         string sData = _socket.Receive();
         return sData.StartsWith("+ idling");
      }

      public bool EndIdle(bool force, out string output)
      {
         output = string.Empty;

         if (force == false)
         {
            output = _socket.Receive();
         }

         _socket.Send("DONE\r\n");

         for (int i = 0; i < 10; i++)
         {
            output += _socket.Receive();

            if (output.Contains("OK IDLE terminated"))
               return true;
         }

         return false;
      }

      public bool GetPendingDataExists()
      {
         return _socket.Peek();
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
         string result = SendSingleCommand("A01 LIST \"\" \"" + wildcard + "\"");

         if (unescapeResponse)
         {
            result = result.Replace("\\\\", "\\");
            result = result.Replace("\\\"", "\"");
         }

         return result;
      }

      public string List(string reference, string wildcard, bool unescapeResponse)
      {
         string result = SendSingleCommand("A01 LIST \"" + reference + "\" \"" + wildcard + "\"");

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
         string result = SendSingleCommand("A01 LSUB \"" + reference + "\" \"" + wildcard + "\"");

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
         string result = SendSingleCommand("A01 STORE " + messageIndex + " +FLAGS (\\Deleted)");
         return result.StartsWith("*");
      }

      public bool SetSeenFlag(int messageIndex)
      {
         string result = SendSingleCommand("A01 STORE " + messageIndex + " +FLAGS (\\Seen)");
         return result.StartsWith("*");
      }

      public bool Expunge()
      {
         string result;
         return Expunge(out result);
      }

      public bool Expunge(out string result)
      {
         result = SendSingleCommand("A01 EXPUNGE");
         return result.StartsWith("*");
      }

      public string GetCapabilities()
      {
         // Capability
         _socket.Send("A01 CAPABILITY\r\n");
         string sData = _socket.Receive();
         return sData;
      }

      public int GetMessageCount(string sFolder)
      {
         string sData = SendSingleCommand("A01 SELECT " + sFolder);

         if (!sData.Contains("A01 OK"))
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
         _socket.Send(s + "\r\n");
         string sData = _socket.Receive();
         return sData;
      }

      public void SendRaw(string s)
      {
         _socket.Send(s);
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

            if (!_socket.Connected)
               return result;

            if (DateTime.Now - startTime > new TimeSpan(0, 0, 30))
               throw new Exception("Timeout while waiting for data.");
         }

         return result;
      }


      public string Receive()
      {
         return _socket.Receive();
      }

      public string ReceiveUntil(string characters)
      {
         return _socket.ReadUntil(characters);
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
            TestSetup.AssertRecipientsInDeliveryQueue(0);
         }

         var oIMAP = new IMAPSimulator();
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
         return SendSingleCommand("A01 NOOP");
      }
   }
}