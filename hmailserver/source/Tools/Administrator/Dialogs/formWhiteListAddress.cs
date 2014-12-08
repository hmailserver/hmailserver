using System.Windows.Forms;
using hMailServer.Shared;


namespace hMailServer.Administrator
{
   public partial class formWhiteListAddress : Form
   {
      public formWhiteListAddress()
      {
         InitializeComponent();

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
         Strings.Localize(this);
      }

      public void SaveProperties(hMailServer.WhiteListAddress address)
      {
         address.LowerIPAddress = textLowerIP.Text;
         address.UpperIPAddress = textUpperIP.Text;
         address.Description = textDescription.Text;
         address.EmailAddress = textEmailAddress.Text;
      }

      public void LoadProperties(hMailServer.WhiteListAddress address)
      {
         textLowerIP.Text = address.LowerIPAddress;
         textUpperIP.Text = address.UpperIPAddress;
         textDescription.Text = address.Description;
         textEmailAddress.Text = address.EmailAddress;
      }
   }
}