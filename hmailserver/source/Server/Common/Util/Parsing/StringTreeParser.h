// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class StringNode
   {
   public:
   
      void SetValue(const String &sValue) {m_sValue = sValue; }

      vector<shared_ptr<StringNode> > GetSubNodes() {return m_vecSubNodes;}

   private:

      String m_sValue;
      vector<shared_ptr<StringNode> > m_vecSubNodes;
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