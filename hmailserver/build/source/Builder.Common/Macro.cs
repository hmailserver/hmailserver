// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

namespace Builder.Common
{
   internal class Macro
   {
      private readonly string _name;
      private readonly string _value;

      public Macro(string name, string value)
      {
         _name = name;
         _value = value;
      }

      public string Name
      {
         get { return _name; }
      }

      public string Value
      {
         get { return _value; }
      }
   }
}