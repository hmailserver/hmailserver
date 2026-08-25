using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GenerateComApiDocumentation.Idl
{
   /// <summary>
   /// Minimal IDL reader. It understands the constructs used by hMailServer.idl:
   /// typedef'ed enums and dual IDispatch interfaces. Everything else is skipped.
   /// </summary>
   internal class IdlParser
   {
      private readonly string _text;
      private int _pos;

      private IdlParser(string text)
      {
         _text = text;
      }

      public static IdlDocument ParseFile(string path)
      {
         return ParseText(System.IO.File.ReadAllText(path));
      }

      public static IdlDocument ParseText(string text)
      {
         return new IdlParser(StripComments(text)).Parse();
      }

      private IdlDocument Parse()
      {
         var document = new IdlDocument();
         IdlAttributes pendingAttributes = IdlAttributes.Empty;

         while (true)
         {
            SkipWhitespace();
            if (Eof)
               break;

            char c = _text[_pos];

            if (c == '[')
            {
               pendingAttributes = ParseAttributes(ReadBalanced('[', ']'));
               continue;
            }

            if (!IsIdentifierChar(c))
            {
               _pos++;
               continue;
            }

            string word = ReadIdentifier();

            switch (word)
            {
               case "interface":
                  var parsedInterface = ParseInterface(pendingAttributes);
                  if (parsedInterface != null)
                     document.Interfaces.Add(parsedInterface);
                  break;
               case "typedef":
                  var parsedEnum = ParseTypedefEnum();
                  if (parsedEnum != null)
                     document.Enums.Add(parsedEnum);
                  break;
               case "library":
               case "dispinterface":
               case "coclass":
               case "module":
                  SkipDeclaration();
                  break;
            }

            pendingAttributes = IdlAttributes.Empty;
         }

         return document;
      }

      /// <summary>
      /// Parses an interface definition. Forward declarations (interface IFoo;) return null.
      /// </summary>
      private IdlInterface ParseInterface(IdlAttributes attributes)
      {
         SkipWhitespace();
         string name = ReadIdentifier();

         SkipWhitespace();
         if (Eof || _text[_pos] == ';')
            return null;

         if (_text[_pos] == ':')
         {
            _pos++;
            SkipWhitespace();
            ReadIdentifier();
            SkipWhitespace();
         }

         if (Eof || _text[_pos] != '{')
            return null;

         var result = new IdlInterface { Attributes = attributes, InterfaceName = name };

         foreach (var declaration in SplitTopLevel(ReadBalanced('{', '}'), ';'))
         {
            var member = ParseMember(declaration);
            if (member != null)
               result.Members.Add(member);
         }

         return result;
      }

      private static IdlMember ParseMember(string declaration)
      {
         var rest = declaration.TrimStart();
         var attributes = IdlAttributes.Empty;

         if (rest.StartsWith("["))
         {
            int end = FindBalanced(rest, 0, '[', ']');
            if (end < 0)
               return null;
            attributes = ParseAttributes(rest.Substring(1, end - 1));
            rest = rest.Substring(end + 1);
         }

         int parenStart = rest.IndexOf('(');
         if (parenStart < 0)
            return null;

         // The return type is always HRESULT, so the last word before "(" is the member name.
         var signature = rest.Substring(0, parenStart)
                             .Split(new[] { ' ', '\t', '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
         if (signature.Length == 0)
            return null;

         var kind = IdlMemberKind.Method;
         if (attributes.Has("propget"))
            kind = IdlMemberKind.PropertyGet;
         else if (attributes.Has("propput") || attributes.Has("propputref"))
            kind = IdlMemberKind.PropertyPut;

         var member = new IdlMember
         {
            Attributes = attributes,
            Name = signature[signature.Length - 1],
            Kind = kind
         };

         int parenEnd = FindBalanced(rest, parenStart, '(', ')');
         if (parenEnd < 0)
            return null;

         string parameterList = rest.Substring(parenStart + 1, parenEnd - parenStart - 1);
         foreach (var part in SplitTopLevel(parameterList, ','))
         {
            var parameter = ParseParameter(part);
            if (parameter != null)
               member.Parameters.Add(parameter);
         }

         return member;
      }

      private static IdlParameter ParseParameter(string text)
      {
         var rest = text.Trim();
         if (rest.Length == 0)
            return null;

         var attributes = IdlAttributes.Empty;
         if (rest.StartsWith("["))
         {
            int end = FindBalanced(rest, 0, '[', ']');
            if (end < 0)
               return null;
            attributes = ParseAttributes(rest.Substring(1, end - 1));
            rest = rest.Substring(end + 1);
         }

         int pointerDepth = rest.Count(c => c == '*');
         var words = rest.Replace("*", " ")
                         .Split(new[] { ' ', '\t', '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries)
                         .Where(w => w != "const")
                         .ToArray();
         if (words.Length == 0)
            return null;

         var parameter = new IdlParameter { Attributes = attributes, PointerDepth = pointerDepth };

         if (words.Length == 1)
         {
            // Type without a parameter name.
            parameter.TypeName = words[0];
         }
         else
         {
            parameter.Name = words[words.Length - 1];
            parameter.TypeName = words[words.Length - 2];
         }

         return parameter;
      }

      /// <summary>
      /// Parses "typedef [attributes] enum { ... } eName;". Returns null for other typedefs.
      /// </summary>
      private IdlEnum ParseTypedefEnum()
      {
         var attributes = IdlAttributes.Empty;

         while (true)
         {
            SkipWhitespace();
            if (Eof)
               return null;

            if (_text[_pos] == '[')
            {
               attributes = ParseAttributes(ReadBalanced('[', ']'));
               continue;
            }

            if (_text[_pos] == '{')
               break;

            if (_text[_pos] == ';')
               return null;

            if (!IsIdentifierChar(_text[_pos]))
            {
               _pos++;
               continue;
            }

            if (ReadIdentifier() != "enum")
               return null;
         }

         string body = ReadBalanced('{', '}');

         SkipWhitespace();
         string name = ReadIdentifier();
         if (string.IsNullOrEmpty(name))
            return null;

         var result = new IdlEnum { Attributes = attributes, Name = name };

         long next = 0;
         foreach (var entry in SplitTopLevel(body, ','))
         {
            var value = ParseEnumValue(entry, ref next);
            if (value != null)
               result.Values.Add(value);
         }

         return result;
      }

      private static IdlEnumValue ParseEnumValue(string text, ref long next)
      {
         var rest = text.Trim();
         if (rest.Length == 0)
            return null;

         var attributes = IdlAttributes.Empty;
         while (rest.StartsWith("["))
         {
            int end = FindBalanced(rest, 0, '[', ']');
            if (end < 0)
               return null;
            attributes = ParseAttributes(rest.Substring(1, end - 1));
            rest = rest.Substring(end + 1).TrimStart();
         }

         string name;
         long value;

         int equals = rest.IndexOf('=');
         if (equals >= 0)
         {
            name = rest.Substring(0, equals).Trim();
            if (!TryParseInteger(rest.Substring(equals + 1), out value))
               value = next;
         }
         else
         {
            name = rest.Trim();
            value = next;
         }

         if (name.Length == 0)
            return null;

         next = value + 1;
         return new IdlEnumValue { Attributes = attributes, Name = name, Value = value };
      }

      private static bool TryParseInteger(string text, out long value)
      {
         text = text.Trim();

         if (text.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
         {
            try
            {
               value = Convert.ToInt64(text.Substring(2), 16);
               return true;
            }
            catch (Exception)
            {
               value = 0;
               return false;
            }
         }

         return long.TryParse(text, out value);
      }

      private static IdlAttributes ParseAttributes(string text)
      {
         var names = new List<string>();
         string helpString = null;

         foreach (var attribute in SplitTopLevel(text, ','))
         {
            var trimmed = attribute.Trim();
            if (trimmed.Length == 0)
               continue;

            int paren = trimmed.IndexOf('(');
            string name = (paren >= 0 ? trimmed.Substring(0, paren) : trimmed).Trim();
            names.Add(name);

            if (paren >= 0 && name.Equals("helpstring", StringComparison.OrdinalIgnoreCase))
            {
               int end = FindBalanced(trimmed, paren, '(', ')');
               if (end > paren)
                  helpString = UnquoteString(trimmed.Substring(paren + 1, end - paren - 1));
            }
         }

         return new IdlAttributes(names, helpString);
      }

      private static string UnquoteString(string text)
      {
         text = text.Trim();
         if (text.Length >= 2 && text[0] == '"' && text[text.Length - 1] == '"')
            text = text.Substring(1, text.Length - 2);

         return Unescape(text);
      }

      /// <summary>
      /// Resolves the escape sequences of a string literal. Unknown ones are kept as written,
      /// since the IDL contains text such as "\SEEN" which is not an escape sequence.
      /// </summary>
      private static string Unescape(string text)
      {
         var result = new StringBuilder(text.Length);

         for (int i = 0; i < text.Length; i++)
         {
            if (text[i] != '\\' || i + 1 >= text.Length)
            {
               result.Append(text[i]);
               continue;
            }

            char escaped = text[i + 1];
            switch (escaped)
            {
               case 't': result.Append('\t'); break;
               case 'n': result.Append('\n'); break;
               case 'r': result.Append('\r'); break;
               case '"': result.Append('"'); break;
               case '\\': result.Append('\\'); break;
               default:
                  result.Append('\\').Append(escaped);
                  break;
            }

            i++;
         }

         return result.ToString();
      }

      // --- Scanner helpers -------------------------------------------------

      private bool Eof
      {
         get { return _pos >= _text.Length; }
      }

      private void SkipWhitespace()
      {
         while (_pos < _text.Length && char.IsWhiteSpace(_text[_pos]))
            _pos++;
      }

      private static bool IsIdentifierChar(char c)
      {
         return char.IsLetterOrDigit(c) || c == '_';
      }

      private string ReadIdentifier()
      {
         SkipWhitespace();
         int start = _pos;
         while (_pos < _text.Length && IsIdentifierChar(_text[_pos]))
            _pos++;
         return _text.Substring(start, _pos - start);
      }

      /// <summary>Reads a balanced block starting at the current position and returns its content.</summary>
      private string ReadBalanced(char open, char close)
      {
         SkipWhitespace();
         int end = FindBalanced(_text, _pos, open, close);
         if (end < 0)
         {
            _pos = _text.Length;
            return string.Empty;
         }

         string content = _text.Substring(_pos + 1, end - _pos - 1);
         _pos = end + 1;
         return content;
      }

      /// <summary>Skips forward past the next declaration, including any braced body.</summary>
      private void SkipDeclaration()
      {
         while (_pos < _text.Length)
         {
            char c = _text[_pos];

            if (c == '{')
            {
               ReadBalanced('{', '}');
               SkipWhitespace();
               if (!Eof && _text[_pos] == ';')
                  _pos++;
               return;
            }

            if (c == ';')
            {
               _pos++;
               return;
            }

            _pos++;
         }
      }

      /// <summary>Returns the index of the closing character matching the first opening one at or after start.</summary>
      private static int FindBalanced(string text, int start, char open, char close)
      {
         while (start < text.Length && text[start] != open)
            start++;
         if (start >= text.Length)
            return -1;

         int depth = 0;
         for (int i = start; i < text.Length; i++)
         {
            char c = text[i];

            if (c == '"')
            {
               i = SkipStringLiteral(text, i);
               continue;
            }

            if (c == open)
            {
               depth++;
            }
            else if (c == close)
            {
               depth--;
               if (depth == 0)
                  return i;
            }
         }

         return -1;
      }

      /// <summary>Splits on a separator that is not nested inside (), [], {} or a string literal.</summary>
      private static List<string> SplitTopLevel(string text, char separator)
      {
         var parts = new List<string>();
         var current = new StringBuilder();
         int depth = 0;

         for (int i = 0; i < text.Length; i++)
         {
            char c = text[i];

            if (c == '"')
            {
               int end = SkipStringLiteral(text, i);
               current.Append(text, i, end - i + 1);
               i = end;
               continue;
            }

            if (c == '(' || c == '[' || c == '{')
            {
               depth++;
            }
            else if (c == ')' || c == ']' || c == '}')
            {
               depth--;
            }
            else if (c == separator && depth == 0)
            {
               parts.Add(current.ToString());
               current.Clear();
               continue;
            }

            current.Append(c);
         }

         if (current.ToString().Trim().Length > 0)
            parts.Add(current.ToString());

         return parts;
      }

      /// <summary>Returns the index of the closing quote of the string literal starting at start.</summary>
      private static int SkipStringLiteral(string text, int start)
      {
         for (int i = start + 1; i < text.Length; i++)
         {
            if (text[i] == '\\')
            {
               i++;
               continue;
            }

            if (text[i] == '"')
               return i;
         }

         return text.Length - 1;
      }

      /// <summary>Removes // and /* */ comments without touching string literals.</summary>
      private static string StripComments(string text)
      {
         var result = new StringBuilder(text.Length);

         for (int i = 0; i < text.Length; i++)
         {
            char c = text[i];

            if (c == '"')
            {
               int end = SkipStringLiteral(text, i);
               result.Append(text, i, end - i + 1);
               i = end;
               continue;
            }

            if (c == '/' && i + 1 < text.Length)
            {
               if (text[i + 1] == '/')
               {
                  while (i < text.Length && text[i] != '\n')
                     i++;
                  result.Append('\n');
                  continue;
               }

               if (text[i + 1] == '*')
               {
                  int end = text.IndexOf("*/", i + 2, StringComparison.Ordinal);
                  i = end < 0 ? text.Length : end + 1;
                  result.Append(' ');
                  continue;
               }
            }

            result.Append(c);
         }

         return result.ToString();
      }
   }
}
