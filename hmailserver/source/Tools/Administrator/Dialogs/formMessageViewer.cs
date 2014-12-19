// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Shared;

namespace hMailServer.Administrator.Dialogs
{
   public partial class formMessageViewer : Form
   {
      private string _filename;
      public formMessageViewer(string fileName)
      {
         InitializeComponent();

         _filename = fileName;

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
         Strings.Localize(this);
      }

      private void formMessageViewer_Shown(object sender, EventArgs e)
      {
         this.Cursor = Cursors.WaitCursor;

         textFile.Text = _filename; 
         
         try
         {
            string fileContent = System.IO.File.ReadAllText(_filename);
            textMessage.Text = fileContent;

         }
         catch (System.IO.FileNotFoundException)
         {
             textMessage.Text =
                 string.Format(
                     "The file {0} could not be loaded. The message has probably been delivered to the recipient and is no longer available in the queue.",
                     _filename);
         }
         catch (Exception ex)
         {
            textMessage.Text = ex.Message;
         }

         this.Cursor = Cursors.Default;
      }

      private void buttonClose_Click(object sender, EventArgs e)
      {
         
      }


      
   }
}