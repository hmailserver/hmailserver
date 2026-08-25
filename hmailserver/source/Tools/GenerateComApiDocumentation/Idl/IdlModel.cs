using System.Collections.Generic;
using System.Linq;

namespace GenerateComApiDocumentation.Idl
{
   /// <summary>
   /// Attributes found inside an IDL [ ... ] block, e.g. [propget, id(1), helpstring("Active")].
   /// </summary>
   internal class IdlAttributes
   {
      public IdlAttributes(IEnumerable<string> names, string helpString)
      {
         Names = new HashSet<string>(names, System.StringComparer.OrdinalIgnoreCase);
         HelpString = helpString;
      }

      public static IdlAttributes Empty => new IdlAttributes(new string[0], null);

      public HashSet<string> Names { get; }
      public string HelpString { get; }

      public bool Has(string name) => Names.Contains(name);
      public bool IsHidden => Has("hidden") || Has("restricted");
   }

   internal class IdlParameter
   {
      public IdlAttributes Attributes { get; set; } = IdlAttributes.Empty;
      public string TypeName { get; set; }
      public int PointerDepth { get; set; }
      public string Name { get; set; }

      public bool IsRetVal => Attributes.Has("retval");
   }

   internal enum IdlMemberKind
   {
      Method,
      PropertyGet,
      PropertyPut
   }

   internal class IdlMember
   {
      public IdlAttributes Attributes { get; set; } = IdlAttributes.Empty;
      public string Name { get; set; }
      public IdlMemberKind Kind { get; set; }
      public List<IdlParameter> Parameters { get; } = new List<IdlParameter>();

      public IdlParameter RetVal => Parameters.FirstOrDefault(p => p.IsRetVal);
      public IEnumerable<IdlParameter> InParameters => Parameters.Where(p => !p.IsRetVal);
   }

   internal class IdlInterface
   {
      public IdlAttributes Attributes { get; set; } = IdlAttributes.Empty;

      /// <summary>Name as written in the IDL, e.g. IInterfaceAccount.</summary>
      public string InterfaceName { get; set; }

      /// <summary>Name used in scripts, e.g. Account.</summary>
      public string PublicName =>
         InterfaceName.StartsWith("IInterface")
            ? InterfaceName.Substring("IInterface".Length)
            : InterfaceName.TrimStart('I');

      public List<IdlMember> Members { get; } = new List<IdlMember>();
   }

   internal class IdlEnumValue
   {
      public IdlAttributes Attributes { get; set; } = IdlAttributes.Empty;
      public string Name { get; set; }
      public long Value { get; set; }
   }

   internal class IdlEnum
   {
      public IdlAttributes Attributes { get; set; } = IdlAttributes.Empty;
      public string Name { get; set; }
      public List<IdlEnumValue> Values { get; } = new List<IdlEnumValue>();
   }

   internal class IdlDocument
   {
      public List<IdlInterface> Interfaces { get; } = new List<IdlInterface>();
      public List<IdlEnum> Enums { get; } = new List<IdlEnum>();
   }
}
