// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class StringNode
   {
   public:
   
      void SetValue(const String &sValue) {value_ = sValue; }

      vector<shared_ptr<StringNode> > GetSubNodes() {return sub_nodes_;}

   private:

      String value_;
      vector<shared_ptr<StringNode> > sub_nodes_;
   };

   class StringTreeParser
   {
   public:

      StringTreeParser(void);
      ~StringTreeParser(void);

      void Parse(const String &sCommand, vector<shared_ptr<StringNode> > & vecNodes);

   private:

      int c_iCurState;
   };
}