// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class formSelectLanguage : Form
   {
      private string _language;

      public string Language
      {
         get
         {
            return _language;
         }
      }
      public formSelectLanguage(string language)
      {
         InitializeComponent();

         _language = language;

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
         Strings.Localize(this);

         hMailServer.Languages languages = APICreator.Application.GlobalObjects.Languages;
         for (int i = 0; i < languages.Count; i++)
         {
            hMailServer.Language lang = languages[i];

            comboLanguage.AddItem(lang.Name, lang);
         }

         comboLanguage.Text = _language;
      }

      private void btnOK_Click(object sender, EventArgs e)
      {
         _language = comboLanguage.Text;
      }
   }
}