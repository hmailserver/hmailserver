// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace DBUpdater
{
   class UpgradeScript
   {
      private int _from = 0;
      private int _to = 0;
      private bool _dummy = false;


      public UpgradeScript(int from, int to)
      {
         _from = from;
         _to = to;
      }

      public UpgradeScript(int from, int to, bool dummy)
      {
         _from = from;
         _to = to;
         _dummy = true;
      }

      public bool Dummy
      {
         get
         {
            return _dummy;
         }
         set
         {
            _dummy = value;
         }


      }

      public int From
      {
         get
         {
            return _from;
         }
      }

      public int To
      {
         get
         {
            return _to;
         }
      }
   }
}
