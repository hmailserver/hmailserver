// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SQLScriptCreator.Statements;

namespace SQLScriptCreator.Generators
{
   interface IGenerator
   {
      string Name { get; }

      string StatementSeparator { get; }

      List<string> GenerateRenameColumnStatement(RenameColumn renameColumn);
      List<string> GenerateAddColumnStatement(AddColumn addColumn);
      List<string> GenerateCreateIndexStatement(CreateIndex createIndex);
      List<string> GenerateDropColumnObjectsStatement(DropColumnObjects statement);
      List<string> GenerateRawSQLStatement(RawSQL statement);
   }
}
