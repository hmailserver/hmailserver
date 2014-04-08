// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SQLScriptCreator.Statements
{
   class CreateIndex : IStatement
   {
      public class Column
      {
         public string Name {get;set;}
         public int MaxLength {get;set;}
      }

      public string Name { get; set; }
      public string Table { get; set; }
      public bool Unique { get; set; }
      public bool Clustered { get; set; }

      private List<Column> _columns;
      public List<Column> Columns
      {
         get
         {
            return _columns;
         }
      }

      public CreateIndex()
      {
         _columns = new List<Column>();
      }


   }
}
