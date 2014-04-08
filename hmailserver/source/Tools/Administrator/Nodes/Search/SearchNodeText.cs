// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace hMailServer.Administrator.Nodes
{
   public class SearchNodeText : ISearchNodeCriteria
   {
      private string _title;

      public SearchNodeText(string title)
      {
         _title = title;
      }

      public bool IsMatch(TreeNode node)
      {
         if (node.Text == _title)
            return true;
         else
            return false;
      }


   }
}
