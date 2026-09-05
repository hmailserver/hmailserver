using System;
using System.Security.Cryptography;
using System.Text;

namespace RegressionTests.SMTP.SRS
{
   /// <summary>
   /// The Sender Rewriting Scheme address format, implemented here a second time so that
   /// the tests can check what hMailServer produces against something other than itself,
   /// and can hand it addresses it has never seen: expired ones, forged ones, and ones
   /// another forwarding server would have created.
   ///
   ///    SRS0=hash=timestamp=domain=local part@forwarding domain
   ///    SRS1=hash=first hop=the SRS0 part of the address@forwarding domain
   ///
   /// The hash covers the fields which follow it, lower-cased, and is the first few
   /// characters of the base64 of an HMAC-SHA1 taken with the server's SRS secret.
   /// </summary>
   internal class SrsAddress
   {
      public const string TimestampCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
      public const int DefaultHashLength = 8;
      public const int MinHashLength = 8;
      public const int MaxHashLength = 20;
      public const int DefaultMaxAgeDays = 21;
      public const int MinMaxAgeDays = 1;
      public const int MaxMaxAgeDays = 512;

      public string Tag { get; private set; }
      public string Hash { get; private set; }
      public string Timestamp { get; private set; }
      public string Domain { get; private set; }
      public string LocalPart { get; private set; }
      public string Srs0Payload { get; private set; }
      public string ForwardingDomain { get; private set; }

      public bool IsSrs0 => Tag == "SRS0";

      /// <summary>
      /// The address an SRS0 address was created for.
      /// </summary>
      public string OriginalAddress => LocalPart + "@" + Domain;

      public static string CreateTimestamp(DateTime utcTime)
      {
         var days = (long) (utcTime - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalDays;

         return new string(new[]
            {
               TimestampCharacters[(int) ((days >> 5) & 31)],
               TimestampCharacters[(int) (days & 31)]
            });
      }

      public static string CreateHash(string secret, string data, int hashLength)
      {
         using (var hmac = new HMACSHA1(Encoding.UTF8.GetBytes(secret)))
         {
            var hash = hmac.ComputeHash(Encoding.UTF8.GetBytes(data.ToLowerInvariant()));

            return Convert.ToBase64String(hash).TrimEnd('=').Substring(0, hashLength);
         }
      }

      /// <summary>
      /// The address hMailServer should create when it forwards a message from
      /// originalSender out of forwardingDomain.
      /// </summary>
      public static string Create(string secret, string originalSender, string forwardingDomain)
      {
         return Create(secret, originalSender, forwardingDomain, DateTime.UtcNow, DefaultHashLength);
      }

      public static string Create(string secret, string originalSender, string forwardingDomain, DateTime utcTime,
         int hashLength)
      {
         var separatorPosition = originalSender.LastIndexOf('@');
         var localPart = originalSender.Substring(0, separatorPosition);
         var domain = originalSender.Substring(separatorPosition + 1);

         var timestamp = CreateTimestamp(utcTime);
         var hash = CreateHash(secret, timestamp + "=" + domain + "=" + localPart, hashLength);

         return "SRS0=" + hash + "=" + timestamp + "=" + domain + "=" + localPart + "@" + forwardingDomain;
      }

      /// <summary>
      /// The address a second forwarding server should create when it forwards a message
      /// whose sender is already the given SRS0 address.
      /// </summary>
      public static string CreateChained(string secret, string srs0Address, string forwardingDomain, int hashLength)
      {
         var separatorPosition = srs0Address.LastIndexOf('@');
         var firstHop = srs0Address.Substring(separatorPosition + 1);
         var payload = srs0Address.Substring(4, separatorPosition - 4);

         var hash = CreateHash(secret, firstHop + payload, hashLength);

         return "SRS1=" + hash + "=" + firstHop + "=" + payload + "@" + forwardingDomain;
      }

      public static SrsAddress Parse(string address)
      {
         var separatorPosition = address.LastIndexOf('@');

         if (separatorPosition <= 0)
            throw new FormatException("Not an email address: " + address);

         var localPart = address.Substring(0, separatorPosition);

         var result = new SrsAddress
            {
               ForwardingDomain = address.Substring(separatorPosition + 1)
            };

         if (localPart.Length < 6 || (localPart[4] != '=' && localPart[4] != '+' && localPart[4] != '-'))
            throw new FormatException("Not an SRS address: " + address);

         result.Tag = localPart.Substring(0, 4);

         var fields = localPart.Substring(5).Split(new[] {'='}, result.Tag == "SRS0" ? 4 : 3);

         if (result.Tag == "SRS0")
         {
            if (fields.Length != 4)
               throw new FormatException("Not an SRS0 address: " + address);

            result.Hash = fields[0];
            result.Timestamp = fields[1];
            result.Domain = fields[2];
            result.LocalPart = fields[3];
         }
         else if (result.Tag == "SRS1")
         {
            if (fields.Length != 3)
               throw new FormatException("Not an SRS1 address: " + address);

            result.Hash = fields[0];
            result.Domain = fields[1];

            // The embedded SRS0 part opens with a separator of its own, which is where
            // the double separator in an SRS1 address comes from.
            result.Srs0Payload = fields[2];

            if (!result.Srs0Payload.StartsWith("="))
               throw new FormatException("Not an SRS1 address: " + address);
         }
         else
         {
            throw new FormatException("Not an SRS address: " + address);
         }

         return result;
      }

      /// <summary>
      /// Whether the hash in the address is the one the secret produces for it. This is
      /// what stops anyone without the secret from making up an address which relays
      /// mail through the server.
      /// </summary>
      public bool HasValidHash(string secret)
      {
         var data = IsSrs0
            ? Timestamp + "=" + Domain + "=" + LocalPart
            : Domain + Srs0Payload;

         var expected = CreateHash(secret, data, Hash.Length);

         return string.Equals(expected, Hash, StringComparison.OrdinalIgnoreCase);
      }

      /// <summary>
      /// How many days ago the address was created, as its timestamp counts them.
      /// </summary>
      public int GetAgeInDays(DateTime utcTime)
      {
         var then = TimestampCharacters.IndexOf(char.ToUpperInvariant(Timestamp[0])) * 32 +
                    TimestampCharacters.IndexOf(char.ToUpperInvariant(Timestamp[1]));

         var today = (int) ((long) (utcTime - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalDays % 1024);

         var age = (today - then) % 1024;

         return age < 0 ? age + 1024 : age;
      }
   }
}
