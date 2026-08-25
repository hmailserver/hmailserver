using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using GenerateComApiDocumentation.Idl;

namespace GenerateComApiDocumentation
{
   internal class Program
   {
      private static int Main(string[] args)
      {
         string idlPath = null;
         string outputPath = null;
         bool dryRun = false;
         bool deleteOrphans = false;

         var positional = new List<string>();

         for (int i = 0; i < args.Length; i++)
         {
            switch (args[i].ToLowerInvariant())
            {
               case "--idl":
               case "-i":
                  if (++i >= args.Length)
                     return Fail("Missing value for " + args[i - 1] + ".");
                  idlPath = args[i];
                  break;
               case "--output":
               case "-o":
                  if (++i >= args.Length)
                     return Fail("Missing value for " + args[i - 1] + ".");
                  outputPath = args[i];
                  break;
               case "--dry-run":
                  dryRun = true;
                  break;
               case "--delete-orphans":
                  deleteOrphans = true;
                  break;
               case "--help":
               case "-h":
               case "/?":
                  PrintUsage();
                  return 0;
               default:
                  positional.Add(args[i]);
                  break;
            }
         }

         if (idlPath == null && positional.Count > 0)
            idlPath = positional[0];
         if (outputPath == null && positional.Count > 1)
            outputPath = positional[1];

         if (idlPath == null || outputPath == null)
         {
            PrintUsage();
            return 1;
         }

         if (!File.Exists(idlPath))
            return Fail("IDL file not found: " + idlPath);

         if (!Directory.Exists(outputPath))
            return Fail("Output directory not found: " + outputPath);

         try
         {
            Generate(idlPath, outputPath, dryRun, deleteOrphans);
         }
         catch (Exception ex)
         {
            return Fail(ex.Message);
         }

         return 0;
      }

      private static void Generate(string idlPath, string outputPath, bool dryRun, bool deleteOrphans)
      {
         var document = IdlParser.ParseFile(idlPath);

         Console.WriteLine("Parsed {0}: {1} interfaces, {2} enums.",
                           Path.GetFileName(idlPath), document.Interfaces.Count, document.Enums.Count);

         var generator = new MarkdownGenerator(document);

         var pages = document.Interfaces
            .Where(i => !i.Attributes.IsHidden)
            .Select(generator.GenerateInterfacePage)
            .ToList();

         if (document.Enums.Count > 0)
            pages.Add(generator.GenerateConstantsPage(document.Enums));

         var indexPage = GenerateIndexPage(document, outputPath, document.Enums.Count > 0);
         if (indexPage != null)
            pages.Add(indexPage);

         int written = 0;
         int unchanged = 0;

         foreach (var page in pages.OrderBy(p => p.FileName, StringComparer.OrdinalIgnoreCase))
         {
            string fullPath = Path.Combine(outputPath, page.FileName);
            bool exists = File.Exists(fullPath);

            if (exists && ReadNormalized(fullPath) == page.Content)
            {
               unchanged++;
               continue;
            }

            Console.WriteLine("{0} {1}", exists ? "update" : "create", page.FileName);

            if (!dryRun)
               File.WriteAllText(fullPath, page.Content, new UTF8Encoding(false));

            written++;
         }

         int deleted = HandleOrphans(outputPath, pages, deleteOrphans, dryRun);

         Console.WriteLine();
         Console.WriteLine("{0} file(s) {1}, {2} unchanged, {3} {4}.",
                           written, dryRun ? "would be written" : "written", unchanged,
                           deleted, dryRun ? "would be deleted" : "deleted");
      }

      /// <summary>
      /// Rebuilds the object tree in _index.md. Returns null if the directory has no _index.md.
      /// </summary>
      private static GeneratedPage GenerateIndexPage(IdlDocument document, string outputPath, bool hasConstantsPage)
      {
         string fullPath = Path.Combine(outputPath, "_index.md");
         if (!File.Exists(fullPath))
         {
            Console.WriteLine("No _index.md in the output directory - the object tree was not updated.");
            return null;
         }

         var indexGenerator = new IndexGenerator(document);
         string content = indexGenerator.UpdateIndex(File.ReadAllText(fullPath), hasConstantsPage);

         return new GeneratedPage
         {
            FileName = "_index.md",
            Content = content.Replace("\r\n", "\n").TrimEnd('\n') + "\n"
         };
      }

      /// <summary>
      /// Handles com_object_ pages in the output directory that the IDL no longer describes.
      /// They are always reported, and deleted only when asked for. Returns the number deleted.
      /// </summary>
      private static int HandleOrphans(string outputPath, IEnumerable<GeneratedPage> pages,
                                       bool deleteOrphans, bool dryRun)
      {
         var generated = new HashSet<string>(pages.Select(p => p.FileName), StringComparer.OrdinalIgnoreCase);

         // Only generated pages are considered. The hand written pages in the same directory
         // (com_examples.md, com_changelog.md, com_example_*.md, ...) are never touched.
         var orphans = Directory.GetFiles(outputPath, "com_object_*.md")
            .Select(Path.GetFileName)
            .Where(name => !generated.Contains(name))
            .OrderBy(name => name, StringComparer.OrdinalIgnoreCase)
            .ToList();

         if (orphans.Count == 0)
            return 0;

         Console.WriteLine();

         if (!deleteOrphans)
         {
            Console.WriteLine("Not described by the IDL (use --delete-orphans to remove):");
            foreach (var orphan in orphans)
               Console.WriteLine("  " + orphan);

            return 0;
         }

         int deleted = 0;

         foreach (var orphan in orphans)
         {
            if (!IsDeletable(orphan))
            {
               Console.WriteLine("skip   {0} (only com_object pages may be deleted)", orphan);
               continue;
            }

            Console.WriteLine("delete {0}", orphan);

            if (!dryRun)
               File.Delete(Path.Combine(outputPath, orphan));

            deleted++;
         }

         return deleted;
      }

      /// <summary>
      /// Nothing outside the generated com_object pages may ever be removed, whatever the
      /// caller asks for. The hand written pages in the same directory are also named com_*,
      /// so the full prefix is what keeps them safe.
      /// </summary>
      private static bool IsDeletable(string fileName)
      {
         return fileName.StartsWith("com_object", StringComparison.OrdinalIgnoreCase) &&
                fileName.EndsWith(".md", StringComparison.OrdinalIgnoreCase) &&
                fileName.IndexOf(Path.DirectorySeparatorChar) < 0 &&
                fileName.IndexOf(Path.AltDirectorySeparatorChar) < 0;
      }

      private static string ReadNormalized(string path)
      {
         return File.ReadAllText(path).Replace("\r\n", "\n").TrimEnd('\n') + "\n";
      }

      private static int Fail(string message)
      {
         Console.Error.WriteLine("Error: " + message);
         return 1;
      }

      private static void PrintUsage()
      {
         Console.WriteLine("Generates the COM API documentation pages from hMailServer.idl.");
         Console.WriteLine();
         Console.WriteLine("Usage:");
         Console.WriteLine("  GenerateComApiDocumentation <idl-file> <output-directory> [options]");
         Console.WriteLine();
         Console.WriteLine("Options:");
         Console.WriteLine("  -i, --idl <path>       Path to hMailServer.idl.");
         Console.WriteLine("  -o, --output <path>    Directory holding the com_objects Markdown pages.");
         Console.WriteLine("      --dry-run          Show what would change without writing any file.");
         Console.WriteLine("      --delete-orphans   Delete com_object pages the IDL no longer describes.");
         Console.WriteLine();
         Console.WriteLine("Example:");
         Console.WriteLine("  GenerateComApiDocumentation ..\\Server\\hMailServer\\hMailServer.idl " +
                           "..\\..\\..\\www.hmailserver.com\\docs-content\\latest\\book_other\\com_objects");
      }
   }
}
