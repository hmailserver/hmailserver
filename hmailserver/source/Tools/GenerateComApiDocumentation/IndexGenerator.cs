using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GenerateComApiDocumentation.Idl;

namespace GenerateComApiDocumentation
{
   /// <summary>
   /// Rebuilds the "# Reference" object list in _index.md from the IDL. The list contains every
   /// documented object in alphabetical order. Everything else in _index.md is left alone.
   /// </summary>
   internal class IndexGenerator
   {
      private const string ReferenceHeading = "# Reference";
      private const string SlugPrefix = "com_object_";

      private readonly List<IdlInterface> _interfaces;

      public IndexGenerator(IdlDocument document)
      {
         _interfaces = document.Interfaces
            .Where(i => !i.Attributes.IsHidden)
            .GroupBy(i => i.InterfaceName, StringComparer.Ordinal)
            .Select(g => g.First())
            .OrderBy(i => i.PublicName, StringComparer.OrdinalIgnoreCase)
            .ToList();
      }

      /// <summary>
      /// Returns <paramref name="existingContent"/> with the reference section replaced.
      /// The section runs from "# Reference" up to the next top level heading.
      /// </summary>
      public string UpdateIndex(string existingContent, bool hasConstantsPage)
      {
         string content = existingContent.Replace("\r\n", "\n");

         int start = FindReferenceHeading(content);
         if (start < 0)
            throw new InvalidOperationException("_index.md does not contain a '" + ReferenceHeading + "' heading.");

         int end = FindNextTopLevelHeading(content, start);

         return content.Substring(0, start) + BuildReferenceSection(hasConstantsPage) + content.Substring(end);
      }

      private string BuildReferenceSection(bool hasConstantsPage)
      {
         var entries = _interfaces
            .Select(i => new { Title = i.PublicName, Slug = SlugPrefix + i.PublicName.ToLowerInvariant() })
            .ToList();

         if (hasConstantsPage)
            entries.Add(new { Title = "Constants", Slug = SlugPrefix + "constants" });

         var text = new StringBuilder();
         text.Append(ReferenceHeading).Append('\n').Append('\n');

         foreach (var entry in entries.OrderBy(e => e.Title, StringComparer.OrdinalIgnoreCase))
         {
            text.Append("- [").Append(entry.Title)
                .Append("](?page=").Append(entry.Slug).Append(')')
                .Append('\n');
         }

         text.Append('\n');
         return text.ToString();
      }

      private static int FindReferenceHeading(string content)
      {
         foreach (var offset in HeadingOffsets(content))
         {
            if (content.Substring(offset).StartsWith(ReferenceHeading, StringComparison.Ordinal))
               return offset;
         }

         return -1;
      }

      private static int FindNextTopLevelHeading(string content, int after)
      {
         foreach (var offset in HeadingOffsets(content))
         {
            if (offset > after)
               return offset;
         }

         return content.Length;
      }

      /// <summary>Offsets of every line starting with "# " (a top level heading).</summary>
      private static IEnumerable<int> HeadingOffsets(string content)
      {
         int offset = 0;

         foreach (var line in content.Split('\n'))
         {
            if (line.StartsWith("# ", StringComparison.Ordinal))
               yield return offset;

            offset += line.Length + 1;
         }
      }
   }
}
