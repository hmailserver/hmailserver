// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Utilities;
using System.Reflection;
using Microsoft.Win32;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class formMain : Form, IMainForm
    {
        private hMailServer.Application application;
        private string _serverHost;
        private TreeNode currentlySelectedNode;
        private string _language;

        public formMain(hMailServer.Application app, string serverHost)
        {
            InitializeComponent();

            application = app;
            _serverHost = serverHost;
            currentlySelectedNode = null;

            _language = "English";

            LoadWindowSettings();

            _language = Strings.LoadLanguage(_language);
            Strings.Localize(this);

        }

        public void ShowItem(INode node)
        {
            ShowNodeRepresentation(node);
        }

        private void buttonExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void formMain_Load(object sender, EventArgs e)
        {
            Instances.MainForm = this;

            Initialize();
        }

        private bool Connect()
        {
           // Should we save the currently viewed?
           if (!AskSaveChanges())
           {
              treeNodes.SelectedNode = currentlySelectedNode;
              return false;
           }

            // Connect
            formConnect connectDlg = new formConnect();

            if (connectDlg.ShowDialog() != DialogResult.OK)
                return false;

            application = connectDlg.Application;
            _serverHost = connectDlg.Server;

            Initialize();

            return true;
        }

        private void Initialize()
        {
            CreateMainNodes();

            this.Text = "hMailServer Administrator - [" + _serverHost + "]";

        }

        private void CreateMainNodes()
        {
            treeNodes.Nodes.Clear();

            AddNode(null, new NodeWelcome(), 1);
            AddNode(null, new NodeStatus(), 1);
            AddNode(null, new NodeDomains(this), 1);
            AddNode(null, new NodeGlobalRules(), 1);
            AddNode(null, new NodeSettings(), 1);
            AddNode(null, new NodeUtilities(), 1);

            SearchNodeType crit = new SearchNodeType(typeof(NodeWelcome));
            SelectNode(crit);

            

        }

        private void AddNode(TreeNode parentNode, INode internalNode, int recursion)
        {
            TreeNode node = CreateNode(internalNode);

            if (parentNode != null)
                parentNode.Nodes.Add(node);
            else
                treeNodes.Nodes.Add(node);

            LoadNodeChildren(node, recursion);
        }

        private TreeNode CreateNode(INode internalNode)
        {
            TreeNode node = new TreeNode(internalNode.Title);

            if (!internalNode.IsUserCreated)
                Strings.Localize(node);

            node.NodeFont = new Font("MS Sans Serif", 8, FontStyle.Regular);

            string icon = internalNode.Icon;

            if (icon != null)
            {
               if (imageList.Images.ContainsKey(icon))
               {
                  node.ImageKey = icon;
                  node.SelectedImageKey = icon;
               }
               else
               {
                  throw new Exception("Icon not found for page " + internalNode.Title);
               }
            }

            node.ForeColor = internalNode.ForeColor;
            node.Tag = internalNode;

            return node;
        }
        public bool SelectNode(ISearchNodeCriteria criteria)
        {
            foreach (TreeNode treeNode in treeNodes.Nodes)
            {
                INode node = treeNode.Tag as INode;

                if (criteria.IsMatch(treeNode))
                {
                    if (!SelectedNodeChange(treeNode))
                        return false;

                    treeNodes.SelectedNode = treeNode;
                    treeNode.Expand();
                    // treeNodes.Focus();

                    ShowNodeRepresentation(node);

                    return true;
                }
            }

            foreach (TreeNode treeNode in currentlySelectedNode.Nodes)
            {
                INode node = treeNode.Tag as INode;

                if (criteria.IsMatch(treeNode))
                {
                    if (!SelectedNodeChange(treeNode))
                        return false;

                    treeNodes.SelectedNode = treeNode;
                    treeNode.Expand();
                    //treeNodes.Focus();

                    ShowNodeRepresentation(node);

                    return true;
                }
            }



            return false;
        }


        private void LoadNodeChildren(TreeNode node, int recursionDepth)
        {
            // Don't auto-recurse to deep (performance)
            if (recursionDepth <= 0)
            {
                INode theNode = node.Tag as INode;
                if (theNode.GetType() == typeof(Administrator.Nodes.NodeDomain))
                {
                    // Don't actually list all sub-nodes to all domains. Do it later
                    // instead (see a few lines below)
                    NodeDummy dummy = new NodeDummy();
                    AddNode(node, dummy, 0);
                    return;
                }
                
                return;
            }

            // check if sub items are already loaded
            if (node.Nodes.Count > 0)
                return;

            INode internalNode = (INode)node.Tag;

            List<INode> subNodes = internalNode.SubNodes;
            if (subNodes != null)
            {
                bool lockUpdate = node.IsExpanded;
                if (lockUpdate)
                    treeNodes.BeginUpdate();

                foreach (INode subNode in subNodes)
                {
                    AddNode(node, subNode, recursionDepth-1);
                }

                if (lockUpdate)
                    treeNodes.EndUpdate();
            }
        }

        private bool AskSaveChanges()
        {
            if (panelMain.Controls.Count == 0)
            {
                // No control is currently being displayed. Not much to save here.
                return true;
            }

            // Check if the currently selected control is dirty.
            ISettingsControl settingsControl = panelMain.Controls[0] as ISettingsControl;

            if (settingsControl.Dirty)
            {
                DialogResult dr = MessageBox.Show("Do you want to save the changes?", EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNoCancel);

                switch (dr)
                {
                    case DialogResult.Cancel:
                        return false;
                    case DialogResult.Yes:
                        return SaveCurrentObject();
                    case DialogResult.No:
                        return true;
                }
            }



            return true;
        }

        private bool ShowControl(INode node, UserControl control)
        {

            if (panelMain.Controls.Count > 0)
            {
                // Notify the old page that we're leaving, so that it can take care
                // of cleaning up...
                ISettingsControl oldSettingsControl = panelMain.Controls[0] as ISettingsControl;
                oldSettingsControl.OnLeavePage();
            }


            panelMain.Controls.Clear();
            panelMain.Controls.Add(control);

            control.Dock = DockStyle.Fill;

            ISettingsControl settingsControl = (ISettingsControl)control;
            labelTopCaption.Text = node.Title;

            if (!node.IsUserCreated)
                Strings.Localize(labelTopCaption);

            OnContentChanged();

            

            return true;
        }



        private void menuItemExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }


        private void menuItemConnect_Click(object sender, EventArgs e)
        {
            Connect();
        }

        private void treeNodes_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            TreeNode node = e.Node;

            using (new WaitCursor())
            {
                INode theNode = node.Tag as INode;
                if (theNode.GetType() == typeof(Administrator.Nodes.NodeDomain))
                {
                    // List the sub nodes now.
                    node.Nodes.Clear();
                    LoadNodeChildren(node, 1);
                }

                foreach (TreeNode subNode in node.Nodes)
                {
                    LoadNodeChildren(subNode, 1);
                }
            }
        }

        private void treeNodes_AfterSelect(object sender, TreeViewEventArgs e)
        {
            using (new WaitCursor())
            {
                if (e.Action == TreeViewAction.ByKeyboard || e.Action == TreeViewAction.ByMouse)
                {
                    // Check if the clicked node is the one which is already selected. If so,
                    // just skip this...
                    TreeNode treeNode = e.Node;

                    if (!SelectedNodeChange(treeNode))
                        return;

                    INode node = (INode)treeNode.Tag;
                    ShowNodeRepresentation(node);
                }
            }
        }

        private bool SelectedNodeChange(TreeNode treeNode)
        {
            if (treeNode == currentlySelectedNode)
                return false;

            // Should we save the currently viewed?
            if (!AskSaveChanges())
            {
                treeNodes.SelectedNode = currentlySelectedNode;
                return false;
            }

            currentlySelectedNode = treeNode;

            return true;
        }

        private void ShowNodeRepresentation(INode node)
        {
            UserControl control = node.CreateControl();

            ISettingsControl settingsControl = (ISettingsControl)control;
            settingsControl.LoadData();

            Strings.Localize(control);

            ShowControl(node, control);
        }

        public void RefreshParentNode()
        {
            TreeNode currentNode = treeNodes.SelectedNode;
            TreeNode parentNode = currentNode.Parent;

            parentNode.Nodes.Clear();
            LoadNodeChildren(parentNode, 2);

            parentNode.Expand();
            currentlySelectedNode = parentNode;
               
            INode node = (INode)currentlySelectedNode.Tag;
            ShowNodeRepresentation(node);
        }

        public void RefreshCurrentNode(System.Drawing.Color color, string newName)
        {
           INode internalNode = currentlySelectedNode.Tag as INode;

           internalNode.ForeColor = color;

           if (internalNode.ForeColor != currentlySelectedNode.ForeColor)
              currentlySelectedNode.ForeColor = internalNode.ForeColor;

           RefreshCurrentNode(newName);
        }

        public void RefreshCurrentNode(string newName)
        {
            INode internalNode = currentlySelectedNode.Tag as INode;
            if (!string.IsNullOrEmpty(newName))
                internalNode.Title = newName;

            if (currentlySelectedNode.Text != Strings.Localize(internalNode.Title))
                currentlySelectedNode.Text = Strings.Localize(internalNode.Title);

            currentlySelectedNode.Nodes.Clear();

            labelTopCaption.Text = currentlySelectedNode.Text;

            int recursion = internalNode.GetType() == typeof(Administrator.Nodes.NodeDomains) ? 1 : 2;

            LoadNodeChildren(currentlySelectedNode, recursion);
        }

        public INode GetCurrentNode()
        {
            // Refresh the currently selected node. A bit experimental...
            TreeNode currentNode = treeNodes.SelectedNode;

            INode internalNode = currentNode.Tag as INode;

            return internalNode;

        }
        private void buttonSave_Click(object sender, EventArgs e)
        {
            if (panelMain.Controls.Count == 0)
            {
                // No control is currently being displayed. Not much to save here.
                return;
            }

            SaveCurrentObject();
        }

        private bool SaveCurrentObject()
        {
            // Check if the currently selected control is dirty.
            ISettingsControl settingsControl = panelMain.Controls[0] as ISettingsControl;

            try
            {
                if (!settingsControl.SaveData())
                   return false;

                buttonSave.Enabled = settingsControl.Dirty;

                return true;
            }
            catch (Exception e)
            {
                MessageBox.Show("Changes could not be saved" + Environment.NewLine + e.Message, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK);
            }

            return false;

        }

        private void formMain_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F5)
            {
                // Refresh the currently selected node. A bit experimental...
                RefreshCurrentNode(null);

                if (panelMain.Controls.Count > 0)
                {
                    ISettingsControl settingsControl = panelMain.Controls[0] as ISettingsControl;
                    settingsControl.LoadData();

                    panelMain.Controls[0].Focus();
                }
            }
        }

        private void treeNodes_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                TreeViewHitTestInfo hitTest = treeNodes.HitTest(e.Location);

                // Determine whether we should display a context menu or not.
                TreeNode currentNode = hitTest.Node;
                if (currentNode == null)
                    return;

                INode internalNode = currentNode.Tag as INode;
                if (internalNode == null)
                    return;

                // Let's see if we can create a context menu now.
                ContextMenuStrip menu = internalNode.CreateContextMenu();
                if (menu == null)
                    return;

                /*
                    Switch to the new node. We do this to make sure that the
                    event handler can refresh the tree properly.
                 
                    For example, if the user clicks on a domain, right clicks
                    on the Accounts node and selects Add, we need to do the following:
                    1) Ask the user if he wants to save the domain changes (if any)
                    2) Switch to the Accounts node
                    3) Show the context menu
                 
                    Number 2 isn't strictly required here. However, if we don't change
                    the node, it will be hard for the Account user control to update
                    the tree in the correct location.
                */

                /*if (!ChangeToNode(currentNode))
                    return;
*/
                // Display the context menu.
                menu.Show(treeNodes.PointToScreen(e.Location));

             
            }
        }

        private bool ChangeToNode(TreeNode newNode)
        {
            if (currentlySelectedNode != newNode)
            {
                if (!SelectedNodeChange(newNode))
                    return false;
            }

            treeNodes.SelectedNode = newNode;
            newNode.Expand();

            INode node = (INode)currentlySelectedNode.Tag;
            ShowNodeRepresentation(node);

            return true;
        }

        public void OnContentChanged()
        {
            if (panelMain.Controls.Count == 0)
                return;

            ISettingsControl settingsControl = panelMain.Controls[0] as ISettingsControl;
            buttonSave.Enabled = settingsControl.Dirty;
        }

        private void splitContainer_SplitterMoved(object sender, SplitterEventArgs e)
        {

        }

        private void SaveWindowSettings()
        {
            RegistryKey currentUserKey = Registry.CurrentUser;
            RegistryKey administratorKey = currentUserKey.CreateSubKey("Software\\hMailServer\\Administrator");

            administratorKey.SetValue("WindowState", this.WindowState);

            if (this.WindowState == FormWindowState.Normal)
            {
               administratorKey.SetValue("WindowLeft", this.Left);
               administratorKey.SetValue("WindowTop", this.Top);

               administratorKey.SetValue("WindowWidth", this.Width);
               administratorKey.SetValue("WindowHeight", this.Height);
            }

            administratorKey.SetValue("SplitterDistance", this.splitContainer.SplitterDistance);

            administratorKey.SetValue("Language", _language);
        }

        private void LoadWindowSettings()
        {
            RegistryKey currentUserKey = Registry.CurrentUser;
            RegistryKey administratorKey = currentUserKey.CreateSubKey("Software\\hMailServer\\Administrator");

            object value = administratorKey.GetValue("WindowWidth", "800");
            if (value != null)
                this.Width = Convert.ToInt32(value);

            value = administratorKey.GetValue("WindowHeight", "600");
            if (value != null)
                this.Height = Convert.ToInt32(value);

            value = administratorKey.GetValue("SplitterDistance", "250");
            if (value != null)
                this.splitContainer.SplitterDistance = Convert.ToInt32(value);

            value = administratorKey.GetValue("WindowLeft", "0");
            if (value != null && Convert.ToInt32(value) > 0)
                this.Left = Convert.ToInt32(value);

            value = administratorKey.GetValue("WindowTop", "0");
            if (value != null && Convert.ToInt32(value) > 0)
                this.Top = Convert.ToInt32(value);

            value = administratorKey.GetValue("WindowState", "0");
            if (value != null)
            {
                string val = (string)value;
                switch (val)
                {
                    case "Maximized":
                        this.WindowState = FormWindowState.Maximized;
                        break;
                    case "Minimized":
                        this.WindowState = FormWindowState.Minimized;
                        break;
                    case "Normal":
                        this.WindowState = FormWindowState.Normal;
                        break;
                }
            }

            value = administratorKey.GetValue("Language", "English");
            if (value != null)
                _language = Convert.ToString(value);

        }

        private void formMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            SaveWindowSettings();

            new WaitCursor();

        }

        private void buttonHelp_Click(object sender, EventArgs e)
        {
            if (panelMain.Controls.Count == 0)
                return;

            // Check if the currently selected control is dirty.
            ISettingsControl settingsControl = panelMain.Controls[0] as ISettingsControl;
            Type t = settingsControl.GetType();
            string name = t.Name;

            if (!name.StartsWith("uc"))
                return;

            name = name.Substring(2);
            name = name.ToLower();

            // override...
            switch (name)
            {
                case "globalrules":
                    name = "rule";
                    break;
            }

            string url = "http://www.hmailserver.com/documentation/?page=reference_" + name;

            try
            {
                System.Diagnostics.Process.Start(url);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Web browser could not be started." + Environment.NewLine + ex.Message, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }


        }

        private void menuItemHelpIndex_Click(object sender, EventArgs e)
        {
            string url = "http://www.hmailserver.com/documentation/";

            try
            {
                System.Diagnostics.Process.Start(url);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Web browser could not be started." + Environment.NewLine + ex.Message, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void menuItemHelpAbout_Click(object sender, EventArgs e)
        {
            formAbout aboutDlg = new formAbout();
            aboutDlg.ShowDialog();
        }

        private void menuItemSelectLanguage_Click(object sender, EventArgs e)
        {
            formSelectLanguage languageDlg = new formSelectLanguage(_language);
            languageDlg.ShowDialog();

            string oldLanguage = _language;
            _language = languageDlg.Language;

            if (oldLanguage != _language)
            {
                // Restart hMailServer Administrator to use the new language.

                string currentExecutable = Assembly.GetExecutingAssembly().Location;

                System.Diagnostics.Process.Start(currentExecutable);

                this.Close();
            }


        }

        private void treeNodes_AfterExpand(object sender, TreeViewEventArgs e)
        {
            Cursor.Current = Cursors.Default;
        }

        public void Repaint()
        {
            this.Refresh();
        }
    }
}
