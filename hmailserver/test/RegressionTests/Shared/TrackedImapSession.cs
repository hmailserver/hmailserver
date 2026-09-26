// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using NUnit.Framework;

namespace RegressionTests.Shared
{
   /// <summary>
   ///    Wraps an IMAP connection and checks every response against the session's message count.
   ///    An EXPUNGE or FETCH for a number the client was never told about is recorded as a violation.
   /// </summary>
   internal class TrackedImapSession
   {
      private static readonly Regex ExistsLine = new Regex(@"^\* (\d+) EXISTS\r?$", RegexOptions.Multiline);
      private static readonly Regex UntaggedLine = new Regex(@"^\* (\d+) (EXISTS|EXPUNGE|FETCH)\b", RegexOptions.Multiline);

      private readonly ImapClientSimulator _simulator;
      private readonly List<string> _violations = new List<string>();
      private readonly string _name;
      private int _tag;

      public TrackedImapSession(string address, string password, string name = "session")
      {
         _name = name;
         _simulator = new ImapClientSimulator();
         Assert.IsTrue(_simulator.ConnectAndLogon(address, password), "Logon failed for " + name);
      }

      public ImapClientSimulator Simulator => _simulator;

      /// <summary>The number of messages the client currently believes the mailbox holds.</summary>
      public int KnownCount { get; private set; } = -1;

      public IReadOnlyList<string> Violations => _violations;

      public string Select(string folder, bool examine = false)
      {
         var response = Command((examine ? "EXAMINE" : "SELECT") + " \"" + folder + "\"");
         Assert.IsTrue(IsOk(response), response);

         var match = ExistsLine.Match(response);
         Assert.IsTrue(match.Success, response);
         KnownCount = int.Parse(match.Groups[1].Value);

         return response;
      }

      public string Command(string command)
      {
         var tag = "T" + (++_tag).ToString("D5");
         var response = _simulator.SendSingleCommand(tag + " " + command);
         Track(response, command);
         return response;
      }

      public string Append(string folder, string message, string flags = null)
      {
         var tag = "T" + (++_tag).ToString("D5");
         var command = tag + " APPEND \"" + folder + "\" " + (flags != null ? "(" + flags + ") " : "") + "{" + message.Length + "}";
         var response = _simulator.SendSingleCommandWithLiteral(command, message);
         Track(response, "APPEND " + folder);
         return response;
      }

      /// <summary>Returns true if the tagged response of the last command was OK.</summary>
      public static bool IsOk(string response)
      {
         return Regex.IsMatch(response, @"^T\d{5} OK", RegexOptions.Multiline);
      }

      public static bool IsNo(string response)
      {
         return Regex.IsMatch(response, @"^T\d{5} NO", RegexOptions.Multiline);
      }

      public static bool IsBad(string response)
      {
         return Regex.IsMatch(response, @"^T\d{5} BAD", RegexOptions.Multiline);
      }

      public void Disconnect()
      {
         _simulator.Disconnect();
      }

      public void AssertNoViolations()
      {
         Assert.IsEmpty(_violations, _name + ":" + Environment.NewLine + string.Join(Environment.NewLine, _violations));
      }

      // RFC 4315 4: nz-number UIDVALIDITY, uid-sets of nz-numbers without *, only in a tagged OK.
      private static readonly Regex UidPlusCode = new Regex(@"\[(APPENDUID|COPYUID)[^\]]*\]");
      private static readonly Regex ValidUidPlusLine = new Regex(
         @"^\S+ OK \[(APPENDUID [1-9]\d* [1-9]\d*|COPYUID [1-9]\d* (?<set>[1-9]\d*(:[1-9]\d*)?(,[1-9]\d*(:[1-9]\d*)?)*) (?<set>[1-9]\d*(:[1-9]\d*)?(,[1-9]\d*(:[1-9]\d*)?)*))\] ");

      private void Track(string response, string command)
      {
         foreach (var line in response.Split('\n'))
         {
            if (UidPlusCode.IsMatch(line) && !ValidUidPlusLine.IsMatch(line))
               _violations.Add(string.Format("{0}: malformed or misplaced UIDPLUS code: {1}", _name, line.Trim()));
         }

         if (KnownCount < 0)
            return;

         foreach (Match match in UntaggedLine.Matches(response))
         {
            var number = int.Parse(match.Groups[1].Value);

            switch (match.Groups[2].Value)
            {
               case "EXISTS":
                  // EXISTS may not decrease the count; only EXPUNGE does (RFC 3501 7.3.1).
                  if (number < KnownCount)
                     _violations.Add(string.Format("{0}: EXISTS {1} below known count {2}. Command: {3}", _name, number, KnownCount, command));
                  KnownCount = number;
                  break;
               case "EXPUNGE":
                  if (number < 1 || number > KnownCount)
                     _violations.Add(string.Format("{0}: EXPUNGE {1} outside 1..{2}. Command: {3}", _name, number, KnownCount, command));
                  else
                     KnownCount--;
                  break;
               case "FETCH":
                  if (number < 1 || number > KnownCount)
                     _violations.Add(string.Format("{0}: FETCH {1} outside 1..{2}. Command: {3}", _name, number, KnownCount, command));
                  break;
            }
         }
      }
   }

   internal static class UidPlusHelpers
   {
      public const string Password = "test";

      public static string CreateMessage(string messageId, string body = "Body\r\n")
      {
         return "Message-ID: <" + messageId + ">\r\n" +
                "Subject: " + messageId + "\r\n" +
                "\r\n" +
                body;
      }

      public static Match ParseAppendUid(string response)
      {
         return Regex.Match(response, @" OK \[APPENDUID (\d+) (\d+)\]");
      }

      public static long AssertAppendUid(string response, long expectedUidValidity = -1)
      {
         var match = ParseAppendUid(response);
         Assert.IsTrue(match.Success, "No APPENDUID in response. " + response);

         if (expectedUidValidity >= 0)
            Assert.AreEqual(expectedUidValidity, long.Parse(match.Groups[1].Value), response);

         return long.Parse(match.Groups[2].Value);
      }

      public class CopyUid
      {
         public long UidValidity;
         public List<long> Source;
         public List<long> Destination;
      }

      public static CopyUid ParseCopyUid(string response)
      {
         var match = Regex.Match(response, @" OK \[COPYUID (\d+) ([\d:,]+) ([\d:,]+)\]");
         if (!match.Success)
            return null;

         return new CopyUid
         {
            UidValidity = long.Parse(match.Groups[1].Value),
            Source = ExpandUidSet(match.Groups[2].Value),
            Destination = ExpandUidSet(match.Groups[3].Value)
         };
      }

      /// <summary>Expands a uid-set keeping order. "3:1" is expanded as written, 3 2 1.</summary>
      public static List<long> ExpandUidSet(string set)
      {
         var result = new List<long>();

         foreach (var part in set.Split(','))
         {
            var bounds = part.Split(':');
            var first = long.Parse(bounds[0]);
            var last = long.Parse(bounds[bounds.Length - 1]);
            var step = first <= last ? 1 : -1;

            for (var uid = first; ; uid += step)
            {
               result.Add(uid);
               if (uid == last)
                  break;
            }
         }

         return result;
      }

      public static long GetResponseCodeValue(string response, string name)
      {
         var match = Regex.Match(response, @"\[" + name + @" (\d+)\]");
         Assert.IsTrue(match.Success, name + " not found. " + response);
         return long.Parse(match.Groups[1].Value);
      }

      public static long GetStatusValue(string response, string name)
      {
         var match = Regex.Match(response, name + @" (\d+)");
         Assert.IsTrue(match.Success, name + " not found. " + response);
         return long.Parse(match.Groups[1].Value);
      }

      public static int[] GetExpungedSequences(string response)
      {
         return Regex.Matches(response, @"^\* (\d+) EXPUNGE", RegexOptions.Multiline)
            .Cast<Match>()
            .Select(match => int.Parse(match.Groups[1].Value))
            .ToArray();
      }

      public static List<long> UidSearchAll(TrackedImapSession session)
      {
         var response = session.Command("UID SEARCH ALL");
         Assert.IsTrue(TrackedImapSession.IsOk(response), response);

         var match = Regex.Match(response, @"^\* SEARCH ?(.*)\r$", RegexOptions.Multiline);
         Assert.IsTrue(match.Success, response);

         return match.Groups[1].Value.Split(new[] {' '}, StringSplitOptions.RemoveEmptyEntries).Select(long.Parse).ToList();
      }

      /// <summary>UID to Message-ID, for the given UID set in the selected folder.</summary>
      public static Dictionary<long, string> FetchMessageIds(TrackedImapSession session, string uidSet)
      {
         var response = session.Command("UID FETCH " + uidSet + " (UID BODY.PEEK[HEADER.FIELDS (MESSAGE-ID)])");
         Assert.IsTrue(TrackedImapSession.IsOk(response), response);

         var result = new Dictionary<long, string>();

         // One chunk per FETCH response. The order of UID and BODY within it is not fixed.
         var chunks = Regex.Split(response, @"(?=^\* \d+ FETCH)", RegexOptions.Multiline);

         foreach (var chunk in chunks)
         {
            var uid = Regex.Match(chunk, @"UID (\d+)");
            var messageId = Regex.Match(chunk, @"Message-ID: <([^>]+)>", RegexOptions.IgnoreCase);

            if (uid.Success && messageId.Success)
               result[long.Parse(uid.Groups[1].Value)] = messageId.Groups[1].Value;
         }

         return result;
      }
   }
}
