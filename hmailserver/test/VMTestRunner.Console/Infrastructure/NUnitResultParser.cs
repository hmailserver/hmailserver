using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace VMTestRunner.Console
{
   /// <summary>
   /// Parses an NUnit3 TestResult.xml document and produces a human readable
   /// summary of the failing tests, so that failures can be spotted in the log
   /// without having to read through the raw NUnit output.
   /// </summary>
   public static class NUnitResultParser
   {
      /// <summary>
      /// Returns a formatted summary listing every failing test together with its
      /// failure message and stack trace. Returns <c>null</c> when no failures are
      /// found.
      /// </summary>
      public static string SummarizeFailures(XmlDocument doc)
      {
         if (doc == null)
            return null;

         var failures = new List<string>();

         // Failing test-cases are the actual failing tests.
         var testCases = doc.SelectNodes("//test-case[@result='Failed']");
         if (testCases != null)
         {
            foreach (XmlNode testCase in testCases)
               failures.Add(FormatFailure(testCase));
         }

         // Failing fixtures/suites with a direct failure element capture setup or
         // teardown errors that are not attached to any single test-case. Suites
         // that merely propagate a child failure are skipped to avoid noise.
         var suites = doc.SelectNodes("//test-suite[@result='Failed']");
         if (suites != null)
         {
            foreach (XmlNode suite in suites)
            {
               var failure = GetDirectChild(suite, "failure");
               if (failure == null)
                  continue;

               var message = GetChildText(failure, "message");
               if (message != null && message.IndexOf("child test", StringComparison.OrdinalIgnoreCase) >= 0)
                  continue;

               failures.Add(FormatFailure(suite));
            }
         }

         if (failures.Count == 0)
            return null;

         var builder = new StringBuilder();
         builder.AppendLine($"{failures.Count} failing test(s):");
         builder.AppendLine();

         for (int i = 0; i < failures.Count; i++)
         {
            builder.Append($"{i + 1}) ");
            builder.AppendLine(failures[i]);
            builder.AppendLine();
         }

         return builder.ToString().TrimEnd();
      }

      private static string FormatFailure(XmlNode node)
      {
         var name = node.Attributes?["fullname"]?.Value
                    ?? node.Attributes?["name"]?.Value
                    ?? "(unknown test)";

         var builder = new StringBuilder();
         builder.AppendLine(name);

         var failure = GetDirectChild(node, "failure");

         var message = GetChildText(failure, "message");
         if (!string.IsNullOrWhiteSpace(message))
            builder.AppendLine(Indent(message.Trim()));

         var stackTrace = GetChildText(failure, "stack-trace");
         if (!string.IsNullOrWhiteSpace(stackTrace))
            builder.AppendLine(Indent(stackTrace.TrimEnd()));

         return builder.ToString().TrimEnd();
      }

      private static XmlNode GetDirectChild(XmlNode parent, string name)
      {
         if (parent == null)
            return null;

         foreach (XmlNode child in parent.ChildNodes)
         {
            if (child.NodeType == XmlNodeType.Element && child.Name == name)
               return child;
         }

         return null;
      }

      private static string GetChildText(XmlNode parent, string name)
      {
         var child = GetDirectChild(parent, name);
         return child?.InnerText;
      }

      private static string Indent(string text)
      {
         var lines = text.Split(new[] { "\r\n", "\n" }, StringSplitOptions.None);
         return "   " + string.Join(Environment.NewLine + "   ", lines);
      }
   }
}
