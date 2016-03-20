// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using hMailServer.Administrator.Dialogs;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class ucIMAPFolders : UserControl
   {
      private hMailServer.IMAPFolders _folders;
      private string _folderHierarchyDelimiter;
      public ucIMAPFolders()
      {
         InitializeComponent();

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

         Strings.Localize(contextMenuStrip);

      }

      public bool SaveCurrentFolder()
      {
         TreeNode currentNode = treeFolders.SelectedNode;
         if (currentNode != null)
         {
            hMailServer.IMAPFolder folder = currentNode.Tag as hMailServer.IMAPFolder;
            if (folder.Name != currentNode.Text && currentNode.Text.Length > 0)
            {
               // Name has been changed. First check that it's not busy.
               if (!ValidateFolderName())
                  return false;

               try
               {
                  folder.Name = currentNode.Text;
                  folder.Save();
                  return true;
               }
               catch (Exception e)
               {
                  MessageBox.Show(e.Message, EnumStrings.hMailServerAdministrator);
                  return false;
               }
            }
         }

         return true;
      }

      private bool ValidateFolderName()
      {
         
         TreeNode currentNode = treeFolders.SelectedNode;
         var parentNode = currentNode.Parent;

         List<TreeNode> nodesToCheck = new List<TreeNode>();

         if (parentNode == null)
         {
            foreach (TreeNode node in treeFolders.Nodes)
               nodesToCheck.Add(node);
         }
         else
         {
            foreach (TreeNode node in parentNode.Nodes)
               nodesToCheck.Add(node);
         }

         for (int i = 0; i < nodesToCheck.Count; i++)
         {
            var node = nodesToCheck[i];
            if (node != currentNode)
            {
               if (node.Text == currentNode.Text)
               {
                  MessageBox.Show(Strings.Localize("There is already an folder with this name."),
                     EnumStrings.hMailServerAdministrator);

                  textName.Focus();

                  return false;
               }
            }
         }

         if (textName.Text.Length > 255)
            return false;

         if (textName.Text.ToUpper() == "INBOX")
            return false;

         if (textName.Text.Contains(_folderHierarchyDelimiter))
         {
            MessageBox.Show(Strings.Localize("A folder name can not contain the hierarchy delimiter."), EnumStrings.hMailServerAdministrator);
            return false;
         }

         

         return true;
      }

      public void LoadProperties(hMailServer.IMAPFolders imapFolders, bool publicFolders)
      {
         _folderHierarchyDelimiter = Utilities.APICreator.Settings.IMAPHierarchyDelimiter;

         btnEditPermissions.Visible = publicFolders;

         _folders = imapFolders;

         for (int i = 0; i < _folders.Count; i++)
         {
            hMailServer.IMAPFolder folder = _folders[i];

            TreeNode node = treeFolders.Nodes.Add(folder.Name);
            node.Tag = folder;

            LoadSubFolders(folder, node);
         }

         if (treeFolders.Nodes.Count > 0)
            treeFolders.SelectedNode = treeFolders.Nodes[0];

         ShowCurrentFolder();

      }

      public void Localize()
      {
         Strings.Localize(groupBox);

      }

      private void EnableDisable()
      {
         bool enabled = treeFolders.SelectedNode != null;

         btnEditPermissions.Enabled = enabled;
         textName.Enabled = enabled && textName.Text.ToUpper() != "INBOX";
         buttonEmptyFolder.Enabled = enabled;
      }

      private void LoadSubFolders(hMailServer.IMAPFolder folder, TreeNode node)
      {
         hMailServer.IMAPFolders folders = folder.SubFolders;

         for (int i = 0; i < folders.Count; i++)
         {
            hMailServer.IMAPFolder subFolder = folders[i];

            TreeNode subNode = node.Nodes.Add(subFolder.Name);
            subNode.Tag = subFolder;

            LoadSubFolders(subFolder, subNode);
         }
      }

      private void buttonSave_Click(object sender, EventArgs e)
      {

      }

      private void treeFolders_AfterSelect(object sender, TreeViewEventArgs e)
      {
         ShowCurrentFolder();
      }

      private void ShowCurrentFolder()
      {
         TreeNode selectedNode = treeFolders.SelectedNode;
         if (selectedNode == null)
         {
            EnableDisable();
            return;
         }

         hMailServer.IMAPFolder folder = selectedNode.Tag as IMAPFolder;
         textName.Text = folder.Name;

         EnableDisable();
         
      }

      private void btnEditPermissions_Click(object sender, EventArgs e)
      {
         TreeNode selectedNode = treeFolders.SelectedNode;
         if (selectedNode == null)
            return;

         // First confirm that we can access the permissions.
         hMailServer.IMAPFolder folder = selectedNode.Tag as hMailServer.IMAPFolder;

         try
         {
            // This is a dummy test. We should never return here, but if we don't
            // have access to the ACL permissions, a message should be shown.
            if (folder.Permissions.Count < 0)
               return;
         }
         catch (Exception ex)
         {
            MessageBox.Show(ex.Message, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Information);
            return;
         }
         
         formFolderPermissions folderPermissions = new formFolderPermissions(folder);

         folderPermissions.ShowDialog();
      }

      private void buttonSave_Click_1(object sender, EventArgs e)
      {

      }

      private void textName_TextChanged(object sender, EventArgs e)
      {
         TreeNode selectedNode = treeFolders.SelectedNode;
         if (selectedNode != null)
            selectedNode.Text = textName.Text;
      }

      private void buttonEmptyFolder_Click(object sender, EventArgs e)
      {
         TreeNode selectedNode = treeFolders.SelectedNode;
         if (selectedNode == null)
            return;

         if (MessageBox.Show(Strings.Localize("Are you sure you want to delete all messages in the folder?"),
            EnumStrings.hMailServerAdministrator,
            MessageBoxButtons.YesNo,
            MessageBoxIcon.Question) == DialogResult.Yes)
         {
            hMailServer.IMAPFolder folder = selectedNode.Tag as IMAPFolder;

            folder.Messages.Clear();
         }
      }

      private void deleteFolderToolStripMenuItem_Click(object sender, EventArgs e)
      {
         TreeNode selectedNode = treeFolders.SelectedNode;
         if (selectedNode == null)
            return;

         if (MessageBox.Show(Strings.Localize("Are you sure you want to delete the folder?"),
            EnumStrings.hMailServerAdministrator,
            MessageBoxButtons.YesNo,
            MessageBoxIcon.Question) == DialogResult.Yes)
         {
            hMailServer.IMAPFolder folder = selectedNode.Tag as IMAPFolder;

            folder.Delete();

            treeFolders.Nodes.Remove(selectedNode);

            ShowCurrentFolder();
         }
      }

      private void contextMenuStrip_Opening(object sender, CancelEventArgs e)
      {
         
         TreeNode selectedNode = treeFolders.SelectedNode;
         if (selectedNode == null)
         {
            createFolderToolStripMenuItem.Enabled = true;
            deleteFolderToolStripMenuItem.Enabled = false;
         }
         else
         {
            createFolderToolStripMenuItem.Enabled = true;
            deleteFolderToolStripMenuItem.Enabled = true;
         }
      }

      private void createFolderToolStripMenuItem_Click(object sender, EventArgs e)
      {
         try
         {
            SaveCurrentFolder();

            TreeNode selectedNode = treeFolders.SelectedNode;

            TreeNode newNode = null;
            hMailServer.IMAPFolder folder;
            if (selectedNode != null)
            {
               hMailServer.IMAPFolder parentFolder = selectedNode.Tag as hMailServer.IMAPFolder;
               folder = parentFolder.SubFolders.Add("Unnamed");
               folder.Save();

               newNode = selectedNode.Nodes.Add(folder.Name);
               newNode.Tag = folder;
            }
            else
            {
               folder = _folders.Add("Unnamed");
               folder.Save();

               newNode = treeFolders.Nodes.Add(folder.Name);
               newNode.Tag = folder;
            }

            treeFolders.SelectedNode = newNode;

            textName.Focus();
            textName.SelectAll();
         }
         catch (Exception ex)
         {
            MessageBox.Show("Creation of folder failed." + Environment.NewLine + ex.Message, EnumStrings.hMailServerAdministrator);
         }

      }

      private void treeFolders_BeforeSelect(object sender, TreeViewCancelEventArgs e)
      {
         if (!SaveCurrentFolder())
         {
            e.Cancel = true;
         }
      }

      private void treeFolders_MouseUp(object sender, MouseEventArgs e)
      {
         if (e.Button == MouseButtons.Right)
         {
            TreeViewHitTestInfo hitTest = treeFolders.HitTest(e.Location);

            // Determine whether we should display a context menu or not.
            TreeNode currentNode = hitTest.Node;

            // Save the current folder before we switch to the new one..
            SaveCurrentFolder();

            // Switch to the new node. We do this to make sure that the
            // event handler can refresh the tree properly.
            treeFolders.SelectedNode = currentNode;

            // Display the context menu.
            contextMenuStrip.Show(treeFolders.PointToScreen(e.Location));
         }
      }

   }
}
