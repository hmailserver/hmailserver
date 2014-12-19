// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class ucGlobalRules : UserControl, ISettingsControl
   {
      public ucGlobalRules()
      {
         InitializeComponent();

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
      }

      public void OnLeavePage()
      {

      }

      public bool Dirty
      {
         get
         {
            return DirtyChecker.IsDirty(this);
         }
      }

      public void LoadData()
      {
         hMailServer.Rules globalRules = APICreator.Application.Rules;
         rules.LoadRules(globalRules);
      }

      public bool SaveData()
      {
         return true;
      }

      public void LoadResources()
      {
         // load the translated resources
      }
   }
}
