// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Drawing;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Nodes;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class ucIPRange : UserControl, ISettingsControl
   {
      hMailServer.SecurityRange _representedObject;

      public ucIPRange(int securityRangeID)
      {
         InitializeComponent();

         if (securityRangeID > 0)
         {
             hMailServer.SecurityRanges securityRanges = APICreator.SecurityRanges;
             
            try
            {
               _representedObject = securityRanges.get_ItemByDBID(securityRangeID);
            }
            catch (Exception)
            {
               MessageBox.Show(Strings.Localize("The IP range does not exist."), EnumStrings.hMailServerAdministrator,MessageBoxButtons.OK, MessageBoxIcon.Exclamation);

               this.Enabled = false;
            }
            
            Marshal.ReleaseComObject(securityRanges);
         }

         DirtyChecker.SubscribeToChange(this, OnContentChanged);
         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
         dateTimeExpiresTime.Value = DateTime.Now;

         EnableDisable();
      }


      public void OnLeavePage()
      {

      }

      public bool Dirty
      {
         get
         {
            return DirtyChecker.IsDirty(this) &&
                   textName.Text.Length > 0;
         }
      }

      public void LoadData()
      {
         if (_representedObject == null)
            return;

         textName.Text = _representedObject.Name;
         textPriority.Number = _representedObject.Priority;

         textLowerIPAddress.Text = _representedObject.LowerIP;
         textUpperIPAddress.Text = _representedObject.UpperIP;

         checkAllowSMTP.Checked = _representedObject.AllowSMTPConnections;
         checkAllowPOP3.Checked = _representedObject.AllowPOP3Connections;
         checkAllowIMAP.Checked = _representedObject.AllowIMAPConnections;

         checkRequireSMTPAuthLocalToLocal.Checked = _representedObject.RequireSMTPAuthLocalToLocal;
         checkRequireSMTPAuthLocalToExternal.Checked = _representedObject.RequireSMTPAuthLocalToExternal;
         checkRequireSMTPAuthExternalToLocal.Checked = _representedObject.RequireSMTPAuthExternalToLocal;
         checkRequireSMTPAuthExternalToExternal.Checked = _representedObject.RequireSMTPAuthExternalToExternal;

         checkAllowDeliveiesFromL2L.Checked = _representedObject.AllowDeliveryFromLocalToLocal;
         checkAllowDeliveiesFromL2R.Checked = _representedObject.AllowDeliveryFromLocalToRemote;
         checkAllowDeliveiesFromR2L.Checked = _representedObject.AllowDeliveryFromRemoteToLocal;
         checkAllowDeliveiesFromR2R.Checked = _representedObject.AllowDeliveryFromRemoteToRemote;

         checkSpamProtection.Checked = _representedObject.EnableSpamProtection;
         checkAntiVirus.Checked = _representedObject.EnableAntiVirus;
         checkRequireSSLTLSForAuth.Checked = _representedObject.RequireSSLTLSForAuth;

         checkExpires.Checked = _representedObject.Expires;
         dateTimeExpiresTime.Value = Convert.ToDateTime(_representedObject.ExpiresTime);

         EnableDisable();
      }

      private void EnableDisable()
      {
          checkRequireSMTPAuthLocalToLocal.Enabled = checkAllowDeliveiesFromL2L.Checked;
          checkRequireSMTPAuthLocalToExternal.Enabled = checkAllowDeliveiesFromL2R.Checked;
          checkRequireSMTPAuthExternalToLocal.Enabled = checkAllowDeliveiesFromR2L.Checked;
          checkRequireSMTPAuthExternalToExternal.Enabled = checkAllowDeliveiesFromR2R.Checked;
      }

      public bool SaveData()
      {
         bool newObject = false;
         if (_representedObject == null)
         {
            hMailServer.Application app = APICreator.Application;
            hMailServer.Settings settings = app.Settings;
            hMailServer.SecurityRanges securityRanges = settings.SecurityRanges;
            _representedObject = securityRanges.Add();
            
            newObject = true;

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(securityRanges);
         }

         _representedObject.Name = textName.Text;
         _representedObject.Priority= textPriority.Number;

         _representedObject.LowerIP = textLowerIPAddress.Text;
         _representedObject.UpperIP = textUpperIPAddress.Text;

         _representedObject.AllowSMTPConnections = checkAllowSMTP.Checked;
         _representedObject.AllowPOP3Connections = checkAllowPOP3.Checked;
         _representedObject.AllowIMAPConnections = checkAllowIMAP.Checked;

         _representedObject.RequireSMTPAuthLocalToLocal = checkRequireSMTPAuthLocalToLocal.Checked;
         _representedObject.RequireSMTPAuthLocalToExternal = checkRequireSMTPAuthLocalToExternal.Checked;
         _representedObject.RequireSMTPAuthExternalToLocal = checkRequireSMTPAuthExternalToLocal.Checked;
         _representedObject.RequireSMTPAuthExternalToExternal = checkRequireSMTPAuthExternalToExternal.Checked;

         _representedObject.AllowDeliveryFromLocalToLocal = checkAllowDeliveiesFromL2L.Checked;
         _representedObject.AllowDeliveryFromLocalToRemote = checkAllowDeliveiesFromL2R.Checked;
         _representedObject.AllowDeliveryFromRemoteToLocal = checkAllowDeliveiesFromR2L.Checked;
         _representedObject.AllowDeliveryFromRemoteToRemote = checkAllowDeliveiesFromR2R.Checked;

         _representedObject.EnableSpamProtection = checkSpamProtection.Checked;
         _representedObject.EnableAntiVirus = checkAntiVirus.Checked;
         _representedObject.RequireSSLTLSForAuth = checkRequireSSLTLSForAuth.Checked;

         _representedObject.Expires = checkExpires.Checked;
         _representedObject.ExpiresTime = dateTimeExpiresTime.Value;

         try
         {
            _representedObject.Save();
         }
         catch (Exception e)
         {
            MessageBox.Show(e.Message, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return false;
         }
         
         
         // Refresh the node in the tree if the name has changed.
         IMainForm mainForm = Instances.MainForm;
         mainForm.RefreshCurrentNode(checkExpires.Checked ? Color.Red : Color.Black, textName.Text);

         // Set the object to clean.
         DirtyChecker.SetClean(this);

         if (newObject)
         {
            SearchNodeText crit = new SearchNodeText(_representedObject.Name);
            mainForm.SelectNode(crit);
         }

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

      private void checkExpires_CheckedChanged(object sender, EventArgs e)
      {
         dateTimeExpiresTime.Enabled = checkExpires.Checked;
      }

      private void checkRequireSMTPAuthentication_CheckedChanged(object sender, EventArgs e)
      {
          EnableDisable();
      }

      private void checkAllowDeliveiesFromL2L_CheckedChanged(object sender, EventArgs e)
      {
          EnableDisable();
      }

      private void checkAllowDeliveiesFromL2R_CheckedChanged(object sender, EventArgs e)
      {
          EnableDisable();
      }

      private void checkAllowDeliveiesFromR2L_CheckedChanged(object sender, EventArgs e)
      {
          EnableDisable();
      }

      private void checkAllowDeliveiesFromR2R_CheckedChanged(object sender, EventArgs e)
      {
          EnableDisable();
      }
    
   }
}
