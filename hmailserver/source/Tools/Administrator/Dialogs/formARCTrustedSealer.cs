// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System.Windows.Forms;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class formARCTrustedSealer : Form
   {
      public formARCTrustedSealer()
      {
         InitializeComponent();

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
         Strings.Localize(this);
      }

      public void LoadProperties(hMailServer.ARCTrustedSealer sealer)
      {
         textDomain.Text = sealer.Domain;
         textDescription.Text = sealer.Description;
      }

      public void SaveProperties(hMailServer.ARCTrustedSealer sealer)
      {
         sealer.Domain = textDomain.Text.Trim();
         sealer.Description = textDescription.Text;
      }
   }
}
