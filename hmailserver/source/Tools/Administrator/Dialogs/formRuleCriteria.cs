// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator.Dialogs
{
    public partial class formRuleCriteria : Form
    {
        private hMailServer.RuleCriteria _ruleCriteria;
        private hMailServer.Utilities _utilities;

        public formRuleCriteria()
        {
            InitializeComponent();

            _ruleCriteria = null;

            FillCombos();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
            Strings.Localize(this);

            _utilities = APICreator.Application.Utilities;
        }


        public formRuleCriteria(hMailServer.RuleCriteria ruleCriteria)
        {
            InitializeComponent();

            _ruleCriteria = ruleCriteria;

            FillCombos();
            LoadProperties();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
            Strings.Localize(this);

            _utilities = APICreator.Application.Utilities;
        }

        private void FillCombos()
        {

            comboSearchType.AddItem(EnumStrings.GetMatchTypeString(eRuleMatchType.eMTContains), eRuleMatchType.eMTContains);
            comboSearchType.AddItem(EnumStrings.GetMatchTypeString(eRuleMatchType.eMTEquals), eRuleMatchType.eMTEquals);
            comboSearchType.AddItem(EnumStrings.GetMatchTypeString(eRuleMatchType.eMTGreaterThan), eRuleMatchType.eMTGreaterThan);
            comboSearchType.AddItem(EnumStrings.GetMatchTypeString(eRuleMatchType.eMTLessThan), eRuleMatchType.eMTLessThan);
            comboSearchType.AddItem(EnumStrings.GetMatchTypeString(eRuleMatchType.eMTNotContains), eRuleMatchType.eMTNotContains);
            comboSearchType.AddItem(EnumStrings.GetMatchTypeString(eRuleMatchType.eMTNotEquals), eRuleMatchType.eMTNotEquals);
            comboSearchType.AddItem(EnumStrings.GetMatchTypeString(eRuleMatchType.eMTRegExMatch), eRuleMatchType.eMTRegExMatch);
            comboSearchType.AddItem(EnumStrings.GetMatchTypeString(eRuleMatchType.eMTWildcard), eRuleMatchType.eMTWildcard);

            comboPredefinedField.AddItem(EnumStrings.GetPredefinedFieldString(eRulePredefinedField.eFTBody), eRulePredefinedField.eFTBody);
            comboPredefinedField.AddItem(EnumStrings.GetPredefinedFieldString(eRulePredefinedField.eFTCC), eRulePredefinedField.eFTCC);
            comboPredefinedField.AddItem(EnumStrings.GetPredefinedFieldString(eRulePredefinedField.eFTFrom), eRulePredefinedField.eFTFrom);
            comboPredefinedField.AddItem(EnumStrings.GetPredefinedFieldString(eRulePredefinedField.eFTMessageSize), eRulePredefinedField.eFTMessageSize);
            comboPredefinedField.AddItem(EnumStrings.GetPredefinedFieldString(eRulePredefinedField.eFTRecipientList), eRulePredefinedField.eFTRecipientList);
            comboPredefinedField.AddItem(EnumStrings.GetPredefinedFieldString(eRulePredefinedField.eFTSubject), eRulePredefinedField.eFTSubject);
            comboPredefinedField.AddItem(EnumStrings.GetPredefinedFieldString(eRulePredefinedField.eFTTo), eRulePredefinedField.eFTTo);
            comboPredefinedField.AddItem(EnumStrings.GetPredefinedFieldString(eRulePredefinedField.eFTDeliveryAttempts), eRulePredefinedField.eFTDeliveryAttempts);

        }

        private void LoadProperties()
        {
            comboPredefinedField.SelectedValue = _ruleCriteria.PredefinedField;
            radioPredefinedField.Checked = _ruleCriteria.UsePredefined;
            radioCustomField.Checked = !_ruleCriteria.UsePredefined;
            comboSearchType.SelectedValue = _ruleCriteria.MatchType;
            txtMatchValue.Text = _ruleCriteria.MatchValue;
            txtHeaderField.Text = _ruleCriteria.HeaderField;

            EnableDisable();

        }

        private void SaveProperties()
        {
            _ruleCriteria.PredefinedField = (eRulePredefinedField)comboPredefinedField.SelectedValue;
            _ruleCriteria.UsePredefined = radioPredefinedField.Checked;
            _ruleCriteria.MatchType = (eRuleMatchType)comboSearchType.SelectedValue;
            _ruleCriteria.MatchValue = txtMatchValue.Text;
            _ruleCriteria.HeaderField = txtHeaderField.Text;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            SaveProperties();
        }

        private void radioPredefinedField_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void EnableDisable()
        {
            comboPredefinedField.Enabled = radioPredefinedField.Checked;
            txtHeaderField.Enabled = radioCustomField.Checked;
        }

        private void textTestValue_TextChanged(object sender, EventArgs e)
        {
            RunTest();
        }

        private void radioCustomField_CheckedChanged(object sender, EventArgs e)
        {
            
        }

        private void comboSearchType_SelectionChangeCommitted(object sender, EventArgs e)
        {
            RunTest();
        }

        private void txtMatchValue_TextChanged(object sender, EventArgs e)
        {
            RunTest();
        }

        private void RunTest()
        {
            if (comboSearchType.SelectedValue == null)
            {
                labelTestResult.Text = "";
                return;
            }

            string matchValue = txtMatchValue.Text;
            string testValue = textTestValue.Text;
            eRuleMatchType matchType = (eRuleMatchType)comboSearchType.SelectedValue;

            if (string.IsNullOrEmpty(testValue))
            {
                labelTestResult.Text = "";
                return;
            }

            bool match = _utilities.CriteriaMatch(matchValue, matchType, testValue);

            labelTestResult.Text = match ? Strings.Localize("Match") : Strings.Localize("No match");

        }

        private void formRuleCriteria_FormClosing(object sender, FormClosingEventArgs e)
        {
            Marshal.ReleaseComObject(_utilities);
        }

    }
}