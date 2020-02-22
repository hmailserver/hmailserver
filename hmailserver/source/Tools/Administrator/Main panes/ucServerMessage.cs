using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class ucServerMessage : UserControl, ISettingsControl
   {
      hMailServer.ServerMessage _representedObject;

      public ucServerMessage(int serverMessageID)
      {
         InitializeComponent();

         hMailServer.Settings settings = APICreator.Settings;
         hMailServer.ServerMessages serverMessages = settings.ServerMessages;
         _representedObject = serverMessages.get_ItemByDBID(serverMessageID);
         Marshal.ReleaseComObject(settings);
         Marshal.ReleaseComObject(serverMessages);

         DirtyChecker.SubscribeToChange(this, OnContentChanged);

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


         textMessage.Text = _representedObject.Text;
      }  

      public bool SaveData()
      {
         _representedObject.Text = textMessage.Text;

         _representedObject.Save();

         DirtyChecker.SetClean(this);

         return true;
      }

      public void LoadResources()
      {
         // load the translated resources
      }

      private void OnContentChanged()
      {
         Instances.MainForm.OnContentChanged();
      }

      private void OnContentChanged(object sender, EventArgs e)
      {
         OnContentChanged();
      }

   }
}
