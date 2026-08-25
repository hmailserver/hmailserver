using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GenerateComApiDocumentation.Idl;

namespace GenerateComApiDocumentation
{
   internal class GeneratedPage
   {
      public string FileName { get; set; }
      public string Content { get; set; }
   }

   /// <summary>
   /// Renders the parsed IDL as the Markdown pages used by docs-content/&lt;version&gt;/book_other/com_objects.
   /// </summary>
   internal class MarkdownGenerator
   {
      private const string InterfacePrefix = "IInterface";
      private const string SlugPrefix = "com_object_";
      private const string ParentSlug = "com_objects";

      /// <summary>Maps IDL types to the type names used in the documentation.</summary>
      private static readonly Dictionary<string, string> ScriptTypeNames =
         new Dictionary<string, string>(StringComparer.Ordinal)
         {
            { "BSTR", "string" },
            { "VARIANT_BOOL", "bool" },
            { "VARIANT", "VARIANT" },
            { "long", "long" },
            { "LONG", "long" },
            { "int", "long" },
            { "short", "short" },
            { "hyper", "long" },
            { "float", "float" },
            { "double", "double" },
            { "DATE", "VARIANT" },
            { "IDispatch", "object" },
            { "void", "void" }
         };

      private readonly HashSet<string> _knownInterfaces;

      public MarkdownGenerator(IdlDocument document)
      {
         _knownInterfaces = new HashSet<string>(
            document.Interfaces.Select(i => i.InterfaceName), StringComparer.Ordinal);
      }

      public GeneratedPage GenerateInterfacePage(IdlInterface idlInterface)
      {
         var text = new StringBuilder();

         string publicName = idlInterface.PublicName;

         AppendFrontMatter(text, publicName + " object", Slug(publicName));

         text.AppendLine("### Description");
         text.AppendLine();
         text.AppendLine(Html(idlInterface.Attributes.HelpString ?? string.Empty));
         text.AppendLine();

         var visible = idlInterface.Members.Where(m => !m.Attributes.IsHidden).ToList();

         var methods = visible
            .Where(m => m.Kind == IdlMemberKind.Method)
            .OrderBy(m => m.Name, StringComparer.OrdinalIgnoreCase)
            .ToList();

         var properties = GroupProperties(visible);

         if (methods.Count > 0)
         {
            text.AppendLine("### Methods");
            text.AppendLine();

            foreach (var method in methods)
            {
               AppendEntry(text,
                           Html(method.Name) + "(" + FormatParameters(method.InParameters) + ")",
                           Html(method.Attributes.HelpString ?? string.Empty));
            }
         }

         if (properties.Count > 0)
         {
            text.AppendLine("### Properties");
            text.AppendLine();

            foreach (var property in properties)
               AppendEntry(text, FormatPropertySignature(property), FormatPropertyDescription(property));
         }

         return new GeneratedPage
         {
            FileName = Slug(publicName) + ".md",
            Content = Normalize(text.ToString())
         };
      }

      public GeneratedPage GenerateConstantsPage(IEnumerable<IdlEnum> enums)
      {
         var text = new StringBuilder();

         AppendFrontMatter(text, "Constants", SlugPrefix + "constants");

         text.AppendLine("### Description");
         text.AppendLine();
         text.AppendLine("The constants available in the hMailServer COM API.");
         text.AppendLine();
         text.AppendLine("### Constant Group and values");
         text.AppendLine();

         foreach (var idlEnum in enums.OrderBy(e => e.Name, StringComparer.OrdinalIgnoreCase))
         {
            text.AppendLine("<div class=\"api_method_name\">" + Html(idlEnum.Name) + "</div>");
            text.AppendLine();

            foreach (var value in idlEnum.Values.Where(v => !v.Attributes.IsHidden))
            {
               text.AppendLine("<div class=\"api_description\"> Const " + Html(value.Name) + " = " +
                               value.Value.ToString(System.Globalization.CultureInfo.InvariantCulture) + "</div>");
               text.AppendLine();
            }

            text.AppendLine("<div class=\"api_description\"> </div>");
            text.AppendLine();
         }

         return new GeneratedPage
         {
            FileName = SlugPrefix + "constants.md",
            Content = Normalize(text.ToString())
         };
      }

      // --- Properties ------------------------------------------------------

      private class PropertyInfo
      {
         public string Name { get; set; }
         public IdlMember Getter { get; set; }
         public IdlMember Setter { get; set; }
      }

      private static List<PropertyInfo> GroupProperties(IEnumerable<IdlMember> members)
      {
         var properties = new Dictionary<string, PropertyInfo>(StringComparer.Ordinal);

         foreach (var member in members)
         {
            if (member.Kind == IdlMemberKind.Method)
               continue;

            PropertyInfo property;
            if (!properties.TryGetValue(member.Name, out property))
            {
               property = new PropertyInfo { Name = member.Name };
               properties.Add(member.Name, property);
            }

            if (member.Kind == IdlMemberKind.PropertyGet)
               property.Getter = property.Getter ?? member;
            else
               property.Setter = property.Setter ?? member;
         }

         return properties.Values.OrderBy(p => p.Name, StringComparer.OrdinalIgnoreCase).ToList();
      }

      private string FormatPropertySignature(PropertyInfo property)
      {
         string typeName;
         List<IdlParameter> parameters;

         if (property.Getter != null)
         {
            var retVal = property.Getter.RetVal;
            typeName = retVal == null ? "void" : FormatType(retVal);
            parameters = property.Getter.InParameters.ToList();
         }
         else
         {
            // Write-only property - the value is the last parameter of the setter.
            var setterParameters = property.Setter.InParameters.ToList();
            var value = setterParameters.LastOrDefault();
            typeName = value == null ? "void" : FormatType(value);
            parameters = setterParameters.Take(Math.Max(0, setterParameters.Count - 1)).ToList();
         }

         string signature = typeName + " " + Html(property.Name);
         if (parameters.Count > 0)
            signature += "(" + FormatParameters(parameters) + ")";

         return signature;
      }

      private static string FormatPropertyDescription(PropertyInfo property)
      {
         var source = property.Getter ?? property.Setter;
         string description = Html(source.Attributes.HelpString ?? string.Empty) + "<br>";

         if (property.Setter == null)
            description += " <i>(read-only)</i>";
         else if (property.Getter == null)
            description += " <i>(write-only)</i>";

         return description;
      }

      // --- Formatting helpers ----------------------------------------------

      private string FormatParameters(IEnumerable<IdlParameter> parameters)
      {
         return string.Join(", ", parameters.Select(FormatParameter));
      }

      private string FormatParameter(IdlParameter parameter)
      {
         string text = FormatType(parameter);

         if (!string.IsNullOrEmpty(parameter.Name))
            text += " " + Html(parameter.Name);

         return text;
      }

      /// <summary>Returns the documented type name, linking to the page of the object it refers to.</summary>
      private string FormatType(IdlParameter parameter)
      {
         string idlType = parameter.TypeName ?? string.Empty;

         if (_knownInterfaces.Contains(idlType))
         {
            string publicName = idlType.Substring(InterfacePrefix.Length);
            return "<a href=\"?page=" + Slug(publicName) + "\">" + Html(publicName) + "</a>";
         }

         string mapped;
         if (ScriptTypeNames.TryGetValue(idlType, out mapped))
            return mapped;

         // Enums and anything else are documented using their IDL name.
         return Html(idlType);
      }

      private static void AppendFrontMatter(StringBuilder text, string title, string slug)
      {
         text.AppendLine("---");
         text.AppendLine("title: \"" + title + "\"");
         text.AppendLine("slug: " + slug);
         text.AppendLine("parent: " + ParentSlug);
         text.AppendLine("index: 0");
         text.AppendLine("is_book: false");
         text.AppendLine("---");
         text.AppendLine();
      }

      private static void AppendEntry(StringBuilder text, string signature, string description)
      {
         text.AppendLine("<div class=\"api_method_name\">" + signature + "</div>");
         text.AppendLine();
         text.AppendLine("<div class=\"api_description\">" + description + "</div>");
         text.AppendLine();
      }

      private static string Slug(string publicName)
      {
         return SlugPrefix + publicName.ToLowerInvariant();
      }

      private static string Html(string text)
      {
         return (text ?? string.Empty)
            .Replace("&", "&amp;")
            .Replace("<", "&lt;")
            .Replace(">", "&gt;");
      }

      /// <summary>Trims trailing blank lines and normalizes line endings.</summary>
      private static string Normalize(string text)
      {
         return text.Replace("\r\n", "\n").TrimEnd('\n') + "\n";
      }
   }
}
