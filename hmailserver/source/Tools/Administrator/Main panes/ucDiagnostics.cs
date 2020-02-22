// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using System.IO;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class ucDiagnostics : UserControl, ISettingsControl
   {
      private string _resultString;

      public ucDiagnostics()
      {
         InitializeComponent();
      }

      public bool Dirty
      {
         get { return false; }
      }

      public void LoadData()
      {
         comboLocalDomain.Items.Clear();

         hMailServer.Domains domains = APICreator.Application.Domains;
         string domainNames = domains.Names;
         string[] rows = Microsoft.VisualBasic.Strings.Split(domainNames, "\r\n", -1, Microsoft.VisualBasic.CompareMethod.Binary);

         List<ListViewItem> items = new List<ListViewItem>();

         comboLocalDomain.AddItem("", null);

         foreach (string row in rows)
         {
            if (string.IsNullOrEmpty(row))
               continue;

            string[] properties = Microsoft.VisualBasic.Strings.Split(row, "\t", -1, Microsoft.VisualBasic.CompareMethod.Text);

            int id = Convert.ToInt32(properties[0]);
            string name = properties[1];
            comboLocalDomain.AddItem(name, null);
         }

         Marshal.ReleaseComObject(domains);
      }

      public bool SaveData()
      {
         return true;
      }

      public void LoadResources()
      {
         
      }

      public void OnLeavePage()
      {
         
      }

      private void buttonPerformTests_Click(object sender, EventArgs e)
      {
         using (WaitCursor cursor = new WaitCursor())
         {
            hMailServer.Diagnostics diagnostics = APICreator.Application.Diagnostics;

            diagnostics.LocalDomainName = comboLocalDomain.Text;
            diagnostics.TestDomainName = textboxTestDomain.Text;

            hMailServer.DiagnosticResults results = diagnostics.PerformTests();

            DisplayResults(results);

            Marshal.ReleaseComObject(results);
            Marshal.ReleaseComObject(diagnostics);
         }

         buttonCopyToClipboard.Enabled = true;
         buttonSaveToFile.Enabled = true;
      }

      private void DisplayResults(hMailServer.DiagnosticResults results)
      {
         treeResults.Nodes.Clear();
         _resultString = "";

         for (int i = 0; i < results.Count; i++)
         {
            hMailServer.DiagnosticResult result = results.get_Item(i);

             
            int imageIndex = result.Result ? 1 : 0;

            _resultString += "Test: " + result.Name + "\r\n";

            TreeNode node = treeResults.Nodes.Add(result.Name, result.Name, imageIndex, imageIndex);
            node.ToolTipText = result.Description;

            string [] subItems = Microsoft.VisualBasic.Strings.Split(result.ExecutionDetails, "\r\n", -1, Microsoft.VisualBasic.CompareMethod.Text);

            foreach (string subItem in subItems)
            {
               if (subItem.Length == 0)
                  continue;

               int subImageIndex = subItem.Contains("ERROR:") ? 0 : 1;

               _resultString += subItem + "\r\n";

               node.Nodes.Add(subItem, subItem, subImageIndex, subImageIndex);
            }

            _resultString += "\r\n";

            Marshal.ReleaseComObject(result);
         }

         treeResults.ExpandAll();

      }


      private void buttonCopyToClipboard_Click(object sender, EventArgs e)
      {
         Clipboard.SetText(_resultString);
      }

      private void buttonSaveToFile_Click(object sender, EventArgs e)
      {
         if (saveFileDialog1.ShowDialog() == DialogResult.OK)
         {
            File.WriteAllText(saveFileDialog1.FileName, _resultString);
         }
      }



   }
}
