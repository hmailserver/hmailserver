// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Dialogs;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucRules : UserControl
    {
        private hMailServer.Rules _rulesCollection;

        public ucRules()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

            EnableDisable();
        }


        public void LoadRules(hMailServer.Rules rulesCollection)
        {
            _rulesCollection = rulesCollection;

            ListRules();
        }

        private void ListRules()
        {
            listRules.Items.Clear();

            for (int i = 0; i < _rulesCollection.Count; i++)
            {
                hMailServer.Rule rule = _rulesCollection[i];

                ListViewItem item = listRules.Items.Add(rule.Name);
                item.SubItems.Add(EnumStrings.GetYesNoString(rule.Active));
                item.Tag = rule.ID;

                Marshal.ReleaseComObject(rule);
            }
        }

        private void buttonAddRule_Click(object sender, EventArgs e)
        {
            hMailServer.Rule rule = _rulesCollection.Add();

            formRule ruleDialog = new formRule(rule);

            if (ruleDialog.ShowDialog() == DialogResult.OK)
            {
                rule.Save();

                ListRules();
            }

            Marshal.ReleaseComObject(rule);
        }

        private void buttonEditRule_Click(object sender, EventArgs e)
        {
            EditSelectedItem();
        }

        private void EditSelectedItem()
        {
            if (listRules.SelectedItems.Count <= 0)
                return;

            int id = Convert.ToInt32(listRules.SelectedItems[0].Tag);
            hMailServer.Rule rule = _rulesCollection.get_ItemByDBID(id);

            formRule ruleDialog = new formRule(rule);

            ruleDialog.ShowDialog(this);

            if (ruleDialog.DialogResult == DialogResult.OK)
            {
                rule.Save();

                ListRules();
            }

            Marshal.ReleaseComObject(rule);
        }

        private void buttonDeleteRule_Click(object sender, EventArgs e)
        {
           if (!Utility.AskDeleteItems())
              return;

            foreach (ListViewItem item in listRules.SelectedItems)
            {
                int id = Convert.ToInt32(item.Tag);
                _rulesCollection.DeleteByDBID(id);
            }

            ListRules();
        }

        private void listRules_SelectedIndexChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void EnableDisable()
        {
            buttonEditRule.Enabled = listRules.SelectedItems.Count == 1;
            buttonDeleteRule.Enabled = listRules.SelectedItems.Count > 0;

            buttonMoveUp.Enabled = listRules.SelectedItems.Count == 1;
            buttonMoveDown.Enabled = listRules.SelectedItems.Count == 1;
        }

        private void listRules_DoubleClick(object sender, EventArgs e)
        {
            EditSelectedItem();

            
        }

        private void buttonMoveUp_Click(object sender, EventArgs e)
        {
            if (listRules.SelectedItems.Count != 1)
                return;

            int id = Convert.ToInt32(listRules.SelectedItems[0].Tag);
            hMailServer.Rule rule = _rulesCollection.get_ItemByDBID(id);
            rule.MoveUp();
            ListRules();

            SelectRule(id);
        }

        private void buttonMoveDown_Click(object sender, EventArgs e)
        {
            if (listRules.SelectedItems.Count != 1)
                return;

            int id = Convert.ToInt32(listRules.SelectedItems[0].Tag);
            hMailServer.Rule rule = _rulesCollection.get_ItemByDBID(id);
            rule.MoveDown();
            ListRules();

            SelectRule(id);
        }

        private void SelectRule(int ruleID)
        {
            foreach (ListViewItem item in listRules.Items)
            {
                int id = Convert.ToInt32(item.Tag);

                if (id == ruleID)
                {
                    item.Selected = true;
                    listRules.Focus();
                    break;
                }
            }

            EnableDisable();
        }


    }
}
